#ifndef DATA_STR
#define DATA_STR
#include "datastr.hpp"
#endif

using namespace std;

unordered_map<string,HardB> hardBlocks;
unordered_map<string,Pad> pads;
unordered_map<string,Net> nets;
float dRatio=0;
int total_A=0,A=0,fpRegion;
int best_w=INT_MAX,best_h=INT_MAX;
mt19937 rng(109);
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
int timeLimit = 595;
string outFile;

void parseInput(const string &Infile);
vector<string> initialFP();
void OutputFile(const string &Outfile);

bool isOperand(string name);
tuple<vector<vector<Node>>,int,int,int> get_min_area(const vector<string>& pe);
void renew_hb_coordinates(vector<vector<Node>>& records,int depth,int at);
vector<Node> stockmeyer(vector<Node>& leftchild, vector<Node>& rightchild, string name, int index);

void swap_adj_operand(vector<string>& pe);
void chain_invert(vector<string>& pe);
void adj_operand_operator_swap(vector<string>& pe);

int get_random_index(int lower,int upper);
pair<vector<string>,int> get_neighbor(vector<string> pe);
tuple<double,double,int,int> get_cost(vector<string>& pe,float a, float b);
vector<string> SA_floorplan(double T, double P, double epsilon, double cooling_rate, int k,float a,float b);

