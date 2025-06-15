#include <iostream>
#include <vector>
#include <thread>
#include <cmath> 
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <unordered_set>

using namespace std;
typedef unsigned long long int ll;
struct Chromosome{
    vector<int> DNA;
    int numATK;
};

int N=8;
int P = 50;

ll population_size=1000;
vector<int> InitVec;
mt19937 g(88);
vector<Chromosome> populations;
atomic<bool> keep_running(true);
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
int cnt=0; 
ll k=100;

void alive_msg(){
    while(keep_running){
        this_thread::sleep_for(chrono::seconds(30));
        cout<<"cnt="<<cnt<<"|"<<endl;
        cnt++;
    }
}

int count_attacks(const vector<int>& result){
    int attacks=0;
    int n = result.size();
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            if(result[i]==result[j] || abs(i-j)==abs(result[i]-result[j]))attacks++;
        }
    }
    return attacks;
}

void init_ppl(){
    for(int i=0;i<population_size;i++){
        Chromosome chromosome;
        chromosome.DNA = InitVec;
        shuffle(chromosome.DNA.begin(),chromosome.DNA.end(),g);
        chromosome.numATK = count_attacks(chromosome.DNA);
        populations.emplace_back(chromosome);
    }
}

Chromosome crossover(const Chromosome& parent1,const Chromosome& parent2){
    int cut_point = rand()%(N-2) + 1; //from cut range = 2 to n-1
    Chromosome child_chromosome;
    vector<int> child(parent1.DNA.begin(),parent1.DNA.begin()+cut_point);
    for(int i=cut_point;i<N;i++)child.emplace_back(parent2.DNA[i]);
    child_chromosome.DNA = child;
    child_chromosome.numATK = count_attacks(child_chromosome.DNA);
    return child_chromosome;
}

pair<Chromosome,Chromosome> oxcrossover(const Chromosome& parent1,const Chromosome& parent2,uniform_int_distribution<>& dis){
    Chromosome child1,child2;
    child1.DNA.resize(N);
    child2.DNA.resize(N);
    int cut1 = dis(g) % N;
    int cut2 = dis(g) % (N - cut1) + cut1 ;
    if(cut2>=N)cut2=N-1;

    // vector<bool> filled1(N, false);
    unordered_set<int> in_child1;
    for(int i = cut1; i<= cut2 ; i++){
        child1.DNA[i] = parent1.DNA[i];
        in_child1.insert(parent1.DNA[i]);
        // filled1[i] = true;
    }
    // vector<bool> filled2(N, false);
    unordered_set<int> in_child2;
    for(int i = cut1; i<= cut2 ; i++){
        child2.DNA[i] = parent2.DNA[i];
        in_child2.insert(parent2.DNA[i]);
        // filled2[i] = true;
    }

    int it1 = cut2+1,it2 = cut2+1;
    int current_insert1 = cut2+1, c_insert2 = cut2+1;
    while(in_child1.size() < N-1 || in_child2.size() < N-1){
        if(it1>=N)it1=0;
        if(it2>=N)it2=0;
        if(current_insert1>=N)current_insert1=0;
        if(c_insert2>=N)c_insert2=0;
        // if (!filled1[current_insert1]) {
            if (in_child1.count(parent2.DNA[it1]) == 0) {
                child1.DNA[current_insert1] = parent2.DNA[it1];
                in_child1.insert(parent2.DNA[it1]);
                // filled1[current_insert1] = true;  
                current_insert1++;
            }
            it1++;
        // } else {
        //     current_insert1++;
        // }
        // if (!filled2[c_insert2]) {
            if (in_child2.count(parent1.DNA[it2]) == 0) {
                child2.DNA[c_insert2] = parent1.DNA[it2];
                in_child2.insert(parent1.DNA[it2]);
                // filled2[c_insert2] = true;  
                c_insert2++;
            }
            it2++;
        // } else {
        //     c_insert2++;
        // }
    }
    child1.numATK = count_attacks(child1.DNA);
    child2.numATK = count_attacks(child2.DNA);
    // cout<<"cut1 = "<<cut1<<", cut2 = "<<cut2<<endl;
    return {child1,child2};
}

void mutation(Chromosome& child, uniform_int_distribution<>& dis){
    uniform_int_distribution<> dis2(0, N-1);
    int rate = dis(g)%100;
    if(rate >= P){
        int mutate1 = dis2(g);
        int mutate2 = dis2(g);
        swap(child.DNA[mutate1],child.DNA[mutate2]);
        child.numATK = count_attacks(child.DNA);
    }
    
}

