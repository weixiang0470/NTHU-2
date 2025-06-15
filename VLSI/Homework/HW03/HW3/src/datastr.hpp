#include <bits/stdc++.h>

class Coord{
public:
  int x,y;
  Coord():x(0),y(0){}
  Coord(int x, int y): x(x),y(y){}
};

class Pad{
public:
  std::string name;
  Coord coord;
  
  Pad();
  Pad(std::string name,Coord coord);
};

class HardB{
public:
  std::string name;
  Coord coord;
  int w,h;
  int r;
  
  HardB();
  HardB(std::string name,Coord coord,int w,int h);
  
  int W();
  int H();
  int A();
  Coord get_center();
};

class Net{
public:
  std::string name;
  std::vector<Pad*> pads;
  std::vector<HardB*> hardblocks;
  
  Net(): pads(),hardblocks(){}
  Net(std::string name): name(name),pads(),hardblocks(){}
  
  void add_pad(Pad* pad);
  void add_hardblock(HardB* hardblock);
  int HPWL(); 
};

class Dimension{
public:  
  int w,h;
  int r;
  int area() const{return w*h;}

};

// enum Nodetype {OPERAND,OPERATOR};
class Node{
public:
  std::string name;
  int index;
  int w,h;
  int l_from,l_at;
  int r_from,r_at;
  Coord coord;

  Node():name("none"){}
  Node(std::string name, int index, int width, int height, int l_from, int l_at,int r_from, int r_at, Coord coord);
};


class TreeNode{
public:  
  // std::string name;
  Node node;
  TreeNode* L;
  TreeNode* R;
  

  TreeNode(Node node): node(node), L(nullptr),R(nullptr) {}
  

  bool isOperator(const std::string& s);
  // TreeNode* buildSlicingTree(const std::vector<std::string>& polishExpr);
  
};