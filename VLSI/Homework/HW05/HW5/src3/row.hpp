#pragma once
#include "cell.hpp"
#include "cluster.hpp"
#include "subrow.hpp"
#include <bits/stdc++.h>

class row{
public:
    std::string name;
    int s_w, r_h;
    double x,y;
    std::vector<bool> sites;
    std::vector<subrow*> subrows;    

    row(){}
    row(std::string name, int w, int h, double x, double y, int num):
        name(name), s_w(w), r_h(h), x(x), y(y), sites(num,false) {}

    void print_row(){
        std::cout<<" "<<name<<" "<<s_w<< " "<<r_h<<" "<<x<<" "<<y<<" "<<sites.size()<<std::endl;
    }

    void construct_subrows(){
        int start = -1;
        for(size_t  i=0;i<=sites.size();i++){
            if(i<sites.size() && !sites[i]){
                if (start==-1)start = i;
            }else{
                if (start != -1){
                    int end = i;
                    // int length = end - start;

                    int min_x = x + start*s_w;
                    int max_x = x + end*s_w;

                    subrow* sr = new subrow(min_x,max_x,y,end-start,s_w);
                    sr->name = name +"_"+ std::to_string(start);
                    subrows.emplace_back(sr);
                    start = -1;
                }
            }
        }
    }

    int get_closest_subrow(cell* c) {
        // credited to Github EricLu1218 : https://github.com/EricLu1218/Physical_Design_Automation/blob/main/Placement_Legalization/src/Legalizer/Legalizer.cpp
        std::vector<int> candidateSubrow;
        for(int i=0;i<(int)subrows.size();++i){
            if(c->required_site(s_w) <= subrows[i]->free_sites)
                candidateSubrow.emplace_back(i);
        }

        auto getMinDis = [&](int idx) -> double
        {
            double dis = 0;
            // if cell is infront of the subrow then need to move at least min_x of subrow from global x position
            if(c->g_x < subrows[idx]->min_x) dis = subrows[idx]->min_x - c->g_x;
            // if cell is behind the subrow then to move the cell into the subrow need at least this distance
            else if(c->g_x + c->w > subrows[idx]->max_x) dis = c->g_x + c->w - subrows[idx]->max_x;
            return dis;
        };

        int subrow_id = -1;
        double min_dis = std::numeric_limits<double>::max();

        for(auto idx : candidateSubrow){
            double dis = getMinDis(idx);
            if(dis < min_dis){
                min_dis = dis;
                subrow_id = idx;
            }else break;
        }
        return subrow_id;
    }

    double get_cost(cell* c,subrow* sr, int max_dis){
        return c->get_dis();
        double d = c->get_dis();
        double space_ratio = (double)sr->free_sites/sr->total_sites;
        double penalty = (1.0 - space_ratio)*max_dis/10; 
        if(d > max_dis) penalty += max_dis*10;
        
        return d + penalty;
    }