int main(int argc, char* argv[]){
    float a=1,b=175;
    if(argc < 4){
        cerr<<"Method: "<< argv[0] << " intput_file output_file dead_space_ratio"<<endl;
            return 1;
    }
    if(argv[4]){
        try {
            a = stof(argv[4]);
            // cout<<"Input valid, a = "<<a<<endl;
            }
        catch (const invalid_argument& e){cout<<"Invalid a\nDefault a = "<< a <<endl;}
        }
    if(argv[5]){
        try {
            b = stof(argv[5]);
            // cout<<"Input valid, b = "<<b<<endl;
            }
        catch (const invalid_argument& e){cout<<"Invalid b\nDefault b = "<<b<<endl;}
    }
    dRatio = atof(argv[3]);
    parseInput(argv[1]);
    outFile = argv[2];
    cout<<"a = "<<a<<" b = "<<b<<endl;

    // double T=10000000.0,P=80,epsilon=0.0001,cooling_rate=0.99;
    // int k=20; tmp.out 514909
    // double T=10000000.0,P=10,epsilon=0.0001,cooling_rate=0.988;
    // int k=20;  506893
    // double T=10000000.0,P=5,epsilon=0.0001,cooling_rate=0.99;
    // int k=20; tmp2.out 501089
    // double T=10000000.0,P=20,epsilon=0.0001,cooling_rate=0.99;
    // int k=20; hpwl = 501608
    // double T=10000000.0,P=5,epsilon=0.0001,cooling_rate=0.99;
    // int k=30; hpwl = 489021
    // double T=10000000.0,P=2,epsilon=0.0001,cooling_rate=0.99;
    // int k=30; hpwl = 489040
    // double T=10000000.0,P=2.5,epsilon=0.0001,cooling_rate=0.99;
    // int k=35;  hpwl = failed
    // double T=10000000.0,P=3,epsilon=0.0001,cooling_rate=0.985;
    // int k=30;  hpwl = 508520
    // double T=10000.0,P=3,epsilon=0.0001,cooling_rate=0.985;
    // int k=30;  hpwl = failed
    // double T=10000.0,P=5,epsilon=0.0001,cooling_rate=0.99;
    // int k=30;  hpwl = 493924 

    // P=5,rate = 0.92, k=30, hpwl = 498978 
    // P=5,rate = 0.97, k=30, hpwl = 485940 
    // P=10, rate = 0.97, k=30, hpwl = 486593
    // P=10, r=0.9, k=15, T=2000, hpwl=  483692
    // P=1.2 r=0.9, k=15, T=2000, hpwl=  482416
    // P=1.7,r=0.95, k=20, T=2000, hpwl= 489845
    // P=2, r=0.97, k=25, T=2000, hpwl=  487027
    // P=2, r=0.97, k=15, T=2000, hpwl= 481530
    // 481532 
    // Run SA_Floorplanning
    // cout<<"log(P) = "<<log(1.8)<<endl;
    double T=100.0,P=2,epsilon=0.0001,cooling_rate=0.97;
    int k=15;
    vector<string> best_pe = SA_floorplan(T,P,epsilon,cooling_rate,k,a,b);
    auto [cost,penalty,area,wirelength] = get_cost(best_pe,a,b);
    cout<<"Final cost = "<<cost<<" Penalty = "<<penalty<<fixed<<setprecision(2)<<" area = "<<static_cast<double>(area)/cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/cost*100<<"%"<<endl;

    
    OutputFile(argv[2]);
    // cout<<isOperand("H")<<" "<<isOperand("V")<<" "<<isOperand("test")<<endl;

    // for(int i=0;i<10;i++){
    //     swap_adj_operand(pe);
    // }
    // for(int i=0;i<10;i++){
    //     chain_invert(pe);
    // }
    // for(int i=0;i<10;i++){
    //     adj_operand_operator_swap(pe);
    // }

}
int get_random_index(int lower,int upper) {
    uniform_int_distribution<int> dist(lower, upper - 1);
    return dist(rng);
}
double random_double() {     
    uniform_real_distribution<> dis(0.0, 0.99);
    return dis(rng);
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
//SA floorplanning
vector<string> SA_floorplan(double T,double P, double epsilon, double cooling_rate, int k,float a,float b){
    vector<string> pe = initialFP();
    vector<string> best_pe = pe;
    //a = lambda(0~999), b = how much important is penalty(1~999)
    auto [cost,penalty,area,wirelength] = get_cost(best_pe,a,b);
    long long int best_cost = cost;
    cout<<"Initial cost = "<<best_cost<<fixed<<setprecision(2)<<" Penalty = "<<static_cast<double>(penalty)/best_cost*100<<"% area = "<<static_cast<double>(area)/best_cost * 100<<"% wirelength ="<<static_cast<double>(wirelength)/best_cost*100<<"%"<<endl;
    OutputFile(outFile);
    
    int MT=0,uphill=0,reject=0;
    long long int N = k*static_cast<int>(best_pe.size());
    long long int count =0,startcount=0;
    double num_d_cost =0.0;
    double total_delta_cost=0.0;
    double delta_cost=-1.0;
    bool first=false,second=false,start=false;
    long long int tmp=0;

    while(true){
        MT=0;uphill=0;reject=0;
        while(true){
            auto [neighbor,M] = get_neighbor(pe);
            auto [tmp_cost,tmp_penalty,area,wirelength] = get_cost(neighbor,a,b);
            MT++;
            delta_cost = tmp_cost - cost;
            if(delta_cost > 0)tmp+=delta_cost;
	        if(delta_cost > 0 && delta_cost<5000 && (first || second)){total_delta_cost += delta_cost;num_d_cost++;}
            if(delta_cost <= 0 || random_double() < exp(-delta_cost / T)){
                if(delta_cost > 0)uphill++;
                
                pe = neighbor;
                cost = tmp_cost;
                // penalty = tmp_penalty;
                if(cost < best_cost){// && tmp_penalty<=1 && tmp_penalty>=-1){
                    best_pe = pe;
                    best_cost = cost;
                    if(tmp_penalty <= 0){
                        cout<<"Best update !!! M" << M+1 <<" cost="<<best_cost<<" penalty = "<<tmp_penalty<<fixed<<setprecision(2)<<" area = "<<static_cast<double>(area)/best_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/best_cost*100<<"%"<<endl;
                        if(!start && startcount==0)startcount=count;
                        if(!start && (count - startcount) > 3000){start=true;first=true;second=true;T=2000;}
                    }
                }
                // if(count%1000==0)cout<<"M" << M+1 <<" Accept: cost = "<<tmp_cost<<fixed<<setprecision(2)<<" penalty = "<<static_cast<double>(tmp_penalty)/tmp_cost*100<<"% area = "<<static_cast<double>(area)/tmp_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/tmp_cost*100<<"%"<<endl;
            }else {
                reject++;
                // if(count%1000==0)cout<<"M" << M+1 <<" Reject: cost = "<<tmp_cost<<fixed<<setprecision(2)<<" penalty = "<<static_cast<double>(tmp_penalty)/tmp_cost*100<<"% area = "<<static_cast<double>(area)/tmp_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/tmp_cost*100<<"%"<<endl;
            }
            count++;
            if(count==N*10 && !start)T = tmp/(count*fpRegion*fpRegion*10);
            if(uphill > N || MT>2*N ||checkTimeLimit())break;
        }
        T*=cooling_rate;
        
        if(!first && second){
	    second=false;
 	    T = (total_delta_cost/num_d_cost) / log(P);
        // cout<<"T0 = "<<T<<" exp(-delta_cost/T) = "<<exp(-delta_cost / T)<<" delta = "<<delta_cost<<endl;
        }
        if(first)first=false;
        // cout<<"T = "<<T<<" exp(-delta_cost/T) = "<<exp(-delta_cost / T)<<" count = "<<count<<" N = "<<N*15<<" delta = "<<delta_cost<<endl;
            if(reject/MT > 0.95 || T < epsilon || checkTimeLimit())break;
        }
        // cout<<"Total delta_cost = "<<total_delta_cost<<" avg delta cost = "<<total_delta_cost/num_d_cost<<endl;
        return best_pe;
    }

bool isOperand(string name){
    return name!="V" && name!="H";
}
bool isOperator(string name){
    return name=="V" || name=="H";
}

int get_total_hpwl(){
    int total_hpwl=0;
    for (auto& [name,net] : nets) {
        total_hpwl += net.HPWL();
    }
    return total_hpwl;
}

void OutputFile(const string &Outfile){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }

    int total_hpwl=get_total_hpwl();
    cout << "The total HPWL = " << total_hpwl << endl;
    fo << "Wirelength "<<total_hpwl<<endl;
    fo<< "NumHardBlocks "<<hardBlocks.size()<<endl;
    for(auto& [name,hb] : hardBlocks){
        fo<<name <<" "<<hb.coord.x<<" "<<hb.coord.y<<" "<<hb.r<<endl;
    }

    fo.close();
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    cout<<"Total time used = "<< elapsedTime <<"s"<<endl;
}

