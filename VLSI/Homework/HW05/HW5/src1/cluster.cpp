// #pragma once
#include "cell.hpp"
#include "subrow.hpp"
#include "cluster.hpp"

void Cluster::add_cell(cell* c){
    cells.emplace_back(c);
    last_c = c;
    q_c += c->g_x  -  width;
    width += c->w;
}

void Cluster::add_cluster(Cluster* cls){
    if(this == cls) return;
    cells.insert(cells.begin(), cls->cells.begin(), cls->cells.end());
    // last_c = cls->last_c;
    q_c += cls->q_c - cls->width;
    width += cls->width;
    pre = cls->pre;
    first_c = cls->first_c;

    cls->cells.clear();
    cls->pre=nullptr;
}

void Cluster::collapse(subrow* sr){
    x = q_c;
    if(x < sr->min_x) x = sr->min_x;
    if(x > sr->max_x - width) x = sr->max_x - width;

    Cluster* current = this;
    while(current->pre != nullptr && current->pre->x + current->pre->width > x){
        Cluster* prev = current->pre;
        current->add_cluster(prev);
        sr->remove_cluster(prev);

        x = current->q_c;
        if(x < sr->min_x) x = sr->min_x;
        if(x > sr->max_x - current->width) x = sr->max_x - current->width;   
    }
}

// void Cluster::collapse(subrow* sr){
//     x = q_c;
//     if(x < subr->min_x) x = subr->min_x;
//     if(x > subr->max_x - width) x = subr->max_x - width;

//     if(pre!=nullptr && pre->x + pre->width > x){
//         add_cluster(pre);
//         subr->remove_cluster(pre);
//         // pre=nullptr;
//         collapse();
//     }
// }