    std::pair<subrow*,double> place_row_trial(cell* c,int max_dis){
        int subrow_id = get_closest_subrow(c);

        if(subrow_id == -1)
            return {nullptr,std::numeric_limits<double>::max()};
        
        subrow* sr = subrows[subrow_id];
        double opt_x = c->g_x;
        if(opt_x < sr->min_x) opt_x = sr->min_x;
        else if(opt_x > sr->max_x - c->w) opt_x = sr->max_x - c->w;

        Cluster::ptr last_cluster = sr->last_cls;
        if(last_cluster == nullptr || last_cluster->x + last_cluster->width <= opt_x) {
            c->x = opt_x;
            c->y = y;
        } else {
            int max_move = last_cluster->left_move;
            int c_weight = last_cluster->weight + c->weight;
            int c_width = last_cluster->width + c->w;
            double c_q = last_cluster->q_c + c->weight * (opt_x - last_cluster->width); // new q
            double c_x = 0; // new x
            double current_c_opt = last_cluster->q_c;


            while(1){
                c_x = c_q/c_weight;
                if(c_x < sr->min_x) c_x = sr->min_x;
                else if(c_x > sr->max_x - c_width) c_x = sr->max_x - c_width;

                Cluster::ptr pre_cluster = last_cluster->pre;
                // if overlap with previous cluster
                if(pre_cluster!=nullptr && pre_cluster->x + pre_cluster->width > c_x) {
                    // collapse with the previous cluster
                    c_q = pre_cluster->q_c + c_q - c_weight * pre_cluster->width;
                    c_weight = pre_cluster->weight + c_weight;
                    c_width = c_width + pre_cluster->width;
                    last_cluster = pre_cluster;
                } else break; // no overlap then done
            }
            if(std::ceil(std::abs(c_x - current_c_opt)/s_w) > max_move)std::cout<<"place at here will make other cell over maximum constraints at trial placerow"<<std::endl;
            c->x = c_x + c_width - c->w; // place at the end of the cluster;
            c->y = y;// place at this row
        }

        return {sr,get_cost(c,sr,max_dis)}; // return the subrow id where the cell is placed
    }

    void place_row_final(cell* c, subrow* sr, int max_dis){
        sr -> free_sites -= c->required_site(s_w);//ceil(c->w / s_w);

        double opt_x = c->x;
        if(opt_x < sr->min_x) opt_x = sr->min_x;
        else if(opt_x > sr->max_x - c->w) opt_x = sr->max_x - c->w;

        Cluster::ptr last_cluster = sr->last_cls;
        
        if(!last_cluster || last_cluster->x + last_cluster->width <= opt_x) {
            sr->last_cls = Cluster::ptr(new Cluster(opt_x, y, last_cluster,c->max_d_after_place(max_dis,s_w)));
            last_cluster = sr->last_cls;

            last_cluster->add_cell(c,opt_x);
            last_cluster->left_move = c->max_d_after_place(max_dis,s_w);
            sr->last_cls = last_cluster;
        } else {
            double current_c_opt = last_cluster->q_c;
            last_cluster->add_cell(c,opt_x);
            last_cluster->left_move = std::min(last_cluster->left_move,c->max_d_after_place(max_dis,s_w));

            while(1){
                if(last_cluster->x < sr->min_x) last_cluster->x = sr->min_x;
                else if(last_cluster->x > sr->max_x - last_cluster->width) last_cluster->x = sr->max_x - last_cluster->width;
                
                Cluster::ptr pre_cluster = last_cluster->pre;
                if(pre_cluster!=nullptr && pre_cluster->x + pre_cluster->width > last_cluster->x) {
                    // collapse with the previous cluster
                    pre_cluster->cells.insert(pre_cluster->cells.end(), last_cluster->cells.begin(), last_cluster->cells.end());
                    pre_cluster->weight += last_cluster->weight;
                    pre_cluster->q_c += last_cluster->q_c - last_cluster->weight * pre_cluster->width;
                    pre_cluster->width += last_cluster->width;
                    pre_cluster->left_move = std::min(last_cluster->left_move,pre_cluster->left_move);
                    if(std::ceil(std::abs(pre_cluster->q_c - current_c_opt)/s_w) > pre_cluster->left_move)std::cout<<"Someone make others cell over maximum constraints at final placerow"<<std::endl;
                    last_cluster = pre_cluster;
                }else break;

            }
            sr->last_cls = last_cluster; // update the last cluster  
        }

    }

    void print_subrows() {
        for (auto sr : subrows) {
            std::cout << "Subrow:"<<sr->name <<" [" << sr->min_x << ", " << sr->max_x 
                    << "], width: " << sr->free_sites<<", ratio="<<(double)sr->free_sites/sr->total_sites<<", total sites: " << sr->total_sites <<", Y: "<<sr->y<< " clusters:"<<sr->clusters.size()<<std::endl;
        }
    }

    void print_sites(){
        for(auto s : sites){
            std::cout<<s<<"-";
        }
        std::cout<<std::endl;
    }
};