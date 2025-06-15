#include "parser.hpp"

using namespace std;

vector<cell*> cells;
vector<row*> rows;
vector<blockage> blockages;
vector<cell*> over_max_cells;
int max_dis=0;
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
int timeLimit = 60;
double alpha=0.0;

void assign_block_in_row();
void abacus();
void OutputFile(const string &Outfile);
void cells_alignment();

int main(int argc, char* argv[]){
    if(argc < 3){
        cerr<<"Method: "<< argv[0] << " intput_file output_file "<<endl;
            return 1;
    }

    try {
        InputParser parser;
        parser.parseInput(argv[1],cells,blockages,rows);
        max_dis = parser.MaxDisC;
        alpha = max_dis / 5.0;
        assign_block_in_row();
        // for(cell* c : cells){
        //     c->print_cell();
        // }
        for(row* r:rows){
            // r->print_sites();
            // cout<<"Row : "<< r->name<<endl;
            r->construct_subrows();
            r->print_subrows();
        }
        abacus();
        cout<<"finish abacus"<<endl;
        cells_alignment();
        for(cell* c : cells){
            if(c->best_cost > max_dis){
                over_max_cells.emplace_back(c);
                // c->print_cell();
            }
        }
        // int n_cells = 0,n_cells2=0;
        for(row* r:rows){
            // for(auto sr : r->subrows){
            //     for(auto cls : sr->clusters){
            //         n_cells += cls->cells.size();
            //     }
            //     n_cells2 += sr->cells.size();
            // }
            // cout<<"Row : "<< r->name<<endl;
            r->print_subrows();
        }
        // cout<<"n_cell = "<<n_cells<<", n_cells2 = "<<n_cells2<<endl;
        cout<<"Have "<<over_max_cells.size()<<" cells over maximum constraints!!!"<<endl;
        for(size_t i=0;i<over_max_cells.size();i++){
            over_max_cells[i]->print_cell();
            if(i>10)break;
        }
        
        OutputFile(argv[2]);
        for (auto ptr : cells)delete ptr; 
        for (auto ptr : rows)delete ptr;
        return 0;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

// if infront of cell still have space and cell not access max constraint then move forward
void cells_alignment() {
    for(auto r : rows) {
        int site_width = r->s_w;
        for(auto sr : r->subrows) {
            Cluster* last_cluster = sr->last_cls;
            while(last_cluster != nullptr) {
                double x_offset = last_cluster->x - sr->min_x;
                if(x_offset - std::floor(x_offset / site_width) * site_width <= site_width / 2.0) {
                    // Align the cluster to the left
                    last_cluster->x = sr->min_x + std::floor(x_offset / site_width) * site_width;
                } else {
                    // Align the cluster to the right
                    last_cluster->x = sr->min_x + std::ceil(x_offset / site_width) * site_width;
                }

                int opt_x = last_cluster->x;
                for(auto c : last_cluster->cells) {
                    c->x = opt_x;
                    // c->y = r->y;
                    opt_x += c->w;//ceil(c->w / site_width) * site_width;
                }

                last_cluster = last_cluster->pre;
            }
        }
    }
}

void delete_subrow(subrow* subr) {
    for (Cluster* cls : subr->clusters) {
        delete cls;
    }
    delete subr;
}

void abacus(){
    sort(cells.begin(), cells.end(), [](cell* a, cell* b) {
        return a->g_x < b->g_x;
    });
    cout<<"Sorting done, maximum distance ="<<max_dis<<endl;
    cout<<"Have "<<rows.size()<<" rows, "<< cells.size()<<" cells, "<<blockages.size()<<" blockages"<<endl;
    int count =0;
    for(auto c : cells){
        count++;
        // cout<<"cell "<<c->name<<" row = "<<rows.size()<<endl;
        double best_cost = numeric_limits<double>::max();
        // if(count < static_cast<int>(cells.size())/2)best_cost = 0.0;
        subrow* best_subrow = nullptr;
        for(auto r : rows){
            // if(abs(c->g_y - r->y) >= c->best_cost) continue;
            // cout<<"row = "<<r->name<<endl;
            for(auto subr : r->subrows){
                if(c->g_x - max_dis > subr->max_x || c->g_x + max_dis < subr->min_x)continue;
                // if(c->g_x - c->best_cost > subr->max_x || c->g_x + c->best_cost < subr->min_x)continue;
                try{
                    if(subr->free_sites >= ceil(c->w / subr->site_width)){
                        // cout<<"trying subrow "<<endl;
                        subrow* trial_subrow = subr->clone();
                        // cout<< trial_subrow <<" vs "<<subr<<endl;
                        
                        trial_subrow->insert_cell_in_subrow(c);
                        trial_subrow->Place_subrow(c);
                        delete trial_subrow;
                        
                        double d = c->get_dis();
                        // if(d > max_dis) continue;

                        double space_ratio = (double)subr->free_sites/subr->total_sites;
                        double penalty = (1.0 - space_ratio)*alpha; // alpha
                        if(d > max_dis) penalty += max_dis*10;
                        double cost = d + penalty;

                        // cout<<"cost = "<<cost<<endl;
                        
                        if(cost < best_cost){
                            best_cost = cost;
                            best_subrow = subr;
                            c->best_cost = d;
                            // cout<<"cell="<<c->name<<" cost = "<<std::setprecision(5)<<best_cost<<" subrow:"<<best_subrow->name<<endl;
                        }
                        
                        // subr->remove_cell_from_subrow(c);
                    }
                }
                catch(const exception& e){
                    cerr<<"Error: "<<e.what()<<endl;
                    // return ;
                }
                
            }
        }
        // cout<<"2 best sub row = "<<best_subrow->name<<" cost="<<best_cost<<endl;

        // for(cell* c : best_subrow->cells){
        //     cout<<"testing1"<<endl;
        //     c->print_cell();
        // }
        // for(auto& row:rows){
        //     // row.print_sites();
        //     cout<<"Row : "<< row.name<<endl;
        //     row.print_subrows();
        // }
        if(best_subrow == nullptr){
            // cout<<"cell "<< c->name<<" cannot be place!!!"<<endl;
            continue;
        }
        // int tmp = 0;
        // for(auto cls : best_subrow->clusters){
        //     tmp += cls->cells.size();
        // }
        best_subrow->insert_cell_in_subrow(c);
        // cout<<"before, clusters : "<<best_subrow->clusters.size()<<endl;
        best_subrow->Place_subrow(c);
        // int tmp2 = 0;
        // for(auto cls : best_subrow->clusters){
        //     tmp2 += cls->cells.size();
        // }
        // cout<<"before = "<<tmp<<", after = "<<tmp2<<endl;
        // cout<<"after, clusters : "<<best_subrow->clusters.size()<<endl;
        best_subrow->free_sites -= ceil(c->w/best_subrow->site_width);
        // double cost = c->get_dis();
        // cout<<"cell "<<c->name<<", cost="<<cost<<" count="<<count<<endl;

        // for(row* r:rows){
        //     // row.print_sites();
        //     cout<<"Row : "<< r->name<<endl;
        //     r->print_subrows();
        // }

    }
}

void OutputFile(const string &Outfile){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }
    double Total_dis = 0;
    double max_dis = 0;
    for(cell* c:cells){
        double cost = c->get_dis();
        Total_dis += cost;
        max_dis = max(max_dis,cost);
    }
    fo << "TotalDisplacement "<<static_cast<int>(ceil(Total_dis))<<endl;
    fo<< "MaxDisplacement "<<static_cast<int>(ceil(max_dis))<<endl;
    cout<< "MaxDisplacement "<<static_cast<int>(ceil(max_dis))<<endl;
    fo<< "NumCells "<<static_cast<int>(cells.size())<<endl;
    for(cell* c:cells){
        fo << c->name << " "<< c->x <<" "<< c->y << endl;
    }

    fo.close();

    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    cout<<"Total time used = "<< elapsedTime <<"s"<<endl;

}

void assign_block_in_row(){
    sort(blockages.begin(), blockages.end(), [](blockage a, blockage b) {
        return a.x < b.x;
    });

    for(auto& blockage : blockages){
        for(row* r: rows){
            if(r->y >= blockage.y && r->y < blockage.y + blockage.h){
                bool start=false;
                // assign into row's sites
                for(size_t i=0;i<r->sites.size();i++){
                    if(r->x + i*r->s_w <= blockage.x && r->x + (i+1)*r->s_w > blockage.x){
                        // cout<<row.x + i*row.s_w<<endl;
                        start=true;
                        // row.sites[i]=true;
                    }
                    if(start){
                        if(r->x + i*r->s_w < blockage.x + blockage.w){
                            r->sites[i]=true;
                        }else {start=false;break;}
                    }
                }
            }
        }
    }
}