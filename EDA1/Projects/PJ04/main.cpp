#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <random>
#include <queue>

using namespace std;

struct EdgeCapacity {
    int m1 = 0, m2 = 0, m3 = 0;
};

struct GCell {
    int x, y;
    EdgeCapacity left, right, top, bottom; // supply
    EdgeCapacity L, R, T, B; // demand
};

class Point {
public:
    int x;
    int y;

    Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}

    bool operator<(const Point& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

    void print(){cout<<"("<< x << ", " << y <<")"<<endl;}
};

class Net{
public:
    /* data */
    int number; // Net number
    char mark; // char on map
    vector<pair<int, int>> GC; // Which GC does this net pass through
    vector<Point> Pins; // Convert GC to points on new coordinate
    vector<Point> RoutingPath; // routing path (store as point)
    vector<vector<int>> Result; // map: 0->nothing;1->pins;2->steiner points (?)
    bool complete = false;

    Net(string name);
    ~Net();
    void AssignPoint();
    void Routing();
};

vector<GCell> gcells;
vector<Net*> nets;
int grid_cols = 0, grid_rows = 0;

const int SIZE = 41;
const int X = 8; // 的倍數為切割線
const int Y = 10; // 的倍數為 ''
vector<vector<char>> MAP(SIZE, vector<char>(SIZE, '.'));
vector<vector<bool>> IsUsed(SIZE, vector<bool>(SIZE, 0));


void parseInput(const string& filename) {
    ifstream fin(filename);
    string line;
    enum State { HEADER, GCELL, NETS } state = HEADER;

    while (getline(fin, line)) {
        if (line.empty() || line[0] == '#') continue;

        istringstream iss(line);

        if (line.find("Gcell_grid") != string::npos) {
            iss >> ws >> line >> grid_cols >> grid_rows;
        } else if (line.find("Gcell") == 0) {
            state = GCELL;
        } else if (line.find("end_of_Gcell") != string::npos) {
            state = HEADER;
        } else if (line.find("nets") == 0) {
            state = NETS;
        } else if (line.find("end_of_nets") != string::npos) {
            break;
        } else if (state == GCELL) {
            GCell cell;
            string token;
            iss >> token >> cell.x >> cell.y;
            // cout<<"line"<<line<<endl;
            // cout<<token<<" "<< cell.x <<" "<< cell.y<<endl;
            while (iss >> token) {
                if (token == "L") {
                    string m1tag, m1val, m3tag, m3val;
                    iss >> m1tag >> m1val >> m3tag >> m3val;
                    cell.left.m1 = stoi(m1val); cell.left.m3 = stoi(m3val);
                } else if (token == "R") {
                    string m1tag, m1val, m3tag, m3val;
                    iss >> m1tag >> m1val >> m3tag >> m3val;
                    cell.right.m1 = stoi(m1val); cell.right.m3 = stoi(m3val);
                } else if (token == "B") {
                    string m2tag, m2val;
                    iss >> m2tag >> m2val;
                    cell.bottom.m2 = stoi(m2val);
                } else if (token == "T") {
                    string m2tag, m2val;
                    iss >> m2tag >> m2val;
                    // cout <<"top:"<< m2tag <<" "<< m2val<<endl;
                    cell.top.m2 = stoi(m2val);
                }
            }

            gcells.push_back(cell);
        } else if (state == NETS) {
            // Net net;
            // iss >> net.name;
            string name; iss >> name;
            Net* net = new Net(name);
            string coord;
            while (iss >> coord) {
                if (coord.front() == '(') {
                    coord.erase(remove(coord.begin(), coord.end(), '('), coord.end());
                    // coord.erase(remove(coord.begin(), coord.end(), ')'), coord.end());
                    int x = stoi(coord);
                    iss >> coord;
                    coord.erase(remove(coord.begin(), coord.end(), ')'), coord.end());
                    int y = stoi(coord);
                    net->GC.push_back({x, y});
                    //net.pins.push_back({x, y});
                }
            }
            nets.push_back(net);
        }
    }

    fin.close();
}

