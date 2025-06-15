#pragma once
#include <iostream>

class cell{
public:
    std::string name="";
    int w=0,h=0;
    double g_x=0.0,g_y=0.0;
    double x=0.0,y=0.0;
    int weight=1;
    double best_cost = std::numeric_limits<double>::max();
    cell(){}
    cell(std::string name,int w, int h, double x, double y):
        name(name),w(w),h(h),g_x(x),g_y(y),weight(1) {}

    void set_pos(double x, double y){
        this->x = x;
        this->y = y;
    }

    double get_dis(){
        double dx = g_x - x;
        double dy = g_y - y;
        return std::sqrt(dx*dx + dy*dy);
    }

    int required_site(int site_width){
        return std::ceil(this->w / site_width);
    }

    int max_d_after_place(int max_dis, int site_width){
        double y_dis = std::abs(this->y - this->g_y);
        double x_dis = std::abs(this->x - this->g_x);
        double movable_width = std::sqrt((max_dis * max_dis) - (y_dis * y_dis)) - x_dis;

        return std::floor(movable_width / site_width);
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