tuple<double,double,int,int> get_cost(vector<string>& pe,float a, float b){
    int wirelength;
    double penalty=0;
    // b=1;
    auto [records,min_index,min_w,min_h] = get_min_area(pe);
    renew_hb_coordinates(records,records.size()-1,min_index);
    wirelength = get_total_hpwl();

    for(auto [name,hb] : hardBlocks){
        if(hb.coord.x + hb.W() > fpRegion){
            penalty += pow((hb.coord.x + hb.W()) - fpRegion,2);
            // penalty += (min_w - fpRegion);
        }
        if(hb.coord.y + hb.H() > fpRegion){
            penalty += pow((hb.coord.y + hb.H()) - fpRegion,2);
            // penalty += (min_h - fpRegion);
        }
    }

    // cout<<" area = "<<(min_w*min_h) <<" wirelength="<<(a*wirelength) <<" penalty ="<<(b*penalty)<<endl;
    double cost = (a*wirelength) + (b*penalty*min_w*min_h); //(min_w*min_h); 
    
    return make_tuple(cost,(b*penalty*min_w*min_h),min_w*min_h,a*wirelength);
}

pair<vector<string>,int> get_neighbor(vector<string> pe){
    vector<string> neighbor = pe;

    int r_n = get_random_index(0,3);

    switch (r_n){
        case 0:
            swap_adj_operand(neighbor);
            return make_pair(neighbor,r_n);
        case 1:
            chain_invert(neighbor);
            return make_pair(neighbor,r_n);
        case 2:
            adj_operand_operator_swap(neighbor);
            return make_pair(neighbor,r_n);
        default:
            swap_adj_operand(neighbor);
            return make_pair(neighbor,r_n);
    }
}
//M1
void swap_adj_operand(vector<string>& pe){
    int l = get_random_index(0,pe.size()*3) % (pe.size()-1);

    while(isOperator(pe[l])){
        l = get_random_index(0,pe.size()*3) % (pe.size()-1);
    }
    int r = l+1;
    bool found = true;
    while(isOperator(pe[r])){
        r++;
        if(r>= static_cast<int>(pe.size())){
            r=l-1;
            found = false;
            break;
        }
    }
    if(!found){
        while(isOperator(pe[r])){
            r--;
            if (r<0)return;
        }
    }
    // cout<<"M1: "<<pe[l]<< " " << pe[r]<<endl;
    swap(pe[l],pe[r]);
}
//M2
void chain_invert(vector<string>& pe){
    int l = get_random_index(0,pe.size()*3) % pe.size();

    while(isOperand(pe[l])){
        l = get_random_index(0,pe.size()*3) % pe.size();
    }

    // cout<<"M2 : original = "<<pe[l];
    pe[l] = pe[l] == "H" ? "V" : "H";
    // cout<<" after = "<<pe[l]<<endl;
}
//M3
void adj_operand_operator_swap(vector<string>& pe){
    int l = get_random_index(0, pe.size()*3) % (pe.size()-1);
    // bool found = false;
    int r=l+1;
    while(true){
        //(OPERATOR(H,V) , OPERAND(hb))
        if(isOperator(pe[l])){
            r = l+1;
            if(isOperand(pe[r])){break;}
            else{
                l = get_random_index(0, pe.size()*3) % (pe.size()-1);
            }
        }else{// l is OPERAND
            r = l+1;
            if(isOperator(pe[r])){
                int count=0;
                r = l+1;
                for(int i=0;i<=r;i++){
                    if(isOperator(pe[i]))count++;
                }
                if(2*count <= l)break;
                else{
                    l = get_random_index(0,pe.size()*3) % (pe.size()-1);
                    r=l+1;
                }
            }else{
                l = get_random_index(0,pe.size()*3) % (pe.size()-1);
            }
        }
    }

    swap(pe[l],pe[r]);
    int c1=0,c2=0;
    for(int i=0;i<=r;i++){
        if(isOperator(pe[i]))c2++;
        else c1++;
    }
    
    // cout<<"M3: "<<pe[l]<<" "<<pe[r]<<" c1 = "<<c1<<" c2 = "<<c2<<endl;

}