void print_informations(){

    // global informations
    cout << "Grid size: " << grid_cols << " x " << grid_rows << endl;
    cout << "Total Gcells: " << gcells.size() << endl;
    cout << "Total Nets: " << nets.size() << endl;

    // output one GCell -SUPPLY
    cout << "supply" <<endl;
    if (!gcells.empty()) {
        const GCell& g = gcells[0];
        cout << "GCell(" << g.x << "," << g.y << ") L(M1=" << g.left.m1 << ",M3=" << g.left.m3 << ") ";
        cout << "R(M1=" << g.right.m1 << ",M3=" << g.right.m3 << ") ";
        cout << "T(M2=" << g.top.m2 << ") B(M2=" << g.bottom.m2 << ")\n";
    }
    cout << "demand" <<endl;
    // output one GCell -DEMAND
    if (!gcells.empty()) {
        const GCell& g = gcells[0];
        cout << "GCell(" << g.x << "," << g.y << ") L(M1=" << g.L.m1 << ",M3=" << g.L.m3 << ") ";
        cout << "R(M1=" << g.R.m1 << ",M3=" << g.R.m3 << ") ";
        cout << "T(M2=" << g.T.m2 << ") B(M2=" << g.B.m2 << ")\n";
    }

    // output all GCell
    for (auto g:gcells) {
        //const GCell& g = gcells[0];
        cout << "supply" <<endl;
        cout << "GCell(" << g.x << "," << g.y << ") L(M1=" << g.left.m1 << ",M3=" << g.left.m3 << ") ";
        cout << "R(M1=" << g.right.m1 << ",M3=" << g.right.m3 << ") ";
        cout << "T(M2=" << g.top.m2 << ") B(M2=" << g.bottom.m2 << ")\n";
        cout << "demand" <<endl;
        cout << "GCell(" << g.x << "," << g.y << ") L(M1=" << g.L.m1 << ",M3=" << g.L.m3 << ") ";
        cout << "R(M1=" << g.R.m1 << ",M3=" << g.R.m3 << ") ";
        cout << "T(M2=" << g.T.m2 << ") B(M2=" << g.B.m2 << ")\n";
    }

    // output one net
    if (!nets.empty()) {
        const Net* n = nets[0];
        cout << "Net " << n->number << " has pins: ";
        for (auto p : n->GC)
            cout << "[" << p.first << "," << p.second << "] ";
        cout << endl;
    }
        // output all net
    for (auto n:nets) {
        cout << "Net " << n->number << " has pins: ";
        for (auto p : n->GC)
            cout << "[" << p.first << "," << p.second << "] ";
        cout << endl;
    }

}

pair<int, int> PtoGC(Point p){
    return make_pair(p.x/X, p.y/Y);
}

Point GCtoP(pair<int, int> gc) {
    const int x_start = gc.first * X + 1;
    const int x_end = gc.first * X + (X-1);
    const int y_start = gc.second * Y + 1;
    const int y_end = gc.second * Y + (Y-1);
    
    // 先檢查範圍內是否還有未使用的座標
    bool all_used = true;
    for (int x = x_start; x <= x_end; ++x) {
        for (int y = y_start; y <= y_end; ++y) {
            if (!IsUsed[x][y]) {
                all_used = false;
                break;
            }
        }
        if (!all_used) break;
    }
    
    if (all_used) {
        throw runtime_error("No available points in this grid cell");
        // return Point{-1, -1};
    }
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist_x(x_start, x_end);
    uniform_int_distribution<> dist_y(y_start, y_end);
    
    // 最多嘗試一定次數，避免無限循環
    const int max_attempts = 100;
    int attempts = 0;
    
    while (attempts < max_attempts) {
        int x = dist_x(gen);
        int y = dist_y(gen);
        
        if (!IsUsed[x][y]) {
            IsUsed[x][y] = true; // 標記為已使用
            return Point{x, y};
        }
        ++attempts;
    }
    throw runtime_error("Failed to find available point after multiple attempts");
}

void initializeMAP() {
    MAP.assign(SIZE, vector<char>(SIZE, '.'));
    IsUsed.assign(SIZE, vector<bool>(SIZE, false));
    for(int i=0; i<SIZE; i++) {
        for(int j = 0; j < SIZE ; j++) {
            if(i%X==0 && j%Y==0) {
                MAP[i][j] = 'X';
                IsUsed[i][j] = 1;
            }
            else if(i%X==0) {
                MAP[i][j] = '-';
                if((i != 0) && (i != (SIZE-1)) && (j != 0) && (j != (SIZE-1))){
                    if (j%2==0) MAP[i][j] = '#';
                    else IsUsed[i][j] = 1;
                }
            }
            else if(j%Y==0) {
                MAP[i][j] = '|';
                if((i != 0) && (i != (SIZE-1)) && (j != 0) && (j != (SIZE-1))){
                    if (i%2==0) MAP[i][j] = '#';
                    else IsUsed[i][j] = 1;
                }
            }
        }
    }
}

