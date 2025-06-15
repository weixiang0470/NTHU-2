#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <list>
#include <algorithm>
#include <queue>
#include <map>
#include <cmath>
#include <chrono>
#include <cstdlib>
#include <omp.h>

using namespace std;
typedef long long int ll;

struct LibCell {
    string name;
    int width;
    int height;
};

struct Technology {
    string name;
    // 以 library cell 名稱作為 key
    unordered_map<string, LibCell> libCells;
    int Pmax=0;
};

struct Die {
    string name;
    string tech; // 使用的技術
    int maxUtil; // 利用率百分比
    ll width;
    ll height;
    ll usedArea=0;
    double maxArea; // 可利用最大面積，依照 DieSize 與 maxUtil 計算
};

struct Cell {
    string name;
    string libCname;
    int pinNum=0; //To calculate pin number
    int part = -1; // 0 in A, 1 in B
    bool locked=false;
    int gain=0;
    unordered_map<string,string> Nets; // 該 cell 連接到哪些 net（索引）
    int areaA=0;
    int areaB=0;
    int diffArea() const {
        return abs(areaA - areaB);
    }
};


struct Net {
    string name;
    int weight;
    int numC; // number of cell
    int countA=0;
    int countB=0;
    bool isCritical=true;
    bool isLocked = false;
    unordered_map<string,string> cellNames; // 連接的 cell 名稱
};

struct BucketList {
    int max_gain =-1e9;  // 當前最大增益值
    int min_gain =1e9;   // 當前最小增益值（可選）
    unordered_map<int, list<string>> buckets;  // 增益值 → 單元索引列表
    unordered_map<string, list<string>::iterator> cell_to_pos;  // 單元索引 → 在桶中的位置
};

string getMaxGainCell(BucketList& bl);
string getNextMovableCell(BucketList& bl, const unordered_map<string, Cell>& tmp_cells,const Die& tmp_dieA, const Die& tmp_dieB);
BucketList Get_bucket(unordered_map<string, Cell>& cells);

void Unlock_all_cells_nets(unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);

void Update_real_cell_Gain(const string& cellname,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);
void Update_Gain(const string& cellname,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells,BucketList& bucket);

void Update_bucket_for_cell(const string& cellname, unordered_map<string, Cell>& cells, BucketList& bucket, const int oldGain);
void update_net_critical(Net& net,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);
void compute_all_cell_gain(unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);
void compute_cell_gain(Cell& cell,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);
void add_into_dieA(Cell& cell,Die& dieA,unordered_map<string, Net>& nets);
void add_into_dieB(Cell& cell,Die& dieB,unordered_map<string, Net>& nets);
void initialPartition(pair<ll,ll> range_r,ll r,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells,Die& dieA,Die& dieB);
void print_cells(const string &Outfile,unordered_map<string, Cell>& cells);
void print_techs(unordered_map<string, Technology> techs);
void print_nets(const string &Outfile,unordered_map<string, Net>& nets);
void print_bucket(BucketList bl);
void parseInput(const string &Infile,unordered_map<string, Technology>& techs,Die& dieA,Die& dieB,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells);

void Update_net_after_move(Cell& cell,int T,unordered_map<string, Net>& nets,unordered_map<string, Cell> cells);
int computeCutsize(const unordered_map<string, Net> nets, const unordered_map<string, Cell> cells);
bool checkTimeLimit();

pair<int, int> findMaxPartialSum(vector<pair<int,string>> moveRecords);

void Output_file(const string& Outfile,const unordered_map<string, Net> nets,const unordered_map<string, Cell> cells,const Die& dieA, const Die& dieB);


int Sa=0,Sb=0,Pa=0,Pb=0;
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
int timeLimit = 175;
int maxG = 0;