void renew_hb_coordinates(vector<vector<Node>>& records,int depth,int at){
    // To keep the min area only
    records[depth] = {records[depth][at]};

    Node& node = records[depth][0];
    Node& left = records[node.l_from][node.l_at];
    Node& right = records[node.r_from][node.r_at];

    if(node.name == "H"){
        right.coord = node.coord;
        left.coord = Coord(node.coord.x, node.coord.y + right.h);
        renew_hb_coordinates(records,node.r_from,node.r_at);
        renew_hb_coordinates(records,node.l_from,node.l_at);//recursive calculate left and right coordinates
        
    }else if(node.name == "V"){
        left.coord = node.coord;
        right.coord = Coord(node.coord.x + left.w, node.coord.y);
        renew_hb_coordinates(records,node.r_from,node.r_at);
        renew_hb_coordinates(records,node.l_from,node.l_at);//recursive calculate left and right coordinates
        
    }else{
        hardBlocks[node.name].coord = node.coord;// block coordinates
        if(hardBlocks[node.name].w != node.w)hardBlocks[node.name].r = 1;//if width not same then rotate
        else hardBlocks[node.name].r = 0;
    }
}

void print_record(vector<vector<Node>> records){
    for(auto Nodes : records){
        for(auto node : Nodes){
            cout<<node.name<<" ("<<node.w<<","<<node.h<<") ";
        }
        cout<<endl;
    }
    
}

