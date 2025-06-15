#include <iostream>
#include <vector>
#include <thread>
#include <cmath> 
#include <chrono>
#include <random>

using namespace std;
typedef unsigned long long int ll;

ll lowbit(ll x){return x & -x;}

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
        cout<<"used time ="<<usedTime<<" Current_depth="<<cur_trials<<endl;
        // cnt++;
    }
}

bool checkT(){
    auto currentTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::minutes>(currentTime - startTime).count();
    if(usedTime > 1000)return true;
    else return false;
}

bool dfs(ll M, ll L,ll R){

    if( M == MASK){
        return true;
    }
    ll Legal = MASK & ~(M|L|R);
    for(ll num=0; Legal; Legal ^= num){
        num = lowbit(Legal);
        ans.emplace_back(log2(num));
        if(dfs(M|num,(L|num)<<1,(R|num)>>1))return true;
        ans.pop_back();
    }
    return false;
}

// bool ids(){
//     for(ll depth = 1; depth<=N; depth++){
//         ans.clear();
//         cur_trials = depth;
//         // cout<<"Searching depth = "<<depth<<endl;
//         if(dfs(0,0,0,0,depth))return true;
//     }
//     return false;
// }
int main(int argc, char* argv[]){
    // int iteration = 30;
    N = (argc>=2)? stoll(argv[1]) : 8;
    // random_device rd;
    
    // 使用 Mersenne Twister 算法
    mt19937 gen(101);
    
    // 定义分布范围 [0, 100000]
    std::uniform_int_distribution<> dis(0, 1000000);
    
    // 生成随机数
    
    for(int i=0;i<1000000;i++){
        int random_num = dis(gen);
        if(random_num<=10)cout<<"YES"<<endl;
    }
    return 0;
    cout<<"N="<<N<<endl;
    int success=0,total_attack=0;
    thread alive_thread(alive_msg);
    MASK = (1ULL<<N)-1;
    cout<< bitset<64>(MASK).to_string().substr(64-N)<<endl;
    if(dfs(0,0,0)){
        cout<<"Solution found "<<ans.size()<<endl;
        for(auto x:ans){
            cout<<x <<" ";
        }
        cout<<endl;
        success++;
    }

    auto endTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout<<"\nAverage time used = "<< usedTime  <<"ms"<<endl;
    cout<<"Average attack = "<< total_attack <<endl;
    cout<<"Success rate = "<< success * 100.0 <<"%"<<endl;

    keep_running = false;
    alive_thread.join();

    return 0;

}