int main(int argc, char* argv[]){
    if(argc < 3){
        cerr<<"Method: "<< argv[0] << " intput_file output_file"<<endl;
            return 1;
        }
    if(argv[3]){
        try {
          timeLimit = stoi(argv[3]);
          cout<<"Input valid, time limit = "<<timeLimit<<endl;
          }
          catch (const invalid_argument& e){cout<<"Invalid time limit\nDefault time limit =170 seconds"<<endl;}
      }
      if(argv[4]){
        try {
          maxG = stoi(argv[4]);
          cout<<"Input valid, maxG = "<<maxG<<endl;
          }
          catch (const invalid_argument& e){cout<<"Invalid maxG\nDefault maxG = 200"<<endl;}
      }
	omp_set_num_threads(omp_get_max_threads());

      cout << "Number of threads used: " <<omp_get_max_threads()<< endl;
    //   #pragma omp parallel for schedule(dynamic)
    //   for (int i = 0; i < 32; ++i){
    //     int thread_id = omp_get_thread_num();  // 每個執行緒的 ID
    //     std::cout << "Thread " << thread_id << " is working." << std::endl;
    // }
    
    
    unordered_map<string, Technology> techs;
    Die dieA,dieB;
    unordered_map<string, Cell> cells;
    unordered_map<string, Net> nets;
    parseInput(argv[1],techs,dieA,dieB,nets,cells);
    ll W = (dieA.maxArea + dieB.maxArea);
    ll r = dieA.maxArea / W;
    pair<ll,ll> range_r(r-r*10/100,r+r*10/100);


    //cout<<"Have "<<cells.size()<<" cells"<<endl;
    // cout<<r<<" "<<Sa<<" "<<Sb<<endl;
    // cout<<fixed<<dieA.maxArea<<endl;
    // for( const auto& [techname, tech] : techs){
    //     cout << techname << " : " << tech.Pmax <<endl;
    // }

    initialPartition(range_r,r,nets,cells,dieA,dieB);
    //cout << dieA.usedArea <<"/"<<fixed <<dieA.maxArea << " : " << dieB.usedArea <<"/"<<fixed <<dieB.maxArea<<endl;
    compute_all_cell_gain(nets,cells);
    
    // print_cells(argv[2],cells);
    // cout<< cells.size()<<endl;
    int G = maxG + 1;
    int pass = 0;
    vector<pair<int,string>> moveRecords;
    
    while(G>maxG){
        if(checkTimeLimit())break;
        Die tmp_dieA=dieA;
        Die tmp_dieB=dieB;
        unordered_map<string, Cell> tmp_cells = cells;
        unordered_map<string, Net> tmp_nets=nets;

        // Unlock all cells
        // cout<<"Unlock all cells and nets "<<tmp_cells.size()<<endl;
        

        BucketList bl;
        bl = Get_bucket(tmp_cells);
        //cout<<"Before = "<<computeCutsize(nets,cells)<<" Max gain = "<<bl.max_gain;
        // print_bucket(bl);
        // cout<<"Get bucket list done "<<tmp_cells.size()<< endl;
        // Moved cellname , gain
        
        vector<pair<int, string>>().swap(moveRecords);

        for(size_t i=0;i<tmp_cells.size();i++){
            if(checkTimeLimit())break;
            string max_gain_cell = getNextMovableCell(bl,tmp_cells,tmp_dieA,tmp_dieB);// Get available cell and delete from bucket
            // cout<<"in for "<<max_gain_cell<<endl;
            if (max_gain_cell.empty()) break; //No max gain cell, break
            // cout<<"Get next move cell"<<endl;
            Cell& cell = tmp_cells[max_gain_cell];
            if (cell.locked) continue;
            int T = (cell.part == 0) ? 1 : 0;
            int areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
            int areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;

            // 移動單元並更新狀態
            cell.part = T;
            cell.locked = true;
            tmp_dieA.usedArea += areaChangeA;
            tmp_dieB.usedArea += areaChangeB;
            
            moveRecords.push_back({cell.gain,max_gain_cell});
            // Update nets
            Update_Gain(max_gain_cell,tmp_nets,tmp_cells,bl);
            // cout<<"Update gain done"<<endl;
        }
        // cout<<"Done pass"<<endl;
        auto [G, bestIndex] = findMaxPartialSum(moveRecords);
        // cout<<"Get max partialsum done, G = "<<G<<" pass:"<<bestIndex<<endl;
        if(G <= maxG)break;
        if (checkTimeLimit()){
		        #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i <= bestIndex ; i++) {
              // Move and update
              string moved_cell = moveRecords[i].second;
              Cell& cell = cells[moved_cell];
              int T = (cells[moved_cell].part == 0) ? 1 : 0;
              int areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
              int areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;
              //cout<<"Before : "<<moved_cell<< " from "<< cells[moved_cell].part;
              //cout<< " dieA = "<<fixed<<dieA.usedArea<< "dieB = "<< fixed << dieB.usedArea<<endl;
  
              cell.part = T;
              cell.locked = true;
              #pragma omp critical
              {
              dieA.usedArea += areaChangeA;
              dieB.usedArea += areaChangeB;
              }
              
              for(const auto& [key,netname] : cell.Nets){
                Net& net = nets[netname];
                #pragma omp critical
                {
                    if(cell.part == 0){
                        net.countA++;
                        net.countB--;
                    }
                    else if (cell.part == 1){
                        net.countA--;
                        net.countB++;
                    }
                }
              }
          }
        break;
        }
        #pragma omp parallel for schedule(dynamic)
            for (int i = 0; i <= bestIndex ; i++) {
              // Move and update
              string moved_cell = moveRecords[i].second;
              Cell& cell = cells[moved_cell];
              int T = (cells[moved_cell].part == 0) ? 1 : 0;
              int areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
              int areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;
              //cout<<"Before : "<<moved_cell<< " from "<< cells[moved_cell].part;
              //cout<< " dieA = "<<fixed<<dieA.usedArea<< "dieB = "<< fixed << dieB.usedArea<<endl;
  
              cell.part = T;
              cell.locked = true;
              #pragma omp critical
              {
              dieA.usedArea += areaChangeA;
              dieB.usedArea += areaChangeB;
              }
              
              for(const auto& [key,netname] : cell.Nets){
                Net& net = nets[netname];
                #pragma omp critical
                {
                    if(cell.part == 0){
                        net.countA++;
                        net.countB--;
                    }
                    else if (cell.part == 1){
                        net.countA--;
                        net.countB++;
                    }
                }
              }
          } 

            
        
        pass++;
         cout<<" After = "<<computeCutsize(nets,cells);
         cout<<" pass : "<<pass<<" G="<<G<<endl;
         Unlock_all_cells_nets(nets,cells);
        compute_all_cell_gain(nets,cells);
        // cout << dieA.usedArea <<"/"<<fixed <<dieA.maxArea << " : " << dieB.usedArea <<"/"<<fixed <<dieB.maxArea<<endl;

    }
    // cout<<" Total pass : "<<pass<<endl;
    // int cutsize = computeCutsize(nets,cells);
    // cout << "Cutsize = "<<cutsize<<endl;
    
    Output_file(argv[2],nets,cells,dieA,dieB);
    // cout<<"\nHave "<<cells.size()<<" cells"<<endl;
    // cout<<"\nHave "<<nets.size()<<" nets"<<endl;
     //cout << dieA.usedArea <<"/"<<fixed <<dieA.maxArea << " : " << dieB.usedArea <<"/"<<fixed <<dieB.maxArea<<endl;
    
    return 0;
}
bool checkTimeLimit() {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    
    if (elapsedTime > timeLimit) {
        //cout << "Execution time exceeded "<<elapsedTime<<" seconds. Exiting..." << endl;
        //Output_file(Outfile,nets,cells,dieA,dieB);
        //exit(0);  // 中断并结束程序
        return true;
    }else return false;
}

