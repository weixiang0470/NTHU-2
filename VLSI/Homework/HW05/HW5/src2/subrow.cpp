// #pragma once
#include "subrow.hpp"
#include "cluster.hpp"

subrow::~subrow() {
    for (Cluster* c : clusters) {
        delete c;
    }
}

subrow* subrow::clone() const {
    auto* new_subr = new subrow(min_x, max_x, y, free_sites, site_width);
    new_subr->name = name;
    new_subr->total_sites = total_sites;
    new_subr->cells = cells;  // Shallow copy

    std::unordered_map<Cluster*, Cluster*> old_to_new;
    
    for (Cluster* old_cls : clusters) {
        Cluster* new_cls = new Cluster(*old_cls);
        old_to_new[old_cls] = new_cls;
        new_subr->clusters.push_back(new_cls);
    }

    // Rebuild relationships
    for (Cluster* old_cls : clusters) {
        if (old_cls->pre) {
            old_to_new[old_cls]->pre = old_to_new[old_cls->pre];
        }
    }

    new_subr->last_cls = last_cls ? old_to_new[last_cls] : nullptr;
    return new_subr;
}


void subrow::insert_cell_in_subrow(cell* c){
    cells.emplace_back(c);
}

void subrow::add_cell_in_cluster(Cluster* cls, cell* i){
    cls->cells.emplace_back(i);
    cls->last_c = i;
    cls->q_c += i->g_x - cls->width;
    cls->width += i->w;
}

void subrow::merge_cluster(Cluster* c1, Cluster* c2){
    c1->cells.insert(c1->cells.end(), c2->cells.begin(), c2->cells.end());
    c1->last_c = c2->last_c;
    c1->q_c += c2->q_c - c1->width;
    c1->width += c2->width;

    c2->cells.clear();
    c2->first_c = nullptr;
    c2->last_c = nullptr;
    
    remove_cluster(c2); 
}

void subrow::collapse(Cluster* cur_cluster){
    cur_cluster->x = cur_cluster->q_c;
    if(cur_cluster->x < min_x)cur_cluster->x = min_x;
    if(cur_cluster->x > max_x - cur_cluster->width)cur_cluster->x = max_x - cur_cluster->width;

    Cluster* prev = cur_cluster->pre;
    if(prev && prev->x + prev->width > cur_cluster->x){
        merge_cluster(prev,cur_cluster);
        // remove_cluster(cur_cluster);
        collapse(prev);
    }
}

void subrow::remove_cell_from_subrow(cell* c){
    auto it = std::find(cells.begin(), cells.end(), c);
    if(it != cells.end()) {
        cells.erase(it);
        // std::cout<<"Removing cell from subrow"<<std::endl;
    }
}

void subrow::add_cluster_in_subrow(Cluster* cls){
    last_cls = cls;
    // free_width -= cls->width;
    clusters.emplace_back(cls);
    // last_cls = cls;
}

void subrow::remove_cluster(Cluster* target) {
    target->cells.clear();
    target->first_c = nullptr;
    target->last_c=nullptr;
    clusters.remove_if([&](Cluster* c) {
        if (c == target) {
            // 移除调试输出
            delete c;
            return true;
        }
        return false;
    });

    if (last_cls == target) {
        last_cls = clusters.empty() ? nullptr : clusters.back();
    }
}

// void subrow::remove_cluster(Cluster* target){
//     // for(size_t i=0;i<clusters.size();i++){
//     //     if(clusters[i] == target){
//     //         std::cout << "Deleting cluster: " << clusters[i] << std::endl;
//     //         delete clusters[i];
//     //         clusters.erase(clusters.begin() + i);
//     //         break;
//     //     }
//     // }
//     for(auto it = clusters.begin(); it!=clusters.end();++it){
//         // if(*it == nullptr){clusters.erase(it);continue;}
//         if( *it == target){
//             std::cout << "Deleting cluster: " << *it << std::endl;
//             delete *it;
//             *it = nullptr;
//             clusters.erase(it);
//             break;
//         }
//     }
// }

// void subrow::Place_subrow_trial(cell* c){
//     double opt_x = c->x;
//     if(opt_x < min_x)opt_x = min_x;
//     else if(opt_x > max_x - c->w)opt_x = max_x - c->w;

//     Cluster* last_cluster = subrow->last_cls;
//     if(last_cluster == nullptr || last_cluster->x + last_cluster->width <= opt_x){
//         c->x = opt_x;
//     }else{
//         int  c_width = last_cluster->width + c->w;
//         double c_q = last_cluster->q_c + (c->g_x - last_cluster->width);
//     }
// }

// void subrow::collapse(){

// }

void subrow::Place_subrow(cell* c){
    if(!last_cls || last_cls->x + last_cls->width <= c->g_x){
        auto cur_cls = new Cluster(c->g_x,y,last_cls);
        // cur_cls->subr = this;
        // cur_cls->first_c = c;
        
        if(cur_cls->x < min_x)cur_cls->x = min_x;
        else if(cur_cls->x > max_x - cur_cls->width)cur_cls->x = max_x - cur_cls->width;

        add_cell_in_cluster(cur_cls,c);
        clusters.emplace_back(cur_cls);
        last_cls = cur_cls;
    }else{
        add_cell_in_cluster(last_cls,c);
        collapse(last_cls);
    }
    
    for (auto cls : clusters) {
        // cls->collapse();
        // Cluster* cls = cls_ptr;  
        // if(cls->x % site_width != 0)cls->x = ((cls->x / site_width) + 1) * site_width;
        cls->x = floor(cls->x / site_width + 0.5) * site_width;

        int clsx = cls->x;
        if(clsx % site_width != 0){
            clsx = ((clsx / site_width) + 1) * site_width;
        }
        for(cell* c: cls->cells){
            c->set_pos(clsx,y);
            // c->x = clsx;
            // c->y = y;
            clsx += c->w;
        }
    }
}

// void subrow::Place_subrow(){
//     for(auto* c : clusters) delete c;
//     clusters.clear();
//     last_cls = nullptr;
    
//     for(size_t i=0;i<cells.size();i++){
//         cell* c = cells[i];

//         if(!last_cls || last_cls->x + last_cls->width <= c->g_x){
//             auto cur_cls = new Cluster(c->g_x,y,last_cls);
//             // cur_cls->subr = this;
//             cur_cls->first_c = c;
            
//             if(cur_cls->x < min_x)cur_cls->x = min_x;
//             else if(cur_cls->x > max_x - cur_cls->width)cur_cls->x = max_x - cur_cls->width;

//             add_cell_in_cluster(cur_cls,c);
//             clusters.emplace_back(cur_cls);
//             last_cls = cur_cls;
//         }else{
//             add_cell_in_cluster(last_cls,c);
//             collapse(last_cls);
//         }
//     }
    
//     for (auto cls : clusters) {
//         // cls->collapse();
//         // Cluster* cls = cls_ptr;  
//         int clsx = cls->x;
//         for(cell* c: cls->cells){
//             c->set_pos(clsx,y);
//             // c->x = clsx;
//             // c->y = y;
//             clsx += c->w;
//         }
//     }
// }