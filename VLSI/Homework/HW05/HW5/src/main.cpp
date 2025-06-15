#include <chrono>
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
void cells_alignment3();
void legalize_cells(vector<subrow*>& illegal_subrow);
void legalize_cells_2(vector<cell*>& over_max_cells);
int get_closest_row(cell* c);

int main(int argc, char* argv[]){
    if(argc < 3){
        cerr<<"Method: "<< argv[0] << " intput_file output_file "<<endl;
            return 1;
    }

    try {
        InputParser parser;
        parser.parseInput(argv[1],cells,blockages,rows);
        max_dis = parser.MaxDisC;
        alpha = max_dis;
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
        // cells_alignment();
        cells_alignment3();
        vector<subrow*> sr_have_ill_cell;
        for(auto c : cells){
            if(c->get_dis() > max_dis){
                over_max_cells.emplace_back(c);
            }
        }
        for(auto r:rows){
            for(auto sr:r->subrows){
                for(auto c: sr->cells){
                    if(c->get_dis() > max_dis){
                        sr_have_ill_cell.emplace_back(sr);
                        break;
                    }
                }
            }
        }
        // for(auto sr: sr_have_ill_cell){
        //     cout<<"Subrow that have illegal cell is "<<sr->name<<endl;
        // }
        // int n_cells = 0,n_cells2=0;
        // for(row* r:rows){
        //     // for(auto sr : r->subrows){
        //     //     for(auto cls : sr->clusters){
        //     //         n_cells += cls->cells.size();
        //     //     }
        //     //     n_cells2 += sr->cells.size();
        //     // }
        //     // cout<<"Row : "<< r->name<<endl;
        //     // r->print_subrows();
        // }
        // cout<<"n_cell = "<<n_cells<<", n_cells2 = "<<n_cells2<<endl;
        cout<<"Have "<<over_max_cells.size()<<" cells over maximum constraints!!!"<<endl;
        for(size_t i=0;i<over_max_cells.size();i++){
            over_max_cells[i]->print_cell();
            if(i>10)break;
        }
        vector<subrow*> moveable_sr;
        for(auto r:rows){
            for(auto sr:r->subrows){
                bool start=true;
                int last_point = sr->min_x;
                for(auto c: sr->cells){
                    if(start){
                        if(c->x != sr->min_x){
                            moveable_sr.emplace_back(sr);
                            break;
                            // cout<<sr->name<<" subrow's first cell is not at left most!"<<endl;
                        }
                        start=false;
                    }
                    if(last_point!=c->x){
                        moveable_sr.emplace_back(sr);
                        break;
                        // cout<<c->name<<" can still move forward!!"<<endl;
                    }
                    last_point = c->x + c->w;
                }
            }
        }
        vector<subrow*> nofullsrs;
        for(auto illsr : sr_have_ill_cell){
            if(find(moveable_sr.begin(), moveable_sr.end(), illsr) != moveable_sr.end()){
                cout<<illsr->name<<" is illegal and can move!!!"<<endl;
            }else{
                if(illsr->free_sites > 0){
                    nofullsrs.emplace_back(illsr);
                    cout<<illsr->name<<" is illegal, cannot move, and not full!!!"<<endl;
                }
                else{
                    cout<<illsr->name<<" is illegal, cannot move, but full!!!"<<endl;
                }
            }
        }
        for(auto sr:nofullsrs){
            for(auto c:sr->cells){
                if(c->get_dis() > max_dis){
                    // cout<<"subrow: "<<sr->name<<endl;
                    // c->print_cell();
                } 
            }
        }
        // legalize_cells(sr_have_ill_cell);
        legalize_cells_2(over_max_cells);
        OutputFile(argv[2]);
        for (auto ptr : cells)delete ptr; 
        for (auto ptr : rows)delete ptr;
        return 0;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

double get_cell_dis(cell* c1, cell* c2){
    return sqrt(pow(c1->x - c2->x, 2) + pow(c1->y - c2->y, 2));
}

double get_point_dis(double x1, double y1, double x2, double y2){
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int find_cells_to_swap(cell* c,row* r){
    for(auto sr : r->subrows){
        // int free_width=0;
        // cell* start_cell=nullptr;
        for(auto sr_c : sr->cells){
            // if not same size then skip
            if(sr_c->w != c->w)continue;
            // if one of it move to other place will greater than maximum dis then skip
            if(get_point_dis(c->g_x,c->g_y,sr_c->x,sr_c->y) > max_dis || get_point_dis(c->x,c->y,sr_c->g_x,sr_c->g_y) > max_dis){
                continue;
            }
            // if(get_point_dis(c->x + free_width,c->y,sr_c->x,sr_c->y) > max_dis){free_width=0;start_cell=nullptr;continue;}
            // else swap them and return 1
            swap(c->x,sr_c->x);
            swap(c->y,sr_c->y);
            return 1;
        }
    }
    return 0;
}

void legalize_cells_2(vector<cell*>& over_max_cells){
    for(auto c : over_max_cells){
        int row_idx = get_closest_row(c);
        int success = find_cells_to_swap(c,rows[row_idx]);
        if(success){cout<<"Successful found for cell"<<c->name<<endl;continue;}

        int down_row_idx = row_idx - 1;
        while(down_row_idx >= 0 && abs(c->g_y - rows[down_row_idx]->y) < max_dis){
            success = find_cells_to_swap(c,rows[down_row_idx]);
            if(success)break;
            down_row_idx--;
        }
        if(success){cout<<"Successful found for cell"<<c->name<<endl;continue;}

        int up_row_idx = row_idx + 1;
        while(up_row_idx < (int)rows.size() && abs(c->g_y - rows[up_row_idx]->y) < max_dis){
            success = find_cells_to_swap(c,rows[up_row_idx]);
            if(success)break;
            up_row_idx++;
        }
        if(success){cout<<"Successful found for cell"<<c->name<<endl;continue;}
        cout<<"Failed for cell "<<c->name<<endl;

    }
}

void cells_alignment3(){
    for (const auto r : rows){
        for (const auto sr : r->subrows){
            Cluster::ptr last_cluster = sr->last_cls;
            while (last_cluster){
                int tmp_x = r->getSiteX(last_cluster->x);
                for (auto c : last_cluster->cells){
                    c->x = tmp_x;
                    c->y = r->y;
                    tmp_x += c->w;
                }
                last_cluster = last_cluster->pre;
            }
        }
    }
}


void legalize_cells(vector<subrow*>& illegal_subrow){
    int count=0;
    for(auto ill_sr : illegal_subrow){
        for(auto ill_c : ill_sr->cells){
            int state=-1;
            // left_top == 1, right_top == 2
            // left_btm == 3, right_btm == 4
            if(ill_c->get_dis() > max_dis){
                if(ill_c->g_x >= ill_c->x && ill_c->g_y <= ill_c->y)
                    state = 1;
                else if(ill_c->g_x < ill_c->x && ill_c->g_y <= ill_c->y)
                    state = 2;
                else if(ill_c->g_x >= ill_c->x && ill_c->g_y > ill_c->y)
                    state = 3;
                else if(ill_c->g_x < ill_c->x && ill_c->g_y > ill_c->y)
                    state = 4;

                cout<<ill_c->name<<" is at state "<<state<<endl;
                count++;
            }
        }
    }
    cout<<"total "<<count<<" illegal cells"<<endl;
}

// if infront of cell still have space and cell not access max constraint then move forward
void cells_alignment() {
    for(auto r : rows) {
        int site_width = r->s_w;
        for(auto sr : r->subrows) {
            Cluster::ptr last_cluster = sr->last_cls;
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
                    c->y = r->y;
                    opt_x += c->w;//ceil(c->w / site_width) * site_width;
                }

                last_cluster = last_cluster->pre;
            }
        }
    }
}

void delete_subrow(subrow* subr) {
    for (Cluster::ptr cls : subr->clusters) {
        cls.reset();
    }
    delete subr;
}

int get_closest_row(cell* c) {
    double min_dist = abs(c->g_y - rows[0]->y);

    // Iterate through the remaining rows and find the closest one
    for(size_t i = 1; i < rows.size(); i++) {
        double curr_dist = abs(c->g_y - rows[i]->y);
        if(min_dist > curr_dist) {
            min_dist = curr_dist;
        } else {
            return i - 1;
        }
    }
    return rows.size() - 1;
}




void abacus(){
    sort(cells.begin(), cells.end(), [](cell* a, cell* b) {
        return a->g_x < b->g_x;
    });
    cout<<"Sorting done, maximum distance ="<<max_dis<<endl;
    cout<<"Have "<<rows.size()<<" rows, "<< cells.size()<<" cells, "<<blockages.size()<<" blockages"<<endl;
    // int count =0;
    for(auto c : cells){
        // count++;
        // cout<<"cell "<<c->name<<" processing "<<endl;
        //get closest row
        int row_idx = get_closest_row(c);
        row* best_row=nullptr;// = rows[row_idx];
        subrow* best_subrow=nullptr;
        double best_cost = numeric_limits<double>::max();
        // auto [best_subrow,best_cost] = best_row->place_row_trial(c,max_dis);
        // double best_cost = c->get_dis();
        // cout<<"row id = "<<row_idx<<endl;
        for (int round = 1; round >= 0; --round){
            //search downward
            int down_row_idx = row_idx;
            // cout<<"Searching downward"<<endl;
            while(down_row_idx >= 0 && abs(c->g_y - rows[down_row_idx]->y) < best_cost) {
                
                auto [sr,cost] = rows[down_row_idx]->place_row_trial(c,max_dis,round);
                // double cost = c->get_dis();
                if(cost < best_cost) {
                    best_cost = cost;
                    best_subrow = sr;
                    best_row = rows[down_row_idx];
                }
                down_row_idx--;
            }

            // Search up until the y_cost is greater than the minimal cost
            int up_row_idx = row_idx + 1;
            // cout<<"Searching upward"<<endl;
            while(up_row_idx < (int)rows.size() && abs(c->g_y - rows[up_row_idx]->y) < best_cost){
                auto [sr,cost] = rows[up_row_idx]->place_row_trial(c,max_dis,round);
                // double cost = c->get_dis();
                if(cost < best_cost) {
                    best_cost = cost;
                    best_subrow = sr;
                    best_row = rows[up_row_idx];
                }
                up_row_idx++;
            }

            if (best_subrow != nullptr)
                break;
        }
    
        
        // cout<<"best row = "<<best_row->name<<", best subrow = "<<best_subrow->name<<endl;
        if(best_subrow == nullptr){
            cout<<"cell "<< c->name<<" cannot be place!!!"<<endl;
            continue;
        }
        

        // if(best_cost > max_dis)cout<<"OMGOMGOMG "<<best_cost<<endl;
        best_row->place_row_final(c, best_subrow,max_dis);

        // int tmp2 = 0;
        // for(auto cls : best_subrow->clusters){
        //     tmp2 += cls->cells.size();
        // }
        // cout<<"before = "<<tmp<<", after = "<<tmp2<<endl;
        // cout<<"after, clusters : "<<best_subrow->clusters.size()<<endl;
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
        double b_min_X = blockage.x;
        double b_max_X = blockage.x + blockage.w;
        double b_min_Y = blockage.y;
        double b_max_Y = blockage.y + blockage.h;
        for(row* r: rows){
            if (r->y + r->r_h <= b_min_Y || b_max_Y <= r->y)continue;

            bool start=false;
            // assign into row's sites
            for(size_t i=0;i<r->sites.size();i++){
                if(r->x + i*r->s_w <= b_min_X && r->x + (i+1)*r->s_w > b_min_X)
                    start=true;
                if(start){
                    if(r->x + i*r->s_w < b_max_X){
                        r->sites[i]=true;
                    }else {start=false;break;}
                }
            }
        }
    }
}