void Output_file(const string& Outfile,const unordered_map<string, Net> nets,const unordered_map<string, Cell> cells,const Die& dieA, const Die& dieB){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    int cutsize = computeCutsize(nets,cells);

    fo << "CutSize " <<cutsize<<endl;

    int dieA_cnt = 0,dieB_cnt = 0;

    for(const auto& [cellname,cell] : cells){
        if(cell.part==0)dieA_cnt++;
        else if(cell.part==1)dieB_cnt++;
    }

    fo << "DieA "<<dieA_cnt<<endl;
    for( const auto& [cellname,cell] : cells){
        if(cell.part == 0){
            fo<<cellname<<endl;
        }
    }
    fo << "DieB "<<dieB_cnt<<endl;
    for( const auto& [cellname,cell] : cells){
        if(cell.part == 1){
            fo<<cellname<<endl;
        }
    }

    fo.close();
    auto currentTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    cout<<"\nTotal time used = "<<usedTime<<" Cut size = "<<cutsize<<endl;
}


int computeCutsize(unordered_map<string, Net> nets,unordered_map<string, Cell> cells){
    int cutsize = 0;
    
    for (const auto& [netName, net] : nets) {
        bool hasDieA = false, hasDieB = false;
        
        for (const auto& [key,cellname] : net.cellNames) {
            Cell& cell = cells[cellname];
            if (cell.part == 0) hasDieA = true;
            if (cell.part == 1) hasDieB = true;

            if (hasDieA && hasDieB) { // 該 net 連接到兩個不同 partition
                cutsize+= net.weight;
                break; // 計入後不需要繼續檢查這個 net
            }
        }
    }
    return cutsize;
}

