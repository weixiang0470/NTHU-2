#include "datastr.hpp"

//Coordinate
/*
Coord::Coord(){
  x=-1;
  y=-1;
}

Coord::Coord(int x,int y){
  this->x = x;
  this->y = y;
}
*/
//Pad
Pad::Pad(){
  name = "pad-1";
  coord = Coord();
}

Pad::Pad(std::string name,Coord coord){
  this->name = name;
  this->coord = coord;
}

//Hardblock
HardB::HardB(){
  name = "hb-1";
  coord = Coord();
  w = 0;
  h = 0;
  r = 0;
}
HardB::HardB(std::string name,Coord coord,int w,int h){
  this->name = name;
  this->coord = coord;
  this->w = w;
  this->h = h;
}

int HardB::W(){
  if(r) return h;
  return w;
}
int HardB::H(){
  if(r) return w;
  return h;
}
int HardB::A(){
  return w*h;
}

Coord HardB::get_center(){
   return Coord(coord.x + W()/2 , coord.y + H()/2);
}
//Net
/*Net::Net(){
  pads = std::vector<Pad*>();
  hardblocks = std::vector<HardB*>();
}*/
void Net::add_pad(Pad* pad){
  pads.emplace_back(pad);
}
void Net::add_hardblock(HardB* hardblock){
  hardblocks.emplace_back(hardblock);
}
int Net::HPWL(){
  int min_x = INT_MAX, max_x = INT_MIN;
  int min_y = INT_MAX, max_y = INT_MIN;
  
  for(auto& pad: pads){
    Coord c = pad->coord;
    min_x = std::min(min_x, c.x);
    max_x = std::max(max_x, c.x);
    min_y = std::min(min_y, c.y);
    max_y = std::max(max_y, c.y);
  }
  
  for(auto& hb: hardblocks){
    Coord c = hb->get_center();
    min_x = std::min(min_x, c.x);
    max_x = std::max(max_x, c.x);
    min_y = std::min(min_y, c.y);
    max_y = std::max(max_y, c.y);
  }
  return (max_x - min_x + max_y - min_y); 
}

//Node
Node::Node(std::string name, int index, int width, int height, int l_from, int l_at,int r_from, int r_at, Coord coord){
  this->name = name;
  this->index = index;
  this->w = width;
  this->h = height;
  this->l_from = l_from;
  this->l_at = l_at;
  this->r_from = r_from;
  this->r_at = r_at;
  this->coord = coord;
}


// TreeNode
bool TreeNode::isOperator(const std::string& s){
  return s=="H" || s=="V";
}

// TreeNode* TreeNode::buildSlicingTree(const std::vector<std::string>& polishExpr){
//   std::stack<TreeNode*> stk;

//   for(const std::string& token : polishExpr){
//     if(isOperator(token)){
//       TreeNode* right = stk.top(); stk.pop();
//       TreeNode* left = stk.top(); stk.pop();
//       TreeNode* opNode = new TreeNode(token);
//       opNode->L = left;
//       opNode->R = right;
//       stk.push(opNode);
//     }
//     else{
//       stk.push(new TreeNode(token));
//     }
//   }

//   return stk.top();
// }