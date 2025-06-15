#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <map>

using namespace std;

// void tsp(const vector<vector<int>>& cost);
const int INF = 1e9;
int n;
vector<vector<int>> memo;
vector<vector<int>> parent2;

// Edges dict
unordered_map<string, pair<int,pair<string,string>>> Edges_dict;
// node:{neighbour, weight} 
map<int,vector<pair<int,int>>> Nodes;
// All nodes
//set<string> nodes;
/*void addEdge(unordered_map<string, pair<int,pair<string,string>>> edges, const string edge, int weight, const string v1, const string v2){
	edges[edge] = {weight,{v1,v2}};	
}*/
void print_cities(vector<vector<int>>& cities){
    for(int i=0;i<cities.size();++i){
        for(int j=0;j<cities[i].size();++j){
            cout<<cities[i][j]<<" ";
        }
        cout<<endl;
    }
}

int tsp_2(int mask, int now_city,const vector<vector<int>>& cost){
    // If all cities visited, return to origin city;
    if (mask == (1<<n)- 1) return cost[now_city][0];
    // If the route to city is calculated, then return the cost
    if (memo[mask][now_city] != -1) return memo[mask][now_city];

    int minCost = INF;
    int bestNext = -1;

    for(int i=0;i<n;i++){
        if( !(mask & (1<<i)) ){
            int newMask = mask | (1<<i);
            int newCost = cost[now_city][i] + tsp_2(newMask,i,cost);

            if(newCost < minCost){
                minCost = newCost;
                bestNext = i;
            }
        }
    }

    parent2[mask][now_city] = bestNext;
    return memo[mask][now_city] = minCost;

}

void printPath() {
    int mask = 1, i = 0; // 從城市 0 開始
    // int cost = 0;

    cout << "Best path: "<<  <<" -> ";
    // cout<< memo[mask][i] <<endl;
    while (true) {
        int next = parent2[mask][i];
        if (next == -1) break;
        // cost += cities[i][next];
        cout << next << " -> ";
        
        mask |= (1 << next);
        //cout << "Cost = "<<memo[mask][i]<<endl; //bitset<15>(mask)<<endl;
        i = next;
    }
    cout << "0" << endl; // 回到起點
    
}

void tsp(const vector<vector<int>>& cost){

    // dp[mask][i] 表示從起點 (假設為城市 0) 出發，
    // 訪問了 mask 中所有城市且最後到達城市 i 的最小花費
    int N = 1 << n;
    cout<< N <<endl;
    vector<vector<int>> dp(N, vector<int>(n, INF));
    vector<vector<int>> parent(N, vector<int>(n, -1)); // 記錄轉移路徑


    // 初始狀態：只訪問起點，花費為 0
    dp[1][0] = 0;

    // 遍歷所有狀態
    for (int mask = 1; mask < N; mask++) {
        for (int i = 0; i < n; i++) {
            // 如果城市 i 未在 mask 中則跳過
            if (!(mask & (1 << i))) continue;
            // 如果當前狀態不可達則跳過
            if (dp[mask][i] == INF) continue;
            // 嘗試擴展到還未訪問的城市 j
            for (int j = 0; j < n; j++) {
                if (mask & (1 << j)) continue; // j 已訪問，跳過
                int nextMask = mask | (1 << j);
                int newCost = dp[mask][i] + cost[i][j];

                if (newCost < dp[nextMask][j]){
                    dp[nextMask][j] = newCost;
                    parent[nextMask][j] = i; // 記錄最佳前驅城市
                }

                //dp[nextMask][j] = min(dp[nextMask][j], dp[mask][i] + cost[i][j]);
            }
        }
    }

    // 完成所有城市的訪問後，再回到起點 0
    // int answer = INF;
    // for (int i = 0; i < n; i++) {
    //     answer = min(answer, dp[N - 1][i] + cost[i][0]);
    // }
    
    // 找到最短回到城市 0 的路徑
    int lastCity = -1, minCost = INF;
    for (int i = 0; i < n; i++) {
        int totalCost = dp[N - 1][i] + cost[i][0];
        if (totalCost < minCost) {
            minCost = totalCost;
            lastCity = i;
        }
    }

    if (minCost >= INF) {
        cout << "No Hamilton path" << endl;
        // return 0;
    } else {
        cout << "Best cost : " << minCost << endl;
        // return 0;
    }
    // 回溯路徑
    vector<int> path;
    int mask = N - 1;
    while (lastCity != -1) {
        path.push_back(lastCity);
        int prevCity = parent[mask][lastCity];
        mask ^= (1 << lastCity); // 移除 lastCity
        lastCity = prevCity;
    }
    reverse(path.begin(), path.end()); // 反轉路徑

    cout << "Best path : ";
    for (int city : path) {
        cout << city << " -> ";
    }
    cout << "0" << endl; // 回到起點
    // return 0;
}

int main(int argc, char* argv[]) {
   
	if(argc < 3){
cerr<<"Method: "<< argv[0] << " intput_file output_file"<<endl;
	return 1;
}
	ifstream infile(argv[1]);
	if (!infile.is_open()){
		cerr << "Unable to open input file: "<<argv[1]<<endl;
	return 1;
	}
	
	string line;
	while(getline(infile,line)){
		istringstream iss(line);
		string edge;
		int weight;
		string v1,v2;

		if(!(iss>>edge>>weight>>v1>>v2)){
			cerr<<"Error format: "<<line<<endl;
			continue;		
        }
        Edges_dict[edge] = {weight,{v1,v2}};	
        int n1 = stoi(v1.substr(1)) -1;
        int n2 = stoi(v2.substr(1)) -1;
        Nodes[n1].push_back({n2,weight});
        Nodes[n2].push_back({n1,weight});

//		addEdge(Edges_dict,edge,weight,v1,v2);
}	
	infile.close();
	
    vector<vector<int>> cities(Nodes.size(),vector<int>(Nodes.size(),INF));

    for(const auto& node : Nodes){
        // cout<< node.first<<" : ";
        for (const auto& neighbor : node.second){
            // cout<<"("<< neighbor.first <<":"<<neighbor.second<<")";
            cities[node.first][neighbor.first] =  neighbor.second;
            cities[neighbor.first][node.first] =  neighbor.second;
        } 
        // cout<<endl;
    }

    cout<<cities.size()<<endl;
    // print_cities(cities);
    n = cities.size();
    memo.assign(1 << n, vector<int>(n, -1));
    parent2.assign(1 << n, vector<int>(n, -1));
    tsp(cities);
    cout<<tsp_2(1,0,cities)<<endl;
    printPath();

    // for (const auto &kv : Nodes) {
    //     cout << "Node " << kv.first << " has neighbors: ";
    //     for (const auto &neighbor : kv.second) {
    //         cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
    //     }
    //     cout << endl;
    // }
	/*for (const auto& entry : Edges_dict){
		cout<<entry.first<<" "<<entry.second.first<<" "<<entry.second.second.first<<" "<<entry.second.second.second<<endl;
    }*/

}