pair<int, int> findMaxPartialSum(vector<pair<int,string>> moveRecords) {
    int partialSum = 0;
    int maxPartialSum = 0;
    int bestIndex = -1;
    for (size_t i = 0; i < moveRecords.size(); i++) {
        partialSum += moveRecords[i].first;
        if (partialSum > maxPartialSum) {
            maxPartialSum = partialSum;
            bestIndex = i;
        }
    }
    return {maxPartialSum, bestIndex};
}

void Update_net_after_move(Cell& cell,int T,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
    int countChangeA = (T == 0) ? 1 : -1;
    int countChangeB = (T == 1) ? 1 : -1;
    for(auto& [key,netname] : cell.Nets){
      Net& net= nets[netname];
        net.countA += countChangeA;
        net.countB += countChangeB;
        update_net_critical(net,nets,cells);
    }
}

string getNextMovableCell(BucketList& bl, const unordered_map<string, Cell>& tmp_cells,const Die& tmp_dieA, const Die& tmp_dieB){
    // 從目前最大 gain 桶開始遍歷所有 candidate
    if (bl.buckets.empty()) return "";
    // cout<<"not empty ="<<" "<<bl.buckets.size()<<endl;
    while (bl.max_gain >= bl.min_gain) {
        int currentGain = bl.max_gain;
        if (bl.buckets.find(currentGain) != bl.buckets.end()) {
            // 遍歷桶中所有 cell
            for (const string& cellName : bl.buckets.at(currentGain)) {
                const Cell& cell = tmp_cells.at(cellName);
                // 如果 cell 未鎖定且面積條件符合，就返回
                int T = (cell.part == 0) ? 1 : 0;
                int areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
                int areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;
                if (!cell.locked &&
                    (tmp_dieA.usedArea + areaChangeA <= tmp_dieA.maxArea) &&
                    (tmp_dieB.usedArea + areaChangeB <= tmp_dieB.maxArea)) {
                    string result = cellName;
                    //Get available cell, delete cell from bucket and return cellname
                    bl.buckets[currentGain].erase(bl.cell_to_pos[cellName]);
                    bl.cell_to_pos.erase(cellName);
                    return result;
                }
            }
        }
        bl.max_gain--; 
    }
    // cout<<"is empty ="<<endl;
    return "";
}

string getMaxGainCell(BucketList& bl) {
    if (bl.buckets.empty()) return "";  // 無可用單元
    
    // 從最高增益桶開始查找
    while (bl.max_gain >= -1e9) {  // 假設增益最小值不會低於 -1e9
        if (!bl.buckets[bl.max_gain].empty()) {
            string cellName = bl.buckets[bl.max_gain].front();
            return cellName;
        }
        bl.max_gain--;  // 桶空，降低 max_gain
    }
    return "";
}

void Unlock_all_cells_nets(unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
//Parallel
    vector<reference_wrapper<Cell>> cellsVec;
    for (auto& [cellname, cell] : cells) {
      cellsVec.emplace_back(cell);
    }
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < cellsVec.size(); ++i) {
    Cell& cell = cellsVec[i].get();
    cell.locked = false;
    }
    vector<reference_wrapper<Net>> netsVec;
    for (auto& [netname, net] : nets) {
      netsVec.emplace_back(net);
    }
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < netsVec.size(); ++i) {
      Net& net = netsVec[i].get();
      net.isLocked = false; 
      update_net_critical(net,nets,cells);
    }
    //Original
    /*for( auto& [cellname,cell] : cells ){
        if(cell.locked)cell.locked=false;
    }
    for( auto& [netname, net] : nets)
    {
        net.isLocked = false;
        update_net_critical(netname,nets,cells);
    }*/
}

BucketList Get_bucket(unordered_map<string, Cell>& cells){
    BucketList bl;
    for (const auto& [cellname , cell] : cells)
    {
        // Initial bucket list
        if(cell.locked)continue;
        int gain = cell.gain;
        bl.buckets[gain].push_back(cell.name);
        bl.cell_to_pos[cell.name] = prev(bl.buckets[gain].end());
        bl.max_gain = max(bl.max_gain, gain);
        bl.min_gain = min(bl.min_gain, gain);
    }
    return bl;
}