tuple<vector<vector<Node>>,int,int,int> get_min_area(const vector<string>& pe){
    stack<vector<Node>> stk;
    vector<vector<Node>> records;

    for(int i=0;i< static_cast<int>(pe.size());i++){
        if(isOperand(pe[i])){
            HardB hb = hardBlocks[pe[i]];
            int w=hb.w,h=hb.h;
            vector<Node> tmp;
            if(w==h){//only 1 size
                Node node = Node(pe[i],i,w,h,-1,-1,-1,-1,Coord(0,0));
                tmp.emplace_back(node);
            }else{// original and rotate size
                Node node1 = Node(pe[i],i,w,h,-1,-1,-1,-1,Coord(0,0));
                Node node2 = Node(pe[i],i,h,w,-1,-1,-1,-1,Coord(0,0));
                tmp.emplace_back(node1);
                tmp.emplace_back(node2);
                //sort by width large->small
                sort(tmp.begin(),tmp.end(),[](Node a, Node b){return a.w>b.w;});
            }
            stk.push(tmp);
            records.emplace_back(tmp);
        }else{// get left, right from stack and use stockmeyer to calculate all possible shape
            vector<Node> rightchild = stk.top();stk.pop();
            vector<Node> leftchild = stk.top();stk.pop();
            vector<Node> tmp_result = stockmeyer(leftchild,rightchild,pe[i],i);
            stk.push(tmp_result);
            records.emplace_back(tmp_result);
        }
        
    }
    // print_record(records);
    int width,height,area;
    int min_index=-1,min_w=0,min_h=0,min_A = INT_MAX;
    vector<Node> result= stk.top();
    for(int i=0;i< static_cast<int>(result.size());i++){
        width = result[i].w;
        height = result[i].h;
        area = width * height;
        if(area < min_A){
            min_w = width;
            min_h = height;
            min_A = area;
            min_index = i;
        }
    }
    // cout<<"min_w = "<<min_w<<" min_h = "<<min_h<<" Used A = "<<min_w*min_h<<endl;

    return make_tuple(records,min_index,min_w,min_h);

}

vector<Node> stockmeyer(vector<Node>& leftchild, vector<Node>& rightchild, string name, int index){
    vector<Node> result;
    //Width large -> small
    if(name == "H"){
        int i=0,j=0;
        //sort by width, large to small
        // sort(leftchild.begin(),leftchild.end(),[](Node a, Node b){return a.w>b.w;});
        // sort(rightchild.begin(),rightchild.end(),[](Node a, Node b){return a.w>b.w;});
        //width = max(left_w, right_w), height = left_h + right_h
        while(i< static_cast<int>(leftchild.size()) && j< static_cast<int>(rightchild.size())){
            Node node = Node(
                name,index,
                max(leftchild[i].w,rightchild[j].w),
                leftchild[i].h + rightchild[j].h,
                leftchild[i].index, i,
                rightchild[j].index, j,
                Coord(0,0)
            );
        result.emplace_back(node);
        if(leftchild[i].w > rightchild[j].w)i++;
        else if(leftchild[i].w < rightchild[j].w)j++;
        else{i++;j++;}
        }
    }else{ // name = V
        // sort(leftchild.begin(),leftchild.end(),[](Node a, Node b){return a.h>b.h;});
        // sort(rightchild.begin(),rightchild.end(),[](Node a, Node b){return a.h>b.h;});
        int i= static_cast<int>(leftchild.size())-1,j= static_cast<int>(rightchild.size())-1;
        // width = left + right, height = max(left, right)
        while(i>=0 && j>=0){
            Node node = Node(
                name,index,
                leftchild[i].w + rightchild[j].w,
                max(leftchild[i].h,rightchild[j].h),
                leftchild[i].index, i,
                rightchild[j].index, j,
                Coord(0,0)
            );
        result.emplace_back(node);
        if(leftchild[i].h > rightchild[j].h)i--;
        else if(leftchild[i].h < rightchild[j].h)j--;
        else{i--;j--;}
        }
    }

    sort(result.begin(),result.end(),[](Node a, Node b){return a.w>b.w;});
    return result;
}

