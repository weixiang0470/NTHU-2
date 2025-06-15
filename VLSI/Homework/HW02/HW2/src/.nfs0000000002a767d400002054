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
#include <random>
#include <set>
using namespace std;
typedef long long int ll;

struct LibCell {
    string name;
    ll width;
    ll height;
};

struct Technology {
    string name;
    unordered_map<string, LibCell> libCells;
    ll Pmax=0;
};

struct Die {
    string name;
    string tech; 
    ll maxUtil; 
    ll width;
    ll height;
    ll usedArea=0;
    double maxArea; 
};

struct Cell {
    string name;
    string libCname;
    ll id;
    ll pinNum=0; //To calculate pin number
    ll part = -1; // 0 in A, 1 in B
    bool locked=false;
    ll gain=0;
    vector<ll> Nets; // nets_id
    ll areaA=0;
    ll areaB=0;
    ll diffArea() const {
        return abs(areaA - areaB);
    }
};


struct Net {
    string name;
    ll id;
    ll weight;
    ll numC; // number of cell
    ll countA=0;
    ll countB=0;
    bool isCritical=true;
    bool isLocked = false;
    vector<ll> cell_ids;
    // unordered_map<string,string> cellNames;
};

struct BucketList {
    ll max_gain =-1e9;  
    ll min_gain =1e9;   
    unordered_map<ll, list<ll>> buckets;  // gain -> cell-id
    unordered_map<ll, list<ll>::iterator> cell_to_pos;  // cell_id -> position in buckets
};

BucketList Get_bucket(vector<Cell>& cells);
ll getNextMovableCell(BucketList& bl, const vector<Cell>& cells, const Die& dieA, const Die& dieB);
pair<ll, ll> findMaxPartialSum(vector<pair<ll,ll>> moveRecords);

void add_to_die(Cell& cell, Die& die, ll part, vector<Net>& nets);
void initialPartition(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells);

void compute_cell_gain(Cell& cell,vector<Net>& nets,vector<Cell>& cells);
void compute_all_cell_gain(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells);

void Unlock_all_cells_nets(vector<Net>& nets, vector<Cell>& cells);
void Update_bucket_for_cell(ll cell_id, vector<Cell>& cells, BucketList& bl, ll oldGain);
void Update_Gain(ll cell_id, BucketList& bucket,vector<Net>& nets,vector<Cell>& cells);
void update_net_critical(Net& net,vector<Net>& nets,vector<Cell>& cells);


ll get_net_id(const string& name);
ll get_cell_id(const string& name);
void parseInput(const string &Infile,Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells);
void Output_file(const string& Outfile,Die dieA,Die dieB,vector<Net> nets,vector<Cell> cells);

void fm(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells);
bool checkTimeLimit();
ll computeCutsize(vector<Net> nets,vector<Cell> cells);
void print_cells(const string &Outfile, const vector<Cell> cells, const vector<Net> nets);
// void print_techs(unordered_map<string, Technology> techs);
void print_nets(const string &Outfile,vector<Net> nets,vector<Cell> cells);
// void print_bucket(BucketList bl);

ll Sa=0,Sb=0,Pa=0,Pb=0;
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
ll timeLimit = 175;
ll maxG = 0;

mt19937 rng(23);
uniform_real_distribution<double> proDist(0.0,1.0);

unordered_map<string, ll> cell_name_to_id;
vector<string> cell_id_to_name; 
unordered_map<string, ll> net_name_to_id;
vector<string> net_id_to_name; 

unordered_map<string,Technology> techs;
double INITIAL_TEMP=10000.0;
double COOLING_RATE = 0.98;
bool SA=true;

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
          catch (const invalid_argument& e){cout<<"Invalid time limit\nDefault time limit =175 seconds"<<endl;}
      }
      if(argv[4]){
        try {
          maxG = stoi(argv[4]);
          cout<<"Input valid, maxG = "<<maxG<<endl;
          }
          catch (const invalid_argument& e){cout<<"Invalid maxG\nDefault maxG = 0"<<endl;}
      }