void Update_bucket_for_cell(const string& cellname, unordered_map<string, Cell>& cells, BucketList& bucket, const int oldGain) {
    // 先找到該 cell 在 bucket 中原來的位置
    if (bucket.cell_to_pos.find(cellname) != bucket.cell_to_pos.end()) {
        // int oldGain = cells[cellname].gain; // 原 gain，這裡假設你已經在之前保存了舊的 gain
        // 從舊桶中移除
        bucket.buckets[oldGain].erase(bucket.cell_to_pos[cellname]);
        bucket.cell_to_pos.erase(cellname);
    }
    // 計算新的 gain (假設 compute_cell_gain 已經呼叫過了)
    int newGain = cells[cellname].gain;
    // 插入到新的桶中
    bucket.buckets[newGain].push_back(cellname);
    // 更新映射，記錄該 cell 在桶中的 iterator
    bucket.cell_to_pos[cellname] = --bucket.buckets[newGain].end();
    // 更新 bucket.max_gain 與 bucket.min_gain
    bucket.max_gain = max(bucket.max_gain, newGain);
    bucket.min_gain = min(bucket.min_gain, newGain);
}

void Update_Gain(const string& cellname,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells,BucketList& bucket){ // Update gain after the cell moved to other die, ex: A -> B
    Cell& cell = cells[cellname];
    for(const auto& [key,netname] : cell.Nets)
    {  
        Net& net = nets[netname];
        
         if (!net.isCritical || net.isLocked){
             if(cell.part == 0){
                 net.countA++;
                 net.countB--;
             }
             else if (cells[cellname].part == 1){
                 net.countA--;
                 net.countB++;
             }
             update_net_critical(net,nets,cells);
             continue;
         } // If net is locked(both dieA & dieB have locked cell) then skip this net
        else if(cell.part == 0) // Moved from B to A
        {
            // Before move
                
                for(const auto& [key2,othercell]: net.cellNames)
                {  
                    Cell& other_cell = cells[othercell];
                    if (other_cell.locked == false && net.countA == 0) // In the same net, free cell,// Before move, T(n) == 0
                    {
                        int oldGain = other_cell.gain;// Record old gain of other cell
                        other_cell.gain+=net.weight; // then increment gains of all free cells on n
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    }
                    else if(net.countA == 1 && other_cell.locked == false && other_cell.part == 0){ // Before move, T(n) == 1
                        int oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gain of the only T cell on n, if it is free
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    
                    }
                    
                }
            net.countB--; //F(n)-=1
            net.countA++; //T(n)+=1
            update_net_critical(net,nets,cells);
            if (!net.isCritical)continue;
            //After move
                for(const auto& [key2,othercell]: net.cellNames)
                {
                    Cell& other_cell = cells[othercell];
                    if (other_cell.locked == false && net.countB == 0) // In the same net, free cell, //After move, F(n) == 0
                    {
                        int oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gains of all free cells on n
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 1 &&net.countB == 1){// After move, F(n) == 1
                        int oldGain = other_cell.gain;
                        other_cell.gain+=net.weight; //increment gain of the only F cell on n, if it is free
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    
                    }
                }

        }
        else    // Moved from A to B
        {
            // Before move

                for(const auto& [key2,othercell]: net.cellNames)
                {
                    Cell& other_cell = cells[othercell];
                    if (other_cell.locked == false && net.countB == 0) // In the same net, free cell // Before move, T(n) == 0
                    {
                        int oldGain = other_cell.gain;
                        other_cell.gain+=net.weight; // then increment gains of all free cells on n
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 1 && net.countB == 1){// Before move, T(n) == 1 // In the same net, free cell and at T
                        int oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gain of the only T cell on n, if it is free
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    
                    }
                }

            net.countA--; //F(n)-=1
            net.countB++; //T(n)+=1
            update_net_critical(net,nets,cells);
            if (!net.isCritical)continue;
                for(const auto& [key2,othercell]: nets[key].cellNames)
                {
                    Cell& other_cell = cells[othercell];
                    if (other_cell.locked == false && net.countA == 0) // //After move, F(n) == 0, In the same net, free cell
                    {
                        int oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gains of all free cells on n
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 0 && net.countA == 1){// After move, F(n) == 1 // In F, free cell (From A->B)
                        int oldGain =other_cell.gain;
                        other_cell.gain+=net.weight; //increment gain of the only F cell on n, if it is free
                        Update_bucket_for_cell(othercell,cells,bucket,oldGain);
                    }
                }
            }
        }
    
}

// Including update net is critical and net is locked
void update_net_critical(Net& net,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
    if(net.isLocked == true){net.isCritical=false;return;}
    bool CLinA=false,CLinB=false;
    for(auto& [key,cellname] : net.cellNames)
    {
        Cell& cell = cells[cellname];
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
        if (elapsedTime > timeLimit)return;
        if(cell.locked){
            if(cell.part==0)CLinA=true;
            if(cell.part==1)CLinB=true;
        }
    }
    if(CLinA && CLinB){net.isLocked = true;net.isCritical=false;return;}
    else if(net.countA == 0 || net.countA == 1 || net.countB == 0 || net.countB == 1)
    {net.isCritical = true;return;}
    else
    {net.isCritical = false;return;}
}

