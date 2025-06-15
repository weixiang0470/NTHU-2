#include <iostream>
#include <vector>
#include <thread>
#include <cmath> 
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

int N,cnt=0;
bool isRestart=false;
atomic<bool> keep_running(true);
mt19937 g(88);
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();

bool checkT(){
    auto currentTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::minutes>(currentTime - startTime).count();
    if(usedTime > 600)return true;
    else return false;
}

void alive_msg(){
    while(keep_running){
        this_thread::sleep_for(chrono::seconds(30));
        cout<<"cnt="<<cnt<<"|"<<endl;
        cnt++;
    }
}

int count_attacks(vector<int> result){
    int attacks=0;
    int n = result.size();
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if(result[i]==result[j] || abs(i-j)==abs(result[i]-result[j]))attacks++;
        }
    }
    return attacks;
}

vector<int> hc(int max_steps,uniform_int_distribution<>& dis){
    vector<int> board(N);
    vector<int> tmp_board(N);
    vector<int> InitVec;
    for(int i=0;i<N;i++)InitVec.emplace_back(i);
    // for(int i=0;i<N;i++)tmp_board[i] = dis(g)%N;
    tmp_board = InitVec;
    shuffle(tmp_board.begin(),tmp_board.end(),g);
    int current_attacks = count_attacks(tmp_board);

    for(int step=0;step<max_steps;step++){
        if(current_attacks==0)return board;

        int best_attacks = current_attacks;
        pair<int,int> best_move={-1,-1};

        for(int row=0;row<N;row++){
            // int origin_col = tmp_board[row];
            for(int col=row+1;col<N;col++){
                // if(col == origin_col)continue;
                swap(tmp_board[row],tmp_board[col]);
                // tmp_board[row]=col;
                int new_attacks = count_attacks(tmp_board);
                if(new_attacks < best_attacks){
                    best_attacks = new_attacks;
                    best_move={row,col};
                }
                swap(tmp_board[row],tmp_board[col]);
            }
            // tmp_board[row]=origin_col;
        }
        if(best_move.first != -1){
            swap(tmp_board[best_move.first],tmp_board[best_move.second]);
            // tmp_board[best_move.first]=best_move.second;
            current_attacks = best_attacks;
            board = tmp_board;
        }
        else {
            // if(isRestart)for(int i=0;i<N;i++)tmp_board[i] = dis(g)%N;
            if(isRestart)shuffle(tmp_board.begin(),tmp_board.end(),g);
            else break;
        }
    }
    return board;
}

int main(int argc, char* argv[]){
    // srand(101);
    N = (argc>=2)? stoi(argv[1]) : 8;
    int max_depth = (argc>=3)? stoi(argv[2]) : 9999;
    isRestart = (argc >=4 && strcmp(argv[3], "yes") == 0);
    uniform_int_distribution<> dis(0, N*2);

    // thread alive_thread(alive_msg);
    int success=0,total_attack=0;
    for(int i=0;i<30;i++){
        vector<int> result = hc(max_depth,dis);
        int attacks = count_attacks(result);
        if(attacks == 0)success++;
        total_attack += attacks;
        cout<<"Trails "<<i+1<<" attack="<<attacks<<endl;
        // cout<<"Trial "<<i+1<<endl;
        // cout<<"Attack = "<< count_attacks(result)<<endl;
    }

    auto endTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout<<"Average time used = "<< usedTime / 30.0 <<"minutes"<<endl;
    cout<<"Average attack = "<< total_attack / 30.0 <<endl;
    cout<<"Success rate = "<< success * 100.0 / 30.0 <<"%"<<endl;
    // keep_running = false;
    // alive_thread.join();
    return 0;

}