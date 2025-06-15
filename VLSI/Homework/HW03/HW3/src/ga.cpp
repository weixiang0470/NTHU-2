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
mt19937 rng(88);
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
tuple<long long int,long long int,int,int> get_cost(vector<string>& pe,float a, float b);
vector<string> SA_floorplan(double T, double P, double epsilon, double cooling_rate, int k,float a,float b);

TreeNode* build_tree_from_postorder(const vector<string>& pe);
void print_tree(TreeNode* root, int depth = 0);

int main(int argc, char* argv[]){
    float a=0.9,b=1;
    if(argc < 4){
        cerr<<"Method: "<< argv[0] << " intput_file output_file dead_space_ratio"<<endl;
            return 1;
    }
    if(argv[4]){
        try {
            a = stof(argv[4]);
            // cout<<"Input valid, a = "<<a<<endl;
            }
        catch (const invalid_argument& e){cout<<"Invalid a\nDefault maxG = 1"<<endl;}
        }
    if(argv[5]){
        try {
            b = stof(argv[5]);
            // cout<<"Input valid, b = "<<b<<endl;
            }
        catch (const invalid_argument& e){cout<<"Invalid b\nDefault maxG = 200"<<endl;}
    }
    dRatio = atof(argv[3]);
    parseInput(argv[1]);

    // Run GA_Floorplanning
    double T=10000000.0,P=0.95,epsilon=0.0001,cooling_rate=0.99;
    int k=10;
    
    auto pe = initialFP();
    // 顯示排佈結果
    cout << "Postorder layout: ";
    for (const auto& item : pe) {
        cout << item << " ";
    }
    cout << endl;

    // 使用 `pe` 建立樹結構
    TreeNode* root = build_tree_from_postorder(pe);
    
    // 打印樹形結構
    cout << "Tree structure:" << endl;
    print_tree(root);

    outFile = argv[2];
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

TreeNode* build_tree_from_postorder(const vector<string>& pe) {
    stack<TreeNode*> stk;
    
    for (const string& val : pe) {
        if (val == "V" || val == "H") {
            // Create a new internal node
            TreeNode* node = new TreeNode(val);
            
            // Pop two nodes for the left and right children
            node->R = stk.top(); stk.pop();
            node->L = stk.top(); stk.pop();
            
            // Push the current node back onto the stack
            stk.push(node);
        } else {
            // It's a hardblock, create a leaf node
            stk.push(new TreeNode(val));
        }
    }
    
    // The final tree is the only item left in the stack
    return stk.top();
}

// 替換右子樹中的重複硬塊
void replace_duplicates(TreeNode* root, unordered_set<string>& seen_blocks) {
    if (root == nullptr) return;

    // 如果當前節點的值已經出現過，就替換它
    if (seen_blocks.find(root->value) != seen_blocks.end()) {
        // 替換成新的硬塊
        static int new_block_counter = 0;
        root->value = "new_block_" + to_string(new_block_counter++);
    }
    
    // 將當前節點的值標記為已經出現過
    seen_blocks.insert(root->value);

    // 遞迴處理左子樹和右子樹
    replace_duplicates(root->left, seen_blocks);
    replace_duplicates(root->right, seen_blocks);
}

// 交換兩棵樹的左右子樹
pair<TreeNode*, TreeNode*> swap_subtrees(TreeNode* root1, TreeNode* root2) {
    if (root1 == nullptr || root2 == nullptr) return nullptr;

    // 交換 root1 的左子樹和 root2 的右子樹
    TreeNode* temp = root1->L;
    root1->L = root2->R;
    root2->R = temp;

    return {root1,root2}; // 返回修改後的樹
}

void print_tree(TreeNode* root, int depth = 0) {
    if (!root) return;
    
    // Print right subtree first (so it's on the right side)
    print_tree(root->R, depth + 1);
    
    // Print current node with indentation
    cout << string(depth * 2, ' ') << root->value << endl;
    
    // Print left subtree
    print_tree(root->left, depth + 1);
}

// Function to initialize a random population
vector<vector<string>> initialize_population(int pop_size) {
    vector<vector<string>> population;
    for (int i = 0; i < pop_size; ++i) {
        population.push_back(initialFP());
    }
    return population;
}
// Function to calculate fitness of an individual
long long int calculate_fitness(const vector<string>& individual, float a, float b) {
    auto [cost, penalty, area, wirelength] = get_cost(individual, a, b);
    return cost;  // You can modify this to incorporate penalty, area, etc.
}
// Function for tournament selection
vector<string> tournament_selection(const vector<vector<string>>& population, float a, float b, int tournament_size = 5) {
    int best_index = get_random_index(0,population.size());
    long long best_fitness = calculate_fitness(population[best_index], a, b);

    for (int i = 1; i < tournament_size; ++i) {
        int idx = get_random_index(0,population.size());
        long long fitness = calculate_fitness(population[idx], a, b);
        if (fitness < best_fitness) {  // Minimizing cost
            best_fitness = fitness;
            best_index = idx;
        }
    }

    return population[best_index];
}
// Function to perform crossover between two individuals
// vector<string> crossover(const vector<string>& parent1, const vector<string>& parent2) {
//     vector<string> child1,child2;

//     int 
//     // Apply crossover: swap segments of the parents
//     int crossover_point = rand() % parent1.size();
//     for (int i = crossover_point; i < parent1.size(); ++i) {
//         child[i] = parent2[i];  // Replace with part of the second parent
//     }

//     // Ensure the child remains valid (unique hardblocks and balloting property)
//     ensure_validity(child);  // You need to implement this function based on your specific rules

//     return child;
// }

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
    // OutputFile(outFile);
    
    int MT=0,uphill=0,reject=0;
    int N = k*static_cast<int>(best_pe.size());
    int count =0;
    while(true){
        MT=0;uphill=0;reject=0;
        while(true){
            auto [neighbor,M] = get_neighbor(pe);
            auto [tmp_cost,tmp_penalty,area,wirelength] = get_cost(neighbor,a,b);
            MT++;
            long int delta_cost = tmp_cost - cost;

            if(delta_cost <= 0 || random_double() < exp(-delta_cost / T)){
                if(delta_cost > 0)uphill++;
                pe = neighbor;
                cost = tmp_cost;
                // penalty = tmp_penalty;
                if(cost < best_cost){// && tmp_penalty<=1 && tmp_penalty>=-1){
                    best_pe = pe;
                    best_cost = cost;
                    // cout<<"Best update !!! M" << M+1 <<" cost="<<best_cost<<" penalty = "<<tmp_penalty<<fixed<<setprecision(2)<<" area = "<<static_cast<double>(area)/best_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/best_cost*100<<"%"<<endl;
                }
                // if(count%500==0)cout<<"M" << M+1 <<" Accept: cost = "<<tmp_cost<<fixed<<setprecision(2)<<" penalty = "<<static_cast<double>(tmp_penalty)/tmp_cost*100<<"% area = "<<static_cast<double>(area)/tmp_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/tmp_cost*100<<"%"<<endl;
            }else {
                reject++;
                // if(count%500==0)cout<<"M" << M+1 <<" Reject: cost = "<<tmp_cost<<fixed<<setprecision(2)<<" penalty = "<<static_cast<double>(tmp_penalty)/tmp_cost*100<<"% area = "<<static_cast<double>(area)/tmp_cost*100<<"% wirelength = "<<static_cast<double>(wirelength)/tmp_cost*100<<"%"<<endl;
            }
            count++;
            if(uphill > N || MT>2*N)break;
        }
        T*=cooling_rate;

        if(reject/MT > 0.95 || T < epsilon || checkTimeLimit())break;
    }

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

tuple<long long int,long long int,int,int> get_cost(vector<string>& pe,float a, float b){
    int wirelength;
    long long int penalty=0;

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
    // if(min_w > fpRegion){
    //     penalty += (min_w - fpRegion);
    //     // cout<<" w = "<<(min_w - fpRegion);
    // }
    // if(min_h > fpRegion){
    //     penalty += (min_h - fpRegion);
    //     // cout<<" h = "<<(min_h - fpRegion);
    // }

    // cout<<" area = "<<(min_w*min_h) <<" wirelength="<<(a*wirelength) <<" penalty ="<<(b*penalty)<<endl;
    long long int cost = (a*wirelength) + (b*penalty*((min_w*min_h))); //(min_w*min_h) + 
    
    return make_tuple(cost,(b*penalty*((min_w*min_h))),min_w*min_h,a*wirelength);
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
    int l = get_random_index(1,pe.size()-1) % (pe.size()-1);

    while(isOperator(pe[l])){
        l = get_random_index(1,pe.size()-1);
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
    int l = get_random_index(1,pe.size()) % pe.size();

    while(isOperand(pe[l])){
        l = get_random_index(1,pe.size()) % pe.size();
    }

    // cout<<"M2 : original = "<<pe[l];
    pe[l] = pe[l] == "H" ? "V" : "H";
    // cout<<" after = "<<pe[l]<<endl;
}
//M3
void adj_operand_operator_swap(vector<string>& pe){
    int l = get_random_index(1, pe.size()-1) % (pe.size()-1);
    // bool found = false;
    int r=l+1;
    while(true){
        //(OPERATOR(H,V) , OPERAND(hb))
        if(isOperator(pe[l])){
            r = l+1;
            if(isOperand(pe[r])){break;}
            else{
                l = get_random_index(1, pe.size()-1) % (pe.size()-1);
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
                    l = get_random_index(1,pe.size()-1) % (pe.size()-1);
                    r=l+1;
                }
            }else{
                l = get_random_index(1,pe.size()-1) % (pe.size()-1);
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
        left.coord = node.coord;
        right.coord = Coord(node.coord.x, node.coord.y + left.h);
        renew_hb_coordinates(records,node.l_from,node.l_at);//recursive calculate left and right coordinates
        renew_hb_coordinates(records,node.r_from,node.r_at);
    }else if(node.name == "V"){
        left.coord = node.coord;
        right.coord = Coord(node.coord.x + left.w, node.coord.y);
        renew_hb_coordinates(records,node.l_from,node.l_at);//recursive calculate left and right coordinates
        renew_hb_coordinates(records,node.r_from,node.r_at);
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
bool compareByArea(  pair<string, HardB> a,  pair<string, HardB> b) {
    return a.second.A() > b.second.A();
}

pair<vector<string>,long long int> initialbyHeight(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w > hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    sort(hbsVector.begin(),hbsVector.end(),compareByHeight);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;

    for(auto [name,hb] : hbsVector){
        cout<<name<<" ";
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
    cout<<endl;
    pe.emplace_back("H");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

pair<vector<string>,long long int> initialbyWidth(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w < hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    sort(hbsVector.begin(),hbsVector.end(),compareByWidth);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_height=0;

    for(auto [name,hb] : hbsVector){
        cout<<name<<" ";
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
    cout<<endl;
    pe.emplace_back("V");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

pair<vector<string>,long long int> initialbyArea1(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w > hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    sort(hbsVector.begin(),hbsVector.end(),compareByArea);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_width=0;

    for(auto [name,hb] : hbsVector){
        cout<<name<<" ";
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
    cout<<endl;
    pe.emplace_back("H");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}

pair<vector<string>,long long int> initialbyArea2(){
    for(auto& [name,hb] : hardBlocks){
        if(hb.w < hb.h)hb.r=1;
        else hb.r=0;
    }
    vector<pair<string, HardB>> hbsVector(hardBlocks.begin(), hardBlocks.end());
    sort(hbsVector.begin(),hbsVector.end(),compareByArea);

    vector<string> pe;
    bool isV=true,isH=true;
    int current_height=0;

    for(auto [name,hb] : hbsVector){
        cout<<name<<" ";
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
    cout<<endl;
    pe.emplace_back("V");
    auto [cost,penalty,area,wirelength] = get_cost(pe,1,10);
    return make_pair(pe,cost);
}


vector<string> initialFP(){
    // return initialbyWidth().first;
    auto [pe1, cost1] = initialbyHeight();
    auto [pe2, cost2] = initialbyWidth();
    auto [pe3, cost3] = initialbyArea1();
    auto [pe4, cost4] = initialbyArea2();

    vector<pair<vector<string>, long long int>> plans = {
        {pe1, cost1},
        {pe2, cost2},
        {pe3, cost3},
        {pe4, cost4}
    };

    auto best_plan = plans[0];

    for (const auto& plan : plans) {
        if (plan.second < best_plan.second) {
            best_plan = plan;
        }
    }

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
            A = width*height;
            total_A += A;
            hardBlocks[name] = HardB(name,Coord(0,0),width,height);
        } else if (key == "NumPads") {
            iss >> numPads;
        } else if (key == "Pad") {
            string name;
            int x, y;
            iss >> name >> x >> y;
            pads[name] = Pad(name,Coord(x,y));
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
            }
            nets[name] = net;
        }
    }

    fpRegion = sqrt(total_A * (1+dRatio));

    fin.close();
}