bool IsValid(Point a, vector<vector<bool>> T){
    if (a.x >= SIZE || a.x < 0) return false;
    if (a.y >= SIZE || a.y < 0) return false;
    if (T[a.x][a.y]) return true;
    if (IsUsed[a.x][a.y]) return false;
    return true;
}

void PrintMAP(){
    // char map
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            cout<<MAP[i][j] << " ";
        }
        cout << endl;
    }
    // obstacle map
    for (size_t i = 0; i < SIZE; i++){
        for (size_t j = 0; j < SIZE; j++){
            cout<<IsUsed[i][j] << " ";
        }
        cout << endl;
    }
}

void CountCongestion(Point p){
    if(p.x % X == 0){
        // Find which GC's boundary
        int gcx = p.x/X;
        int gcy = p.y/Y;
        // update demand
        // (gcx-1, gcy)的R
        int num = (4*(gcx-1))+gcy;
        if(gcells[num].R.m3 < gcells[num].right.m3) gcells[num].R.m3+=1;
        else if (gcells[num].R.m1 < gcells[num].right.m1) gcells[num].R.m1 += 1;
        else cout <<"overflow1"<<endl;
        
        // (gcx, gcy)的L
        num = (4*(gcx))+gcy;
        if(gcells[num].L.m3 < gcells[num].left.m3) gcells[num].L.m3+=1;
        else if (gcells[num].L.m1 < gcells[num].left.m1) gcells[num].L.m1 += 1;
        else cout <<"overflow2"<<endl;
    }
    if(p.y % Y == 0){
        // Find which GC's boundary
        int gcx = p.x/X;
        int gcy = p.y/Y;
        // update demand
        // (gcx, gcy-1)的T
        int num = (4*(gcx))+gcy-1;

        if(gcells[num].T.m2 < gcells[num].top.m2) {
            // cout<<"yes"<<endl;
            // cout<<gcx<<" "<<gcy<<" "<<num<<endl;
            // cout <<p.x<<" "<<p.y<<": "<< gcells[num].T.m2 <<" "<< gcells[num].top.m2 <<endl;
            gcells[num].T.m2+=1;
        }
        else {
            cout<<"no"<<endl;
            cout<<gcx<<" "<<gcy<<" "<<num << " " << gcells.size() <<endl;
            cout <<p.x<<" "<<p.y<<": "<< gcells[num].T.m2 <<" "<< gcells[num].top.m2 <<endl;
            cout <<"overflow3"<<endl;
        }
        // (gcx, gcy)的B
        num = (4*(gcx))+gcy;
        if(gcells[num].B.m2 < gcells[num].bottom.m2) gcells[num].B.m2+=1;
        else {
            cout << gcells[num].B.m2 <<" "<< gcells[num].bottom.m2 <<endl;
            cout <<"overflow4"<<endl;
        }

    }
}

vector<Point> bfs(const Point s, const Point e, vector<Point> target){

    vector<vector<bool>> T(SIZE, vector<bool>(SIZE, 0));
    T[e.x][e.y] = true;
    for(auto t:target){
        T[t.x][t.y] = true;
    }

    queue<Point> q;
    q.push(s);
    
    vector<Point> path;
    map<Point, Point> parent;
    parent[s] = {-1, -1};
    while(!q.empty()){
        Point current = q.front();
        q.pop();
        if(T[current.x][current.y]){
            // return path
            Point tmp = current;
            while (tmp != Point{-1, -1}){
                path.push_back(tmp);
                tmp = parent[tmp];
                if(tmp.x % X == 0) IsUsed[tmp.x][tmp.y] = true;
                if(tmp.y % Y == 0) IsUsed[tmp.x][tmp.y] = true;
                CountCongestion(tmp);
            }
            // reverse(path.begin(), path.end());
            return path;
        }
        vector<Point> direction = {{0,1}, {0,-1}, {1,0}, {-1,0}};
        if(current.x%X == 0){
            direction = {{1,0}, {-1,0}}; // M1 M3
        }
        if(current.y%Y == 0){
            direction = {{0,1}, {0,-1}}; // M2
        }
        for(auto d: direction){
            Point tmp = {current.x + d.x, current.y + d.y};
            if((IsValid(tmp, T) && parent.find(tmp) == parent.end())){
                q.push(tmp);
                parent[tmp] = current;
            }
        }
    }
    // cout << "No path valid!" << endl;
    return path;
}