void compute_all_cell_gain(unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
    vector<reference_wrapper<Cell>> cellsVec;
    for(auto& [cellname,cell] : cells){
      cellsVec.emplace_back(cell);
    }
    
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < cellsVec.size(); ++i) {
    Cell& cell = cellsVec[i].get();
    int gain = 0;
    if (cell.part == 0) {
      for (const auto& [key, netname] : cell.Nets) {
        const Net& net = nets.at(netname);
        if (net.countA == 1) gain+=nets[netname].weight;
        if (net.countB == 0) gain-=nets[netname].weight;
      }
    } else {
      for (const auto& [key, netname] : cell.Nets) {
        const Net& net = nets.at(netname);
        if (net.countB == 1) gain+=nets[netname].weight;
        if (net.countA == 0) gain-=nets[netname].weight;
      }
    }
    cell.gain = gain;
    }
    /*for (const auto& [cellname , cell] : cells)
    {
    
        compute_cell_gain(cellname,nets,cells);
        // // Initial bucket list
        // if(cell.locked)continue;
        // int gain = cell.gain;
    }*/
}

void compute_cell_gain(Cell& cell,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
      cell.gain =0;
     // cell in dieA
       
        for ( const auto& [key,netname] : cell.Nets)
        {   
            Net& net = nets[netname];
            if(cell.part == 0){  // From A to B
                if(net.countA == 1)cell.gain+=net.weight;
                if(net.countB == 0)cell.gain-=net.weight;
            }
            else if(cell.part == 1){ // From B to A
              if(net.countB == 1)cell.gain+=net.weight;
              if(net.countA == 0)cell.gain-=net.weight;

            }
            else
            {
                cout<<"Cell : "<<cell.name<<" is not in dieA or dieB!!!"<<endl;
                return;
            }
            
        }
}

void add_into_dieA(Cell& cell,Die& dieA,unordered_map<string, Net>& nets){
    cell.part = 0;
    dieA.usedArea += cell.areaA;
    for(auto& [netname,net] : cell.Nets)
    {
        nets[netname].countA++;
    }
}

void add_into_dieB(Cell& cell,Die& dieB,unordered_map<string, Net>& nets){
    cell.part = 1;
    dieB.usedArea += cell.areaB;
    for(auto& [netname,net] : cell.Nets)
    {
        nets[netname].countB++;
    }
}

void initialPartition(pair<ll,ll> range_r,ll r,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells,Die& dieA,Die& dieB){
    // cout<<ran_put_max<<endl;
    vector<pair<string, Cell>> cellVector(cells.begin(), cells.end());
    sort(cellVector.begin(), cellVector.end(), [](const pair<string, Cell>& a, const pair<string, Cell>& b) {
        return a.second.diffArea() > b.second.diffArea(); // 比较总面积
    });
    
    for ( auto& [cellname, cell] : cellVector) {
      //cout<<"Diff = "<<cell.diffArea()<<endl;
        Cell& cell2 = cells[cellname];
       if (cell.areaA < cell.areaB && dieA.usedArea + cell.areaA <= dieA.maxArea)add_into_dieA(cell2,dieA,nets);
       else if (cell.areaB < cell.areaA && dieB.usedArea + cell.areaB <= dieB.maxArea)add_into_dieB(cell2,dieB,nets);
       else if(dieA.usedArea + cell.areaA <= dieA.maxArea)add_into_dieA(cell2,dieA,nets);
       else add_into_dieB(cell2,dieB,nets);
    }
    

    for( auto& [netname, net] : nets)
    {
        update_net_critical(net,nets,cells);
    }
}

void print_cells(const string &Outfile,unordered_map<string, Cell>& cells){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    for(const auto& [cellname,cell] : cells ){
        fo << cellname <<" : "<< cell.libCname<< " : "<< cell.pinNum <<" at Die? : "<< cell.part<<" gain="<<cell.gain<<endl;
    }
    fo.close();
}

void print_techs(unordered_map<string, Technology> techs){
    for( const auto& [techname, tech] : techs){
        cout << techname <<endl;
        for(const auto& [cellname,cell] : tech.libCells){
            cout<<cellname<<" "<<cell.width<<" "<<cell.height<<endl;
        }
    }
}

