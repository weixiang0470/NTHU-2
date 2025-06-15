#pragma once
#include "cell.hpp"
#include "cluster.hpp"
#include "subrow.hpp"

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

    void print_subrows() {
        for (auto sr : subrows) {
            std::cout << "Subrow:"<<sr->name <<" [" << sr->min_x << ", " << sr->max_x 
                    << "], width: " << sr->free_sites<<", total sites: " << sr->total_sites <<", Y: "<<sr->y<< " clusters:"<<sr->clusters.size()<<std::endl;
        }
    }

    void print_sites(){
        for(auto s : sites){
            std::cout<<s<<"-";
        }
        std::cout<<std::endl;
    }
};