#pragma once
#include <iostream>


class cell{
public:
    std::string name="";
    int w=0,h=0;
    double g_x=0.0,g_y=0.0;
    double x=0.0,y=0.0;
    double best_cost = std::numeric_limits<double>::max();
    cell(){}
    cell(std::string name,int w, int h, double x, double y):
        name(name),w(w),h(h),g_x(x),g_y(y){}

    void set_pos(double x, double y){
        this->x = x;
        this->y = y;
    }

    double get_dis(){
        return std::sqrt(pow(g_x - x, 2) + pow(g_y - y, 2));
    }

    void print_cell(){
        std::cout<<"name:"<<name<<" Width:"<<w<< " Height:"<<h<<" G_X:"<<g_x<<" G_Y:"<<g_y<<" X:"<<x<<" Y:"<<y<<" cost="<<get_dis()<<std::endl;
    }
};

class blockage{
public:
    std::string name;
    int w,h;
    double x,y;

    blockage(){}
    blockage(std::string name,int w,int h,double x, double y): 
        name(name), w(w), h(h), x(x), y(y) {}

    void print_block(){
        std::cout<<"name: "<<name<<" Width: "<<w<< " Height: "<<h<<" X: "<<x<<" Y: "<<y<<std::endl;
    }
};