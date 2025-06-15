#pragma once
#include "cell.hpp"
#include "cluster.hpp"
#include <list>


class subrow{
public:
    std::string name="";
    int min_x,max_x;
    int y;
    int free_sites;
    int site_width;
    int total_sites;
    Cluster::ptr last_cls;
    std::vector<cell*> cells;
    std::vector<Cluster::ptr> clusters;

    ~subrow();
    subrow():min_x(0),max_x(0),free_sites(0),site_width(0),last_cls(nullptr){}
    subrow(int minx, int maxx, int y, int free_s,int s_w):min_x(minx), max_x(maxx),y(y), free_sites(free_s),site_width(s_w),total_sites(free_s),last_cls(nullptr){}

    // subrow* clone() const;
    void insert_cell_in_subrow(cell* c);
    void add_cell_in_cluster(Cluster::ptr cls, cell* i);
    void merge_cluster(Cluster::ptr c1, Cluster::ptr c2);
    void collapse(Cluster::ptr cur_cluster);

    void remove_cell_from_subrow(cell* c);
    // void add_cluster_in_subrow(Cluster::ptr cls);
    // void remove_cluster(Cluster::ptr target);
    void Place_subrow(cell* c);
    // void Place_subrow_trial(cell* c);
};