bool compareByHeight(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.H() > b.second.H();
}
bool compareByWidth(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.W() > b.second.W();
}
bool compareByHeight2(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.H() > b.second.H();
}
bool compareByWidth2(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.W() > b.second.W();
}
bool compareByArea(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.A() > b.second.A();
}

pair<vector<string>,long long int> initialbyHeight(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w > hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByHeight);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int width = hb.W();
        
        if(current_width + width <= fpRegion){
            current_width += width;
            
            pe.emplace_back(name);
            if(isV)isV=false;
            else pe.emplace_back("V");
        }else{ //excess wdith switch row
            current_width = width;
            
            if(isH){
                isH=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("H");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("H");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10); 
   //int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

pair<vector<string>,long long int> initialbyHeight2(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w > hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByHeight2);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int width = hb.W();
        
        if(current_width + width <= fpRegion){
            current_width += width;
            
            pe.emplace_back(name);
            if(isV)isV=false;
            else pe.emplace_back("V");
        }else{ //excess wdith switch row
            current_width = width;
            
            if(isH){
                isH=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("H");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("H");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10); 
   //int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

pair<vector<string>,long long int> initialbyWidth(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w < hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByWidth);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_height=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int height = hb.H();
        
        if(current_height + height <= fpRegion){
            current_height += height;
            
            pe.emplace_back(name);
            if(isH)isH=false;
            else pe.emplace_back("H");
        }else{ //excess wdith switch row
            current_height = height;
            
            if(isV){
                isV=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("V");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("V");
auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);    
//int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}
pair<vector<string>,long long int> initialbyWidth2(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w < hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByWidth2);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_height=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int height = hb.H();
        
        if(current_height + height <= fpRegion){
            current_height += height;
            
            pe.emplace_back(name);
            if(isH)isH=false;
            else pe.emplace_back("H");
        }else{ //excess height switch column
            current_height = height;
            
            if(isV){
                isV=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("V");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("V");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);    
    //int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}
pair<vector<string>,long long int> initialbyArea1(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w > hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByArea);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int width = hb.W();
        
        if(current_width + width <= fpRegion){
            current_width += width;
            
            pe.emplace_back(name);
            if(isV)isV=false;
            else pe.emplace_back("V");
        }else{ //excess wdith switch row
            current_width = width;
            
            if(isH){
                isH=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("H");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("H");
auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    //int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}
pair<vector<string>,long long int> initialbyArea2(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w < hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    shuffle(hbsVector.begin(),hbsVector.end(),rng);
    sort(hbsVector.begin(),hbsVector.end(),compareByArea);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_height=0;

    for(auto [name,hb] : hbsVector){
        // cout<<name<<" ";
        int height = hb.H();
        
        if(current_height + height <= fpRegion){
            current_height += height;
            
            pe.emplace_back(name);
            if(isH)isH=false;
            else pe.emplace_back("H");
        }else{ //excess wdith switch row
            current_height = height;
            
            if(isV){
                isV=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
            }else{
                pe.emplace_back("V");
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                pe.emplace_back(name);
            }
        }
    }
    // cout<<endl;
    pe.emplace_back("V");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10); 
   //int cost2 = get_total_hpwl();
    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

vector<string> get_pe( vector<pair<string, HardB>> hbsVector){
	vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;
    sort(hbsVector.begin(),hbsVector.end(),compareByHeight);
    for(auto [name,hb2] : hbsVector){
        //cout<<name<<" ";
        auto hb = hardBlocks[name];
	int width = hb.w;

        if(current_width + width <= fpRegion){
            current_width += width;

            pe.emplace_back(name);
            if(isV)isV=false;
            else pe.emplace_back("V");
        }else{ //excess wdith switch row
            current_width = width;

            if(isH){
                isH=false;
                pe.emplace_back(name);
                // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
                      }else{
                                             pe.emplace_back("H");
                   // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
                                                                             pe.emplace_back(name);
                                                                                         }
                                                                                                 }
                                                                                                     }
                      //                                                                                   cout<<endl;
                                                                                                             pe.emplace_back("H");
                                                                                                             //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
                                                                                                               // int cost2 = get_total_hpwl();
                                                                                                                    //auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
                                                                                                                        return pe;

}

// vector<string> shuffle_pe(){
//  // vector<pair<string, HardB>> hbsVector;
// vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());  
// //for(auto [name,hb] : hardBlocks)hbsVector.emplace_back(name);

//    vector<string> best_pe = get_pe(hbsVector);
//    auto [cost,penalty,area,wirelength] = get_cost(best_pe,1,10);
//    int best_cost = get_total_hpwl();
   
//   for(int i=0;i<100;i++){
// 	shuffle(hbsVector.begin(),hbsVector.end(),rng);
// 	vector<string> pe = get_pe(hbsVector);
// 	auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
//    	int cost2 = get_total_hpwl();
// 	cout<<"cost = "<<cost<<" cost2 = "<<cost2<<endl;  
// 	if(cost2 < best_cost){
//   	  best_pe=pe;
// 	  best_cost = cost2;
// 	}

//   }
//   return best_pe;
//   //shuffle(hbsVector.begin(),hbsVector.end(),rng);   
// }

vector<string> initialFP(){
    // return shuffle_pe();   
    // return initialbyWidth().first;
    auto [best_pe1, best_cost1] = initialbyHeight();
    auto [best_pe2, best_cost2] = initialbyWidth();
    //auto [best_pe3, best_cost3] = initialbyHeight2();
    //auto [best_pe4, best_cost4] = initialbyWidth2();

    for(int i=0;i<1000;i++){
        auto [pe1, cost1] = initialbyHeight();
        auto [pe2, cost2] = initialbyWidth();
       // auto [pe3, cost3] = initialbyHeight2();
       // auto [pe4, cost4] = initialbyWidth2();

        if(cost1 < best_cost1){
            best_pe1 = pe1;
            best_cost1 = cost1;
        }
        if(cost2 < best_cost2){
            best_pe2 = pe2;
            best_cost2 = cost2;
        }
        //if(cost3 < best_cost3){
        //    best_pe3 = pe3;
        //    best_cost3 = cost3;
        //}
        //if(cost4 < best_cost4){
        //    best_pe4 = pe4;
        //    best_cost4 = cost4;
        //}

    }
    
    vector<pair<vector<string>, long long int>> plans = {
        {best_pe1, best_cost1},
        {best_pe2, best_cost2},
        //{best_pe3, best_cost3},
        //{best_pe4, best_cost4}
    };

    auto best_plan = plans[0];

    for (const auto& plan : plans) {
        if (plan.second < best_plan.second) {
            best_plan = plan;
        }
    }
    for(auto x : best_plan.first)
        cout<<x<<" ";
    cout<<endl;
    return best_plan.first;
}

void parseInput(const string &Infile){
    ifstream fin(Infile);
    if (!fin) {
        cerr << "Cannot open input file." << endl;
        exit(1);
    }
    int numHardBlocks = 0, numPads = 0, numNets = 0;
    string line;
    while (getline(fin, line)) {
        if (line.empty() || line.substr(0,2) == "//")continue;
        istringstream iss(line);
        string key;
        iss >> key;
        if (key == "NumHardBlocks") {
            iss >> numHardBlocks;
        } else if (key == "HardBlock") {
            string name;
            int width, height;
            iss >> name >> width >> height;
            //HardB hb;
            //hb.name = name;
            //hb.w = width;
            //hb.h = height;
            A = width*height;
            total_A += A;
            hardBlocks[name] = HardB(name,Coord(0,0),width,height);
            // hardBlocks.push_back({name, width, height});
        } else if (key == "NumPads") {
            iss >> numPads;
        } else if (key == "Pad") {
            string name;
            int x, y;
            iss >> name >> x >> y;
            //Pad pad;
            //pad.name = name;
            //pad.x = x;
            //pad.y = y;
            pads[name] = Pad(name,Coord(x,y));
            // pads.push_back({name, x, y});
        } else if (key == "NumNets") {
            iss >> numNets;
        } else if (key == "Net") {
            string name;
            int numPins;
            iss >> name >> numPins;
            Net net(name); 
            for (int i = 0; i < numPins; ++i) {
                getline(fin, line);
                if(line.empty() || line.substr(0,2) == "//") { i--; continue; }
                istringstream issPin(line);
                string dummy;
                issPin >>dummy>> key;
                if(pads.find(key)!=pads.end()){
                  net.add_pad(&pads[key]);
                }else{
                  net.add_hardblock(&hardBlocks[key]);
                }
                /*if (key == "Pin"){
                    string pin;
                    issPin >> pin;
                    net.pins.push_back(pin);
                }*/
            }
            nets[name] = net;
            // nets.push_back(net);
        }
    }

    fpRegion = sqrt(total_A * (1+dRatio));

    // cout << "HardBlocks: " << endl;
    // for (const auto& [name,hb] : hardBlocks) {
    //     cout << "Name: " << hb.name << ", Width: " << hb.w << ", Height: " << hb.h << endl;
    // }

    // cout << "\nPads: " << endl;
    // for (const auto& [name,pad] : pads) {
    //     cout << "Name: " << pad.name << ", X: " << pad.coord.x << ", Y: " << pad.coord.y << endl;
    // }

    // cout << "\nNets: " << endl;
    // for (const auto& [name,net] : nets) {
    //     cout << "Net Name: " << net.name << ", Pins: ";
    //     for (const auto& pad : net.pads) {
    //         cout << pad->name << " ";
    //     }
    //     for (const auto& hb : net.hardblocks) {
    //         cout << hb->name << " ";
    //     }
    //     cout << endl;
    // }

    fin.close();
}


// vector<string> initialFP(){
//     for(auto& [name,hb] : hardBlocks){
//         if(hb.w > hb.h)hb.r=1;
//         else hb.r=0;
//     }
//     vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
//     sort(hbsVector.begin(),hbsVector.end(),compareByHeight);

//     vector<string> pe;
//     bool isV=true,isH=true;
//     int current_width=0;

//     for(auto [name,hb] : hbsVector){
//         cout<<name<<" ";
//         int width = hb.W();
        
//         if(current_width + width <= fpRegion){
//             current_width += width;
            
//             pe.emplace_back(name);
//             if(isV)isV=false;
//             else pe.emplace_back("V");
//         }else{ //excess wdith switch row
//             current_width = width;
            
//             if(isH){
//                 isH=false;
//                 pe.emplace_back(name);
//                 // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH1="<<isH<<endl; 
//             }else{
//                 pe.emplace_back("H");
//                 // cout<<"width ="<<current_width<<" max = "<<fpRegion<<" isH2="<<isH<<endl; 
//                 pe.emplace_back(name);
//             }
//         }
//     }
//     cout<<endl;
//     pe.emplace_back("H");
//     // int curX = 0, curY = 0;
//     // int maxHeightInRow = 0; 
//     // for (auto [name,hb2] : hbsVector) {
        
//     //     // 檢查當前區域是否足夠容納該硬塊，若不夠則換行
//     //     HardB& hb = hardBlocks[name];
//     //     cout<<name<<" h="<<hb.H()<<endl;
//     //     if (curX + hb.W() > fpRegion) {
//     //         // 換行，設置新的 y 坐標
//     //         curX = 0;
//     //         curY += maxHeightInRow;
//     //         maxHeightInRow = 0;  // 更新新的最大高度
//     //     }

//     //     // 設定硬塊的坐標
//     //     hb.coord.x = curX;
//     //     hb.coord.y = curY;

//     //     // 更新當前的 x 坐標，為下一個硬塊準備空間
//     //     curX += hb.W();

//     //     // 更新行中最大的高度
//     //     if (hb.H() > maxHeightInRow) {
//     //         maxHeightInRow = hb.H();
//     //     }
//     //     //cout<<curX<<endl;
//     // }
//     return pe;
// }