void print_nets(const string &Outfile,unordered_map<string, Net>& nets){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    for( auto& [netname,net] : nets ){
        cout << netname <<" : "<< net.numC << " : "<< net.weight << " countA="<<net.countA<<" countB="<<net.countB<<endl;
        for (const auto& [cell,cellname] : net.cellNames ){
            cout << cellname <<endl;
        }
    }
    fo.close();
}

void print_bucket(BucketList bl){
    for(auto& [gain,cellname] : bl.buckets)
    {
        const auto& cellList = bl.buckets.at(gain);
        int count = 0;
        cout<<"Gain = "<< gain << " -> ";
        for (const string& cellName : cellList) {
            cout << cellName;
            if (++count % 10 == 0)break;
            else cout<<" -> ";
        }
        cout<<endl;
    }

    // cout << "cell_to_pos contents:\n";
    // for (const auto& pair : bl.cell_to_pos) {
    //     const string& cell_name = pair.first;
    //     auto it = pair.second;
        
    //     // 確保迭代器來自 `buckets`，並輸出對應的值
    //     bool found = false;
    //     for (const auto& bucket : bl.buckets) {
    //         const list<string>& lst = bucket.second;
    //         if (it != lst.end()) {
    //             found = true;
    //             cout << "Cell: " << cell_name << " -> " << *it << endl;
    //             break;
    //         }
    //     }

    //     if (!found) {
    //         cout << "Cell: " << cell_name << " -> [Invalid iterator]\n";
    //     }
    // }
}

void parseInput(const string &Infile,unordered_map<string, Technology>& techs,Die& dieA,Die& dieB,unordered_map<string, Net>& nets,unordered_map<string, Cell>& cells){
    ifstream fin(Infile);
    if (!fin) {
        cerr << "Cannot open input file." << endl;
        exit(1);
    }

    string line;
    while(getline(fin, line)){
        if (line.empty() || line.substr(0,2) == "//")continue;
        istringstream iss(line);
        string key;
        iss >> key;
        if(key == "NumTechs"){
            int numTech;
            iss >> numTech;
            for(int i=0;i<numTech;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issTech(line);
                string dumm,techName;
                int numLC;
                issTech >> dumm >> techName >>numLC;
                Technology tech;
                tech.name = techName;
                for(int j=0;j<numLC;j++){
                    getline(fin, line);
                    if(line.empty() || line.substr(0,2) == "//") { j--; continue; }
                    istringstream issLibCell(line);
                    string libcellname;
                    int cw,ch;
                    issLibCell >> dumm >> libcellname >> cw >> ch;
                    if (techName=="TA"){if (cw*ch > Sa)Sa=cw*ch;}
                    else if(techName=="TB"){if (cw*ch > Sb)Sb=cw*ch;}
                    
                    LibCell libcell;
                    libcell.name = libcellname;
                    libcell.width = cw;
                    libcell.height = ch;
                    tech.libCells[libcellname] = libcell;
                }
                techs[techName] = tech;
            }
        }
        else if (key == "DieSize"){
            int dw,dh;
            iss >> dw >> dh;
            dieA.width = dw; dieA.height = dh;
            dieB.width = dw; dieB.height = dh;
        }
        else if (key == "DieA"){
            iss >> dieA.tech >> dieA.maxUtil;
            dieA.name = key;
            dieA.maxArea = dieA.width * dieA.height * dieA.maxUtil / 100;
        }
        else if (key == "DieB"){
            iss >> dieB.tech >> dieB.maxUtil;
            dieB.name = key;
            dieB.maxArea = dieB.width * dieB.height * dieB.maxUtil / 100;
        }
        else if (key == "NumCells"){
            int NumCells;
            iss >> NumCells;
            for(int i=0;i<NumCells;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issCell(line);
                string dumm,cellname,libcellname;
                issCell >> dumm >> cellname >> libcellname;
                Cell cell;
                cell.name = cellname; 
                cell.libCname = libcellname;
                if(techs["TA"].libCells.count(cell.libCname) == 1)cell.areaA = techs["TA"].libCells[cell.libCname].width*techs["TA"].libCells[cell.libCname].height;
                if(techs["TB"].libCells.count(cell.libCname) == 1)cell.areaB = techs["TB"].libCells[cell.libCname].width*techs["TB"].libCells[cell.libCname].height;
                cells[cellname] = cell;
            }
        }
        else if (key == "NumNets"){
            int NumNets;
            iss >> NumNets;
            for(int i=0;i<NumNets;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issNet(line);
                string dumm,netname;
                int numofcell,netweight;
                issNet >> dumm >> netname >> numofcell >> netweight;
                Net net;
                net.name = netname;
                net.numC = numofcell;
                net.weight = netweight;
                for(int j=0;j<numofcell;j++){
                    getline(fin, line);
                    if(line.empty() || line.substr(0,2) == "//") { j--; continue; }
                    istringstream issNetCell(line);
                    string cellname;
                    issNetCell >> dumm >> cellname;
                    net.cellNames[cellname] = cellname;
                    cells[cellname].pinNum++;
                    cells[cellname].Nets[netname] = netname;
                    for(auto& [techname, tech] : techs ){
                        if(tech.libCells.count(cells[cellname].libCname) == 1){
                            if(tech.Pmax < cells[cellname].pinNum)tech.Pmax = cells[cellname].pinNum;
                        }
                    }
                }
                nets[netname] = net;
            }
        }
    }
    fin.close();
}

