#pragma once
#include <fstream> 
#include <sstream>
#include "row.hpp"


class InputParser {
public:
    int MaxDisC = 0;
    int NumCells = 0;
    int NumBlockages = 0;
    int NumRows = 0;
    void parseInput(const std::string& filename,std::vector<cell*>& cells,std::vector<blockage>& blockages,std::vector<row*>& rows){
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line.substr(0, 2) == "//") continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;
            // std::cout<<" token = "<< token<<std::endl;

            if (token == "MaxDisplacementConstraint") {
                // int MaxDisC;
                if (!(iss >> MaxDisC)) {
                    throw std::runtime_error("Invalid max displacement constraint format");
                }
            } else if (token == "NumCells") {
                // int numCells;
                if (!(iss >> NumCells)) {
                    throw std::runtime_error("Invalid numCells format");
                }
                // cells.resize(numCells);
            } else if (token == "Cell"){
                std::string name;
                int width,height;
                double x,y;
                iss >> name >> width >> height >> x >> y;
                // std::cout<<name<<" "<<width<<" "<<height<<" "<<x<<" "<<y<<std::endl;
                cell* c = new cell(name,width,height,x,y);
                cells.emplace_back(c);
            } else if(token == "NumBlockages"){
                iss >> NumBlockages;
            } else if(token == "Blockage"){
                std::string name;
                int width,height;
                double x,y;
                iss >> name >> width >> height >> x >> y;
                blockage b(name,width,height,x,y);
                blockages.emplace_back(b);
            }else if(token == "NumRows"){
                iss >> NumRows;
            }else if (token == "Row"){
                std::string name;
                int s_w,r_h;
                double x,y;
                int s_n;
                iss >> name >> s_w >> r_h >> x >> y >> s_n;
                // std::cout<<"Row : "<<name<<std::endl;
                row* r = new row(name,s_w,r_h,x,y,s_n);
                rows.emplace_back(r);
                // delete r;
            }else{
                token="";
                continue;
            }
        }
    }

    // std::vector<cell> get_cells(){
    //     return cells;
    // }
    // std::vector<blockage> get_blocks(){
    //     return blockages;
    // }
    // std::vector<row> get_rows(){
    //     return rows;
    // }

};