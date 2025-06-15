#pragma once
#include "cell.hpp"
// #include "subrow.hpp"
#include <vector>

class subrow;

class Cluster{
public:
    using ptr = std::shared_ptr<Cluster>;
    int width;
    int x,y;
    int q_c;
    std::vector<cell*> cells;
    Cluster::ptr pre;
    cell *last_c;
    cell *first_c;
    int weight=0;
    int left_move=INT_MAX;
    // subrow *subr;

    Cluster():width(0),x(0),pre(nullptr),weight(0),left_move(INT_MAX) {}
    Cluster(int x,int y, Cluster::ptr pre, int left_move):width(0),x(x),y(y),q_c(0),pre(pre),last_c(nullptr),first_c(nullptr),weight(0),left_move(left_move) {}
    Cluster(const Cluster& other) {
        width = other.width;
        x = other.x;
        y = other.y;
        q_c = other.q_c;
        last_c = other.last_c;
        first_c = other.first_c;
        cells = other.cells;
        pre = nullptr;
    }

    void add_cell(cell* c,int opt_x);
    void add_cluster(Cluster* cls);
    // void collapse(subrow* sr);
};