//	if(argv[5]=="false")SA=false;
//	else SA=true;
    
    vector<Cell> cells;
    vector<Net> nets;
    Die dieA,dieB;

    parseInput(argv[1],dieA,dieB,nets,cells);
    cout<<"Done parsing. Have "<<cells.size()<<" cells"<<endl;
    initialPartition(dieA,dieB,nets,cells);

    // return 0;

    compute_all_cell_gain(dieA,dieB,nets,cells);
    print_cells(argv[2],cells,nets);
    // print_nets(argv[3],nets,cells);
    // return 0;
	INITIAL_TEMP = cells.size();
    fm(dieA,dieB,nets,cells);
    Output_file(argv[2],dieA,dieB,nets,cells);

    return 0;
}
void fm(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells){
    ll G = maxG + 1;
    ll pass = 0;
    // gain, cell id
    vector<pair<ll,ll>> moveRecords;
    
    while(G>maxG){
        if(checkTimeLimit())break;
        Die tmp_dieA = dieA;
        Die tmp_dieB = dieB;
        vector<Cell> tmp_cells = cells;
        vector<Net> tmp_nets = nets;
        BucketList bl;
        bl = Get_bucket(tmp_cells);
        // cout<<"Before = "<<computeCutsize(tmp_nets,tmp_cells)<<" Max gain = "<<bl.max_gain<<endl;
        // print_bucket(bl);
        // cout<<"Get bucket list done "<<tmp_cells.size()<< endl;
        // Moved cellname , gain
        
        vector<pair<ll, ll>>().swap(moveRecords);

        for(size_t i=0;i<cells.size();i++){
            if(checkTimeLimit())break;
            ll max_gain_cell_id = getNextMovableCell(bl,tmp_cells,tmp_dieA,tmp_dieB);// Get available cell and delete from bucket
            // cout<<"in for "<<max_gain_cell<<endl;
            if (max_gain_cell_id == -1) break; //No max gain cell, break
            // cout<<"Get next move cell"<<endl;

            Cell& cell = tmp_cells[max_gain_cell_id];
            if (cell.locked) continue;
            ll T = (cell.part == 0) ? 1 : 0;
            ll areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
            ll areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;

            // Move cell and update
            cell.part = T;
            cell.locked = true;
            tmp_dieA.usedArea += areaChangeA;
            tmp_dieB.usedArea += areaChangeB;
            
            moveRecords.push_back({cell.gain,cell.id});
            // Update nets
            Update_Gain(max_gain_cell_id,bl,tmp_nets,tmp_cells);
            // cout<<"Update gain done"<<endl;
            // cout<<max_gain_cell_id<<endl;
        }
        // // cout<<"Done pass"<<endl;
        auto [G, bestIndex] = findMaxPartialSum(moveRecords);
        //cout<<" Get max partialsum done, G = "<<G<<" bestid:"<<bestIndex<<endl;
        if(G <= maxG)break;
        //Simulated annealing
        if (SA){
       	 	double temp = INITIAL_TEMP;
		//random_device rd;
		//mt19937 rng(69);
		//uniform_real_distribution<double> proDist(0.0,1.0);
		ll old_sum = G;
		double cool_temp_rate = COOLING_RATE;
		for(ll i=bestIndex+1;i<moveRecords.size();i++){
			ll new_sum = old_sum + moveRecords[i].first;
			ll delta = new_sum - G;
			if(delta > 0){bestIndex++;continue;}
			else if(proDist(rng)<exp(-delta/temp)){
				bestIndex++;
			}
			else break;
			old_sum = new_sum;
			if(i%200==0)cool_temp_rate*=0.999;
			//if(i%100==0)cout<<"temp ="<<cool_temp_rate<<endl;
			temp *= cool_temp_rate;
			if(temp<1e-6)break;
		}
		COOLING_RATE *= 0.99;
        }
	//
        for (ll i = 0; i <= bestIndex; i++) {
              // Move and update
              ll moved_cell_id = moveRecords[i].second;
              Cell& cell = cells[moved_cell_id];
              ll T = (cell.part == 0) ? 1 : 0;
              ll areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
              ll areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;
              //cout<<"Before : "<<moved_cell<< " from "<< cells[moved_cell].part;
              //cout<< " dieA = "<<fixed<<dieA.usedArea<< "dieB = "<< fixed << dieB.usedArea<<endl;
  
              cell.part = T;
              cell.locked = true;
              dieA.usedArea += areaChangeA;
              dieB.usedArea += areaChangeB;
              
              for(const auto& net_id : cell.Nets){
                Net& net = nets[net_id];
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

        //pass++;
        cout<<" After = "<<computeCutsize(nets,cells)<<endl;
        //cout<<" pass : "<<pass<<" G="<<G<<endl;
        Unlock_all_cells_nets(nets,cells);
        compute_all_cell_gain(dieA,dieB,nets,cells);
        // // cout << dieA.usedArea <<"/"<<fixed <<dieA.maxArea << " : " << dieB.usedArea <<"/"<<fixed <<dieB.maxArea<<endl;

    }
}

bool checkTimeLimit() {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    
    if (elapsedTime > timeLimit) {
        //cout << "Execution time exceeded "<<elapsedTime<<" seconds. Exiting..." << endl;
        //Output_file(Outfile,nets,cells,dieA,dieB);
        //exit(0);  
        return true;
    }else return false;
}

void Output_file(const string& Outfile,Die dieA,Die dieB,vector<Net> nets,vector<Cell> cells){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    ll cutsize = computeCutsize(nets,cells);

    fo << "CutSize " <<cutsize<<endl;

    ll dieA_cnt = 0,dieB_cnt = 0;

    for(const auto& cell : cells){
        if(cell.part==0)dieA_cnt++;
        else if(cell.part==1)dieB_cnt++;
    }

    fo << "DieA "<<dieA_cnt<<endl;
    for( const auto& cell : cells){
        if(cell.part == 0){
            fo<<cell.name<<endl;
        }
    }
    fo << "DieB "<<dieB_cnt<<endl;
    for( const auto& cell : cells){
        if(cell.part == 1){
            fo<<cell.name<<endl;
        }
    }

    fo.close();
    auto currentTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    cout<<"\nTotal time used = "<<usedTime<<" Cut size = "<<cutsize<<endl;
}

ll computeCutsize(vector<Net> nets,vector<Cell> cells) {
    ll cutsize = 0;

    for (int i = 0; i < nets.size(); i++) {
        const Net& net = nets[i];
        bool hasA = false, hasB = false;

        for (ll cell_id : net.cell_ids) {
            const Cell& cell = cells[cell_id];
            if (cell.part == 0) hasA = true;
            if (cell.part == 1) hasB = true;
            if (hasA && hasB) break;  
        }

        if (hasA && hasB) cutsize += net.weight;
    }
    return cutsize;
}

pair<ll, ll> findMaxPartialSum(vector<pair<ll,ll>> moveRecords) {
    // cout<<"in partial sum"<<endl;
    int partialSum = 0;
    int maxPartialSum = 0;
    int bestIndex = -1;
    for (int i = 0; i < moveRecords.size(); i++) {
        partialSum += moveRecords[i].first;
        if (partialSum > maxPartialSum) {
            maxPartialSum = partialSum;
            bestIndex = i;
        }
    }
    return {maxPartialSum, bestIndex};
}


ll getNextMovableCell(BucketList& bl, const vector<Cell>& cells, const Die& dieA, const Die& dieB) {
    if (bl.buckets.empty()) return -1;
    ll currentGain = bl.max_gain;
    while (currentGain >= bl.min_gain) {
        auto& bucket = bl.buckets[currentGain];
        if (!bucket.empty()) {

            for (auto it = bucket.begin(); it != bucket.end(); ) {
                ll cell_id = *it;
                const Cell& cell = cells[cell_id];
                if (!cell.locked) {

                    ll T = (cell.part == 0) ? 1 : 0;
                    ll areaChangeA = (T == 0) ? cell.areaA : -cell.areaA;
                    ll areaChangeB = (T == 1) ? cell.areaB : -cell.areaB;
                    if (dieA.usedArea + areaChangeA <= dieA.maxArea &&
                        dieB.usedArea + areaChangeB <= dieB.maxArea) {

                        it = bucket.erase(it);
                        bl.cell_to_pos.erase(cell_id);
                        return cell_id;
                    } else {
                        ++it; 
                    }
                } else {
                    ++it;  
                }
            }
        }
        currentGain--; 
    }
    return -1; 
}


void Unlock_all_cells_nets(vector<Net>& nets, vector<Cell>& cells) {

    for (auto& cell : cells) {
        if (cell.locked) {
            cell.locked = false;
        }
    }


    for (auto& net : nets) {
        net.isLocked = false;
        update_net_critical(net,nets,cells);
    }
}

BucketList Get_bucket(vector<Cell>& cells) {
    BucketList bl;
    bl.max_gain = -1e9;
    bl.min_gain = 1e9;

    for (const Cell& cell : cells) {
        if (cell.locked) continue;
        ll gain = cell.gain;
        if (gain > bl.max_gain) bl.max_gain = gain;
        if (gain < bl.min_gain) bl.min_gain = gain;
        bl.buckets[gain].emplace_back(cell.id);
        bl.cell_to_pos[cell.id] = prev(bl.buckets[gain].end());
    }
    return bl;
}

void Update_bucket_for_cell(ll cell_id, vector<Cell>& cells, BucketList& bl, ll oldGain) {
    Cell& cell = cells[cell_id];
    ll newGain = cell.gain;

    if (bl.cell_to_pos.find(cell_id) != bl.cell_to_pos.end()) {
        auto& oldBucket = bl.buckets[oldGain];
        oldBucket.erase(bl.cell_to_pos[cell_id]);
        bl.cell_to_pos.erase(cell_id);
    }

    bl.buckets[newGain].push_back(cell_id);
    bl.cell_to_pos[cell_id] = prev(bl.buckets[newGain].end());

    if (newGain > bl.max_gain) bl.max_gain = newGain;
    if (newGain < bl.min_gain) bl.min_gain = newGain;
}

void Update_Gain(ll cell_id, BucketList& bucket,vector<Net>& nets,vector<Cell>& cells){ // Update gain after the cell moved to other die, ex: A -> B
    Cell& cell = cells[cell_id];

    for(ll net_id : cell.Nets)
    {  
        Net& net = nets[net_id];
        
         if (!net.isCritical || net.isLocked){
             if(cell.part == 0){
                 net.countA++;
                 net.countB--;
             }
             else if (cell.part == 1){
                 net.countA--;
                 net.countB++;
             }
             update_net_critical(net,nets,cells);
             continue;
        } // If net is locked(both dieA & dieB have locked cell) then skip this net
        else if(cell.part == 0) // Moved from B to A
        {
            // Before move
                
                for(ll other_cid : net.cell_ids)
                {  
                    Cell& other_cell = cells[other_cid];
                    if (other_cell.locked == false && net.countA == 0) // In the same net, free cell,// Before move, T(n) == 0
                    {
                        ll oldGain = other_cell.gain;// Record old gain of other cell
                        other_cell.gain+=net.weight; // then increment gains of all free cells on n
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    }
                    else if(net.countA == 1 && other_cell.locked == false && other_cell.part == 0){ // Before move, T(n) == 1
                        ll oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gain of the only T cell on n, if it is free
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    
                    }
                    
                }
            net.countB--; //F(n)-=1
            net.countA++; //T(n)+=1
            update_net_critical(net,nets,cells);
            if (!net.isCritical)continue;
            //After move
                for(ll other_cid : net.cell_ids)
                {
                    Cell& other_cell = cells[other_cid];
                    if (other_cell.locked == false && net.countB == 0) // In the same net, free cell, //After move, F(n) == 0
                    {
                        ll oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gains of all free cells on n
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 1 &&net.countB == 1){// After move, F(n) == 1
                        ll oldGain = other_cell.gain;
                        other_cell.gain+=net.weight; //increment gain of the only F cell on n, if it is free
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    
                    }
                }

        }
        else    // Moved from A to B
        {
            // Before move

                for(ll other_cid  : net.cell_ids)
                {
                    Cell& other_cell = cells[other_cid];
                    if (other_cell.locked == false && net.countB == 0) // In the same net, free cell // Before move, T(n) == 0
                    {
                        ll oldGain = other_cell.gain;
                        other_cell.gain+=net.weight; // then increment gains of all free cells on n
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 1 && net.countB == 1){// Before move, T(n) == 1 // In the same net, free cell and at T
                        ll oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gain of the only T cell on n, if it is free
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    
                    }
                }

            net.countA--; //F(n)-=1
            net.countB++; //T(n)+=1
            update_net_critical(net,nets,cells);
            if (!net.isCritical)continue;
                for(ll other_cid  : net.cell_ids)
                {
                    Cell& other_cell = cells[other_cid];
                    if (other_cell.locked == false && net.countA == 0) // //After move, F(n) == 0, In the same net, free cell
                    {
                        ll oldGain = other_cell.gain;
                        other_cell.gain-=net.weight; //decrement gains of all free cells on n
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    }
                    else if(other_cell.locked == false && other_cell.part == 0 && net.countA == 1){// After move, F(n) == 1 // In F, free cell (From A->B)
                        ll oldGain = other_cell.gain;
                        other_cell.gain+=net.weight; //increment gain of the only F cell on n, if it is free
                        Update_bucket_for_cell(other_cid,cells,bucket,oldGain);
                    }
                }
            }
        }
    
}

// // Including update net is critical and net is locked
void update_net_critical(Net& net,vector<Net>& nets,vector<Cell>& cells){
    if (net.isLocked) {
        net.isCritical = false;
        return;
    }

    bool hasLockedInA = false;
    bool hasLockedInB = false;

    for (ll cell_id : net.cell_ids) {
        const Cell& cell = cells[cell_id];
        if (cell.locked) {
            if (cell.part == 0) hasLockedInA = true;
            else if (cell.part == 1) hasLockedInB = true;
        }
        if (hasLockedInA && hasLockedInB) break;
    }

    net.isLocked = (hasLockedInA && hasLockedInB);
    net.isCritical = !net.isLocked && 
                    ((net.countA == 1 || net.countB == 1) || 
                     (net.countA == 0 || net.countB == 0));
}

void compute_all_cell_gain(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells) {

    for (int i = 0; i < cells.size(); ++i) {
        compute_cell_gain(cells[i],nets,cells);
    }
}

void compute_cell_gain(Cell& cell,vector<Net>& nets,vector<Cell>& cells) {
    cell.gain = 0;

    for (ll net_id : cell.Nets) {
        const Net& net = nets[net_id];

        if (cell.part == 0) {  
            if (net.countA == 1) cell.gain += net.weight;  
            if (net.countB == 0) cell.gain -= net.weight;  
        } else { 
            if (net.countB == 1) cell.gain += net.weight;
            if (net.countA == 0) cell.gain -= net.weight;
        }
    }
}

void add_to_die(Cell& cell, Die& die, ll part, vector<Net>& nets) {
    cell.part = part;
    if (part == 0) {
        die.usedArea += cell.areaA;  
    } else {
        die.usedArea += cell.areaB;  
    }

    for (ll net_id : cell.Nets) {
        Net& net = nets[net_id];
        net.countA += (part == 0);  
        net.countB += (part == 1);  
    }
}
/*struct Cell {
    string name;
    string libCname;
    ll id;
    ll pinNum=0; //To calculate pin number
    ll part = -1; // 0 in A, 1 in B
    bool locked=false;
    ll gain=0;
    vector<ll> Nets; // nets_id
    ll areaA=0;
    ll areaB=0;
    ll diffArea() const {
        return abs(areaA - areaB);
    }
};


struct Net {
    string name;
    ll id;
    ll weight;
    ll numC; // number of cell
    ll countA=0;
    ll countB=0;
    bool isCritical=true;
    bool isLocked = false;
    vector<ll> cell_ids;
    // unordered_map<string,string> cellNames;
};*/
int cal_a_or_b(Cell& cell,vector<Net>& nets,vector<Cell>& cells){
    int cnt_a=0,cnt_b=0;
    for(auto net_id : cell.Nets){
        cnt_a += nets[net_id].countA;
        cnt_b += nets[net_id].countB;
    }
    return cnt_b > cnt_a;
}
void initialPartition(Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells) {
    
    vector<Cell> sorted_cells = cells;
    sort(sorted_cells.begin(), sorted_cells.end(), [](const Cell& a, const Cell& b) {
        return a.diffArea() > b.diffArea();
    });
// 	size_t start=0;
// 	while(start<sorted_cells.size()){
// 	size_t end = start;
// 		while(end < sorted_cells.size() && sorted_cells[end].diffArea() == sorted_cells[start].diffArea())end++;
// 	shuffle(sorted_cells.begin()+start,sorted_cells.begin()+end,rng);
// 	start = end;
// }
    // cout<< cells[0].areaA - cells[0].areaB <<"  --  "<< cells[2000].areaA - cells[2000].areaB<<endl;

    ll totalAreaA = dieA.maxArea;
    ll totalAreaB = dieB.maxArea;

    ll cnt=0;
    multiset<double> net_weights;
    double ave_net_weight=0.0;
    int cnt2=0;
    for (Cell& srt_cell : sorted_cells) {
        ll cell_id = srt_cell.id;
        Cell& cell = cells[cell_id];
        // cnt++;
        double net_weight = 0.0;
        for(auto net_id : cell.Nets){
            net_weight += nets[net_id].weight;
        }
        // net_weights.insert(net_weight);
        // auto median_iter = net_weights.begin();
        // advance(median_iter, net_weights.size() / 2);
        // double median_net_weight = *median_iter;
        ave_net_weight += net_weight/cell.Nets.size();
        // ave_net_weight = ave_net_weight + (net_weight - ave_net_weight) / cnt;
        if(cnt==1);
        else{
            ave_net_weight/=2.0;
        }
        // if(cnt>= cells.size()*0.3){
        // cout<<median_net_weight<<endl;
            if(net_weight/cell.Nets.size() >= ave_net_weight){
                cnt2++;
                int whr = cal_a_or_b(cell,nets,cells);
                if(whr){
                    if (dieB.usedArea + cell.areaB <= totalAreaB) {
                        add_to_die(cell, dieB, 1, nets);
                    } else if (dieA.usedArea + cell.areaA <= totalAreaA) {
                        add_to_die(cell, dieA, 0, nets);
                    }
                }
                else{
                    if (dieA.usedArea + cell.areaA <= totalAreaA) {
                        add_to_die(cell, dieA, 0, nets);
                    } else if (dieB.usedArea + cell.areaB <= totalAreaB) {
                        add_to_die(cell, dieB, 1, nets);
                    }
                }
            }
            else{
                if (cell.areaA < cell.areaB) {
                    if (dieA.usedArea + cell.areaA <= totalAreaA) {
                        add_to_die(cell, dieA, 0, nets);
                    } else if (dieB.usedArea + cell.areaB <= totalAreaB) {
                        add_to_die(cell, dieB, 1, nets);
                    }
                } else {
                    if (dieB.usedArea + cell.areaB <= totalAreaB) {
                        add_to_die(cell, dieB, 1, nets);
                    } else if (dieA.usedArea + cell.areaA <= totalAreaA) {
                        add_to_die(cell, dieA, 0, nets);
                    }
                }
            }
        // }
        // else{
        //     if (cell.areaA < cell.areaB) {
        //         if (dieA.usedArea + cell.areaA <= totalAreaA) {
        //             add_to_die(cell, dieA, 0, nets);
        //         } else if (dieB.usedArea + cell.areaB <= totalAreaB) {
        //             add_to_die(cell, dieB, 1, nets);
        //         }
        //     } else {
        //         if (dieB.usedArea + cell.areaB <= totalAreaB) {
        //             add_to_die(cell, dieB, 1, nets);
        //         } else if (dieA.usedArea + cell.areaA <= totalAreaA) {
        //             add_to_die(cell, dieA, 0, nets);
        //         }
        //     }
        // }
        
        
        // cout<<"first = "<<ave_net_weight<<endl;
        
        // cout<<"Second = "<<ave_net_weight<<endl;
    }

    for (size_t i = 0; i < nets.size(); ++i) {
        update_net_critical(nets[i],nets,cells);
    }
    cout<<"cnt2 = "<<cnt2<<endl;
}


void print_cells(const string &Outfile, const vector<Cell> cells, const vector<Net> nets){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    for(const auto& cell : cells ){
        fo << cell.name <<" : "<< cell.libCname<< " : "<< cell.pinNum <<" at Die? : "<< cell.part<<" gain="<<cell.gain<<" id="<<cell.id<<endl;
        for(const auto& net_id : cell.Nets){
            fo<<" net:"<< nets[net_id].name <<" id="<<net_id;
        }
        fo<<endl;
    }
    fo.close();
}
// void print_techs(unordered_map<string, Technology> techs){
//     for( const auto& [techname, tech] : techs){
//         cout << techname <<endl;
//         for(const auto& [cellname,cell] : tech.libCells){
//             cout<<cellname<<" "<<cell.width<<" "<<cell.height<<endl;
//         }
//     }
// }
void print_nets(const string &Outfile,vector<Net> nets,vector<Cell> cells){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    for( auto& net : nets ){
        fo << net.name <<" : "<< net.numC << " : "<< net.weight << " countA="<<net.countA<<" countB="<<net.countB<< " netid="<<net.id<<endl;
        for (const auto& cell_id : net.cell_ids ){
            Cell cell = cells[cell_id];
            fo << "  " << cell.name << " id="<<cell.id;
        }
        fo<<endl;
    }
    fo.close();
}
// void print_bucket(BucketList bl){
//     for(auto& [gain,cellname] : bl.buckets)
//     {
//         const auto& cellList = bl.buckets.at(gain);
//         ll count = 0;
//         cout<<"Gain = "<< gain << " -> ";
//         for (const string& cellName : cellList) {
//             cout << cellName;
//             if (++count % 10 == 0)break;
//             else cout<<" -> ";
//         }
//         cout<<endl;
//     }

//     // cout << "cell_to_pos contents:\n";
//     // for (const auto& pair : bl.cell_to_pos) {
//     //     const string& cell_name = pair.first;
//     //     auto it = pair.second;
        
//     //     bool found = false;
//     //     for (const auto& bucket : bl.buckets) {
//     //         const list<string>& lst = bucket.second;
//     //         if (it != lst.end()) {
//     //             found = true;
//     //             cout << "Cell: " << cell_name << " -> " << *it << endl;
//     //             break;
//     //         }
//     //     }

//     //     if (!found) {
//     //         cout << "Cell: " << cell_name << " -> [Invalid iterator]\n";
//     //     }
//     // }
// }

// unordered_map<string, ll> cell_name_to_id;
// vector<string> cell_id_to_name; 
// unordered_map<string, ll> net_name_to_id;
// vector<string> net_id_to_name; 

ll get_cell_id(const string& name) {
    if (cell_name_to_id.find(name) == cell_name_to_id.end()) {
        ll new_id = cell_id_to_name.size(); 
        cell_name_to_id[name] = new_id;
        cell_id_to_name.emplace_back(name);
    }
    return cell_name_to_id[name];
}

ll get_net_id(const string& name) {
    if (net_name_to_id.find(name) == net_name_to_id.end()) {
        ll new_id = net_id_to_name.size(); 
        net_name_to_id[name] = new_id;
        net_id_to_name.emplace_back(name);
    }
    return net_name_to_id[name];
}

void parseInput(const string &Infile,Die& dieA,Die& dieB,vector<Net>& nets,vector<Cell>& cells){
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
            ll numTech;
            iss >> numTech;
            for(ll i=0;i<numTech;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issTech(line);
                string dumm,techName;
                ll numLC;
                issTech >> dumm >> techName >>numLC;
                Technology tech;
                tech.name = techName;
                for(ll j=0;j<numLC;j++){
                    getline(fin, line);
                    if(line.empty() || line.substr(0,2) == "//") { j--; continue; }
                    istringstream issLibCell(line);
                    string libcellname;
                    ll cw,ch;
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
            ll dw,dh;
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
            ll NumCells;
            iss >> NumCells;            
            cells.clear();
            cells.resize(NumCells);
            for(ll i=0;i<NumCells;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issCell(line);
                string dumm,cellname,libcellname;
                issCell >> dumm >> cellname >> libcellname;
                ll id = get_cell_id(cellname);
                cells[id].id = id;
                Cell& cell = cells[id];
                cell.name = cellname; 
                cell.libCname = libcellname;
                if(techs["TA"].libCells.count(cell.libCname) == 1)cell.areaA = techs["TA"].libCells[cell.libCname].width*techs["TA"].libCells[cell.libCname].height;
                if(techs["TB"].libCells.count(cell.libCname) == 1)cell.areaB = techs["TB"].libCells[cell.libCname].width*techs["TB"].libCells[cell.libCname].height;
                // cells.emplace_back(cell);
                // cells[cellname] = cell;
            }
        }
        else if (key == "NumNets"){
            ll NumNets;
            iss >> NumNets;
            nets.clear();
            nets.resize(NumNets);
            for(ll i=0;i<NumNets;i++){
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issNet(line);
                string dumm,netname;
                ll numofcell,netweight;
                issNet >> dumm >> netname >> numofcell >> netweight;
                ll net_id = get_net_id(netname);
                nets[net_id].id = net_id;
                // cout<<net_id<<endl;
                Net& net = nets[net_id];
                net.name = netname;
                net.numC = numofcell;
                net.weight = netweight;
                for(ll j=0;j<numofcell;j++){
                    getline(fin, line);
                    if(line.empty() || line.substr(0,2) == "//") { j--; continue; }
                    istringstream issNetCell(line);
                    string cellname;
                    issNetCell >> dumm >> cellname;
                    ll cell_id = get_cell_id(cellname);
                    // cout<<cell_id<<"  "<<endl;
                    net.cell_ids.emplace_back(cell_id);
                    //net.cellNames[cellname] = cellname;
                    cells[cell_id].pinNum++;
                    cells[cell_id].Nets.emplace_back(net_id);
                    for(auto& [techname, tech] : techs ){
                        if(tech.libCells.count(cells[cell_id].libCname) == 1){
                            if(tech.Pmax < cells[cell_id].pinNum)tech.Pmax = cells[cell_id].pinNum;
                        }
                    }
                }
                // exit(0);
                // nets[netname] = net;
            }
        }
    // cout<<"all parse done "<<key<<endl;
    }
    
    fin.close();
}
