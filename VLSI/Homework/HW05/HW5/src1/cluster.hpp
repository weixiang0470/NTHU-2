#pragma once
#include "cell.hpp"
// #include "subrow.hpp"
#include <vector>
#include <list>


class subrow;

class Cluster{
public:
    int width;
    int x,y;
    int q_c;
    std::vector<cell*> cells;
    Cluster *pre;
    cell *last_c;
    cell *first_c;
    // subrow *subr;

    Cluster():width(0),x(0),pre(nullptr) {}
    Cluster(int x,int y, Cluster* pre):width(0),x(x),y(y),q_c(0),pre(pre),last_c(nullptr),first_c(nullptr) {}
    Cluster(const Cluster& other) {
        width = other.width;
        x = other.x;
        y = other.y;
        q_c = other.q_c;
        last_c = other.last_c;
        first_c = other.first_c;
        cells = other.cells;

        // 不拷貝 pre 和 subr，留給 clone() 處理
        pre = nullptr;
        // subr = nullptr;
    }

    void add_cell(cell* c);
    void add_cluster(Cluster* cls);
    void collapse(subrow* sr);
};