Chromosome best_DNA(vector<Chromosome>& candidates){
    Chromosome best_individual = *min_element(candidates.begin(),candidates.end(),[](const Chromosome& a, const Chromosome& b){
        return a.numATK < b.numATK;
    });
    return best_individual;
}

void Survivor_selection(vector<Chromosome>& candidates){
    vector<Chromosome> sorted_candidates = candidates;
    sort(sorted_candidates.begin(), sorted_candidates.end(), 
        [](const Chromosome& a, const Chromosome& b) {
            return a.numATK < b.numATK;
        });
    populations = vector<Chromosome>(sorted_candidates.begin(),sorted_candidates.begin()+population_size);
    // vector<Chromosome> best_1000(sorted_populations.begin(), sorted_populations.begin() + 1000);
    // return best_1000;
}

Chromosome Selection(uniform_int_distribution<>& dis,vector<Chromosome>& new_populations){
    // int best = dis(g);
    // for(int i=0;i<population_size/100;i++){
    //     int candidate = dis(g);
    //     if(populations[candidate].numATK < populations[best].numATK){
    //         best = candidate;
    //     }
    // }
    uniform_int_distribution<> dis2(0,new_populations.size()-1);
    vector<Chromosome> candidates;
    for(int i=0;i<k;i++){
        int candidate_no = dis2(g);
        candidates.emplace_back(new_populations[candidate_no]);
    }
    
    return best_DNA(candidates);
}

void next_gen(uniform_int_distribution<>& dis){
    vector<Chromosome> new_populations;// = populations;//=best_1000_DNA();
    Chromosome best_individual = best_DNA(populations);
    new_populations.emplace_back(best_individual);

    while(new_populations.size()<population_size){
        Chromosome p1 = Selection(dis,populations);
        Chromosome p2 = Selection(dis,populations);
        int cross_P = dis(g)%100;
        int Pc = dis(g) % 30 + 60;
        if(cross_P>= Pc){
        auto [child1,child2] = oxcrossover(p1,p2,dis);
        mutation(child1,dis);
        mutation(child2,dis);
        new_populations.emplace_back(child1);
        new_populations.emplace_back(child2);
        }
        else {
            new_populations.emplace_back(p1);
            new_populations.emplace_back(p2);
        }
        
    }
    // Survivor_selection(new_populations);
    populations = new_populations;
}

Chromosome GA(int generations,uniform_int_distribution<>& dis){
    populations.clear();
    init_ppl();
    Chromosome best_individual = best_DNA(populations);
    for(int i=0;i<generations;i++){
        best_individual = best_DNA(populations);
        if(best_individual.numATK == 0)return best_individual;
        next_gen(dis);
    }
    return best_individual;
}

void print_solutions(Chromosome result){
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            if(result.DNA[i]==j)cout<<"Q";
            else cout<<"o";
        }
        cout<<endl;
    }
}

int main(int argc, char* argv[]){
    
    // srand(101);
    N = (argc>=2)? stoi(argv[1]) : 8;
    population_size = (argc>=3)? stoll(argv[2]) : 100;
    ll generations = (argc>=4)? stoll(argv[3]) : 100;
    k = (argc>=5) ? stoll(argv[4]) : 100;
    if(k>=population_size)k=population_size/10;
    uniform_int_distribution<> dis(0, population_size-1);
    // for(int i =0;i<20;i++)oxcrossover(dis);
    // return 0;
    // thread alive_thread(alive_msg);

    for(int i=0;i<N;i++)InitVec.emplace_back(i);

    int success=0,total_attack=0;
    for(int i=0;i<30;i++){
        
        Chromosome best_individual = GA(generations,dis);
        if(best_individual.numATK==0){
            success++;
            // cout<<"Solution found:\nANS = ";
            // for(auto x : best_individual.DNA){
            //     cout<< x <<" ";
            // }
            // cout<<endl;
            // print_solutions(best_individual);
        }else{
            total_attack+=best_individual.numATK;
            // cout<<"No optimal solution found!\n";
            // cout<<"Attack = "<<best_individual.numATK;
        }
        cout<<"Trails "<<i+1<<" attack="<<best_individual.numATK<<endl;
    }
    auto endTime = chrono::steady_clock::now();
    auto usedTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    cout<<"Average time used = "<< usedTime / 30.0 <<" ms"<<endl;
    cout<<"Average attack = "<< total_attack / 30.0 <<endl;
    cout<<"Success rate = "<< success * 100.0 / 30.0 <<"%"<<endl;
    
    // keep_running = false;
    // alive_thread.join();

    return 0;
}