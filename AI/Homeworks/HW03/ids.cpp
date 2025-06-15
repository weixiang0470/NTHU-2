#include <iostream>
#include <vector>
#include <thread>
#include <cmath> 
#include <chrono>

using namespace std;
typedef unsigned long long int ll;

ll lowbit(ll x){return x & -x;}

ll node=0;
ll N;
ll MASK;
int cur_trials=0;
vector<int> ans;
atomic<bool> keep_running(true);
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();

int count_attacks(){
    int attacks=0;
    int n = ans.size();
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if(ans[i]==ans[j] || abs(i-j)==abs(ans[i]-ans[j]))attacks++;
        }
    }
    return attacks;
}

void alive_msg(){
    while(keep_running){
        this_thread::sleep_for(chrono::seconds(30));
        auto currentTime = chrono::steady_clock::now();
        auto usedTime = chrono::duration_cast<chrono::minutes>(currentTime - startTime).count();
        cout<<"used time ="<<usedTime<<" Current_depth="<<cur_trials<<" Expanded node="<<node<<endl;
        // cnt++;
    }
}

bool checkT(){
    auto currentTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::minutes>(currentTime - startTime).count();
    if(usedTime > 1000)return true;
    else return false;
}

bool dfs(ll M, ll L,ll R,ll current_depth, ll max_depth){
    node++;
    if(current_depth == max_depth || checkT()){
        return M == MASK;
    }
    ll Legal = MASK & ~(M|L|R);
    for(ll num=0; Legal; Legal ^= num){
        num = lowbit(Legal);
        ans.emplace_back(log2(num));
        if(dfs(M|num,(L|num)<<1,(R|num)>>1,current_depth+1,max_depth))return true;
        ans.pop_back();
    }
    return M == MASK;
}

bool ids(){
    for(ll depth = 1; depth<=N; depth++){
        ans.clear();
        node=0;
        cur_trials = depth;
        // cout<<"Searching depth = "<<depth<<endl;
        if(dfs(0,0,0,0,depth))return true;
    }
    return false;
}

int main(int argc, char* argv[]){
    // int iteration = 30;
    N = (argc>=2)? stoll(argv[1]) : 8;

    // cout<<"N="<<N<<endl;
    MASK = (1ULL<<N)-1;
    // cout<< bitset<64>(MASK).to_string().substr(64-N)<<endl;
    int success=0,total_attack=0;
    thread alive_thread(alive_msg);

    for(int i=0;i<1;i++){
        if(ids()){
            success++;
            // cout<<"Solution found:\n";
            for(auto x:ans){
                cout<< x <<" ";
            }
            // cout<<"\nChessboard:\n";
            // for(auto x:ans){
            //     cout<< bitset<64>(x).to_string().substr(64-N)<<endl;
            // }
        }
        total_attack += count_attacks();
    }
    auto endTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::minutes>(endTime - startTime).count();

    cout<<"\nAverage time used = "<< usedTime  <<"minutes"<<endl;
    cout<<"Average attack = "<< total_attack <<endl;
    cout<<"Success rate = "<< success * 100.0 <<"%"<<endl;

    keep_running = false;
    alive_thread.join();

    return 0;

}