void saveToFile(const std::string& filename) {
    ofstream outFile(filename);

    // routing result
    for(auto n:nets){
        outFile << "N" << n->number <<endl;
        outFile << "Pins" << endl;
        for(auto p:n->Pins){
            outFile << p.x << " " << p.y << endl;
        }
        outFile << "Routing Path" << endl;
        for(auto p:n->RoutingPath){
            outFile << p.x << " " << p.y << endl;
        }
    }
    
    outFile.close();
}

void ExportCongestionData(const std::vector<GCell>& gcells, const std::string& filename) {
    std::ofstream outfile(filename);
    
    if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    
    // 寫入標題行
    outfile << "x,y,left_m1,left_m2,left_m3,right_m1,right_m2,right_m3,"
            << "top_m1,top_m2,top_m3,bottom_m1,bottom_m2,bottom_m3,"
            << "L_m1,L_m2,L_m3,R_m1,R_m2,R_m3,T_m1,T_m2,T_m3,B_m1,B_m2,B_m3\n";
    
    // 寫入每個 GCell 的數據
    for (const auto& gcell : gcells) {
        outfile << gcell.x << "," << gcell.y << ","
                << gcell.left.m1 << "," << gcell.left.m2 << "," << gcell.left.m3 << ","
                << gcell.right.m1 << "," << gcell.right.m2 << "," << gcell.right.m3 << ","
                << gcell.top.m1 << "," << gcell.top.m2 << "," << gcell.top.m3 << ","
                << gcell.bottom.m1 << "," << gcell.bottom.m2 << "," << gcell.bottom.m3 << ","
                << gcell.L.m1 << "," << gcell.L.m2 << "," << gcell.L.m3 << ","
                << gcell.R.m1 << "," << gcell.R.m2 << "," << gcell.R.m3 << ","
                << gcell.T.m1 << "," << gcell.T.m2 << "," << gcell.T.m3 << ","
                << gcell.B.m1 << "," << gcell.B.m2 << "," << gcell.B.m3 << "\n";
    }
    
    outfile.close();
    // std::cout << "Congestion data exported to: " << filename << std::endl;
}


int main() {
    // srand(1) ;
    const int iterations = 50000;
    std::random_device rd;
    std::default_random_engine rng(rd());

    for(int i = 0; i<iterations; i++){
        cout<<"iteration:"<<i+1<<"-";
        // initialize
        for (auto ptr : nets) {
            delete ptr;
        }
        nets.clear();
        gcells.clear();
        initializeMAP();
        bool succeed = true;

        parseInput("input.txt");
        // print_informations();
        // return 0;
        for (auto &n:nets){
            n->AssignPoint();
        }

        // 隨機打亂 nets 的順序
        shuffle(nets.begin(), nets.end(), rng);
        // nets[0]->Routing();
    
        for(auto n:nets){
            n->Routing();
            if(!n->complete) {succeed = false; break;}
        }
        if(succeed){
            cout<<"succeed!!!!"<<endl;
            saveToFile("result_succeed.txt");
            ExportCongestionData(gcells, "congestion_data_succeed.csv");
            break;
        }
        else {cout<<"Failed!!!!"<<endl;}
    }

    return 0;
}

Net::Net(string name){
    stringstream ss(name);
    char prefix;
    ss >> prefix >> number;

    static const char lookup[] = {
        '1','2','3','4','5','6','7','8','9',
        'A','B','C','D','E','F','G'
    };

    mark = lookup[number-1];
}

Net::~Net(){

}

void Net::AssignPoint(){
    // 由左往右
    // sort(GC.begin(), GC.end());
    // 由下往上繞
    sort(GC.begin(), GC.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        if (a.second != b.second)
            return a.second < b.second;  // 先比 second
        return a.first < b.first;        // 再比 first
    });
    for(auto gc:GC){
        Point tmp = GCtoP(gc);
        Pins.push_back(tmp);
        MAP[tmp.x][tmp.y] = mark;
    }
}

void Net::Routing(){
    // cout << "Routing N" << number << "..." << endl;
    bool c = true;
    for(size_t i = 1; i<(Pins.size()); i++){
        vector<Point> path;
        if(i==1){
            vector<Point> tmp; tmp.push_back(Pins[0]);
            path = bfs(Pins[i], Pins[0], tmp);
            RoutingPath.insert(RoutingPath.end(), path.begin(), path.end());
        }
        else{
            path = bfs(Pins[i], Pins[i-1], RoutingPath);
            RoutingPath.insert(RoutingPath.end(), path.begin(), path.end());
        }
        if(path.empty()){
            c = false;
            break;
        }
    }
    if(c) complete = true;
    // cout<<endl;
}