// void Update_Gain_test(const string& cellname, unordered_map<string, Net>& nets, unordered_map<string, Cell>& cells, BucketList& bucket) {
//     Cell& movedCell = cells[cellname];
//     int oldPart = (movedCell.part == 0) ? 1 : 0; // 移動前的 Die
//     int newPart = 1 - oldPart; // 移動後的 Die

//     // 鎖定移動的單元
//     movedCell.locked = true;

//     for (const auto& [netKey, netName] : movedCell.Nets) {
//         Net& net = nets[netName];
        
//         // 跳過非關鍵網表或已鎖定網表
//         if (!net.isCritical || net.isLocked) continue;

//         // 記錄移動前的切割狀態
//         bool wasCutBefore = (net.countA > 0 && net.countB > 0);

//         // 更新網表狀態
//         if (newPart == 0) { // 移動到 DieA
//             net.countA++;
//             net.countB--;
//         } else { // 移動到 DieB
//             net.countA--;
//             net.countB++;
//         }

//         // 更新後的切割狀態
//         bool isCutAfter = (net.countA > 0 && net.countB > 0);

//         // 更新所有相關單元的增益
//         for (const auto& [otherCellName, _] : net.cellNames) {
//             Cell& otherCell = cells[otherCellName];
//             if (otherCell.locked) continue;

//             // 計算增益變化：F(net_before) - F(net_after)
//             int delta = (wasCutBefore - isCutAfter) * net.weight;
//             int oldGain = otherCell.gain;
//             otherCell.gain += delta;

//             // 更新 Bucket List
//             Update_bucket_for_cell(otherCellName, cells, bucket, oldGain);
//         }

//         // 更新網表的關鍵狀態
//         update_net_critical(netName, nets, cells);
//     }
// }
// void Update_real_cell_Gain_test(const string& cellname, unordered_map<string, Net>& nets, unordered_map<string, Cell>& cells) {
//     Cell& movedCell = cells[cellname];
//     int oldPart = (movedCell.part == 0) ? 1 : 0; // 移動前的 Die
//     int newPart = 1 - oldPart; // 移動後的 Die

//     // 鎖定移動的單元
//     movedCell.locked = true;

//     for (const auto& [netKey, netName] : movedCell.Nets) {
//         Net& net = nets[netName];
        
//         // 跳過非關鍵網表或已鎖定網表
//         if (!net.isCritical || net.isLocked) continue;

//         // 記錄移動前的切割狀態
//         bool wasCutBefore = (net.countA > 0 && net.countB > 0);

//         // 更新網表狀態
//         if (newPart == 0) { // 移動到 DieA
//             net.countA++;
//             net.countB--;
//         } else { // 移動到 DieB
//             net.countA--;
//             net.countB++;
//         }

//         // 更新後的切割狀態
//         bool isCutAfter = (net.countA > 0 && net.countB > 0);

//         // 更新所有相關單元的增益
//         for (const auto& [otherCellName, _] : net.cellNames) {
//             Cell& otherCell = cells[otherCellName];
//             if (otherCell.locked) continue;

//             // 計算增益變化：F(net_before) - F(net_after)
//             int delta = (wasCutBefore - isCutAfter) * net.weight;
//             int oldGain = otherCell.gain;
//             otherCell.gain += delta;

//             // 更新 Bucket List
//             // Update_bucket_for_cell(otherCellName, cells, bucket, oldGain);
//         }

//         // 更新網表的關鍵狀態
//         update_net_critical(netName, nets, cells);
//     }
// }
