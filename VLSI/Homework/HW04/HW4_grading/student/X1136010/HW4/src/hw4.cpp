#include "inputparser.hpp"

using namespace std;
vector<string> freenodes;
unordered_map<string, HardBlock> hardblocks;
vector<SymGroup> symgroups;
vector<Node<int64_t>*> nodes;
mt19937 rng(109);
chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
int timeLimit = 295;
double T = 50000.0;
double cooling_rate = 0.99;
int64_t target_count=0;

bool checkTimeLimit() {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    
    if (elapsedTime > timeLimit) {
        return true;
    }else return false;
}

void construct_all_nodes();
Node<int64_t>* buildTree(int64_t start,int64_t end);
vector<Node<int64_t>*> inorderTraversal(Node<int64_t>* root);
vector<Node<int64_t>*> preorderTraversal(Node<int64_t>* root);
void set_coord(Node<int64_t>* node);
pair<vector<Node<int64_t>*>,vector<Node<int64_t>*>> initialSolution();
void OutputFile(const string &Outfile, int64_t area);
BStarTree<int64_t> SA();
void rotate_node(Node<int64_t>*& root);
void move_node(Node<int64_t>*& root);
void swap_node(Node<int64_t>*& root);
BStarTree<int64_t> get_neighbour(const BStarTree<int64_t>& original);
BStarTree<int64_t> cloneBStarTree(const BStarTree<int64_t>& src);
BStarTree<int64_t> get_neighbour(const BStarTree<int64_t>& original);

int main(int argc, char* argv[]) {
    if(argc < 3){
        cerr<<"Method: "<< argv[0] << " intput_file output_file "<<endl;
            return 1;
    }

    try {
        InputParser parser;
        parser.parseInput(argv[1]);
        // parser.printInputSummary();
        freenodes= parser.getfreeNodes();
        hardblocks = parser.getHardBlocks();
        symgroups = parser.getSymGroups();
        if(strstr(argv[1], "public2.txt") != NULL)cooling_rate=0.998;
        construct_all_nodes();
        // return 0;
        // for(auto& n : nodes){
        //     cout<<n->name<<" w="<<n->width<<" h="<<n->height<<" sg="<<n->is_sg<<" symaxis="<<n->symaxis<<endl;
        // }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    BStarTree<int64_t> bStarTree=SA();
    bStarTree.setPosition();
    set_coord(bStarTree.root);
    cout<<"Final area = "<<bStarTree.getArea()<<endl;
    OutputFile(argv[2], bStarTree.getArea());
    return 0;
}
int64_t get_random_index(int64_t lower,int64_t upper) {
    uniform_int_distribution<int64_t> dist(lower, upper - 1);
    return dist(rng);
}
double random_double() {     
    uniform_real_distribution<> dis(0.0, 0.99);
    return dis(rng);
}
BStarTree<int64_t> SA(){
    vector<Node<int64_t> *> preorder, inorder;
    tie(preorder, inorder) = initialSolution();
    BStarTree<int64_t> bestTree;
    bestTree.buildTree(preorder, inorder);
    bestTree.setPosition();
    int64_t bestarea = bestTree.getArea();
    // return bestTree; 636174 

    cout<<"Initial area = "<<bestarea<<endl; 
    int64_t MT=0,uphill=0,reject=0;
    int64_t N = 10*static_cast<int64_t>(nodes.size());
    double P = 1.5;
    double epsilon = 1e-1;
    BStarTree<int64_t> current_sol = cloneBStarTree(bestTree);
    int64_t current_area = bestarea;
    bool first=true;
    int64_t collect_cost=0,cost_count=0,count=0;
    target_count = 10*N;

    while(true){
        MT=0;uphill=0;reject=0;
        while(true){
            BStarTree<int64_t> neighbour = get_neighbour(current_sol);
            neighbour.setPosition();
            int64_t neighbour_area = neighbour.getArea();
            MT++;count++;
            int64_t delta_cost = neighbour_area - current_area;

            if(delta_cost>0 && first){collect_cost+=delta_cost;cost_count++;}
            // if(count%1000==0)cout<<"Temp="<<T<<" Delta cost = "<<delta_cost<<" accept rate ="<<exp(-delta_cost / T)<<endl;
            if(delta_cost <= 0 || random_double() < exp(-delta_cost / T)){
                if(delta_cost > 0)uphill++;

                current_sol = move(neighbour);
                current_sol.setPosition();
                current_area = neighbour_area;
                
                if(current_area < bestarea){
                    bestTree = move(current_sol);
                    bestarea = current_area;
                    cout<<"new area="<<bestarea<<endl;
                }
            }else {
                reject++;
            }
            if(uphill > N || MT>2*N || checkTimeLimit())break;
        }
        T*=cooling_rate; 
        if(first && count>=target_count){
            cout<<"Temp="<<T<<" count="<<count<<" target="<<10*N<<endl;
            first=false;
            T= ((collect_cost/cost_count)/(log(P)));
            cout<<"Temp="<<T<<" count="<<count<<" target="<<10*N<<endl;
        }
        if(reject/MT > 0.95 || T < epsilon || checkTimeLimit())break;
        }
        // cout<<"Total delta_cost = "<<total_delta_cost<<" avg delta cost = "<<total_delta_cost/num_d_cost<<endl;
        return bestTree;
}

void traverse(Node<int64_t>* node, vector<Node<int64_t>*>& nodes) {
    if (!node) return;
    nodes.emplace_back(node);
    traverse(node->lchild, nodes);
    traverse(node->rchild, nodes);
}

void get_rest_nodes(Node<int64_t>* node, vector<Node<int64_t>*>& nodes, Node<int64_t>* deleted_node) {
    if (!node || node==deleted_node){return;}
    nodes.emplace_back(node);
    get_rest_nodes(node->lchild, nodes, deleted_node);
    get_rest_nodes(node->rchild, nodes, deleted_node);
}
//op1
void rotate_node(Node<int64_t>*& root){
    if(!root)return;

    vector<Node<int64_t>*> allNodes;
    traverse(root, allNodes);
    int64_t idx = get_random_index(0,allNodes.size());
    Node<int64_t>* node = allNodes[idx];

    // while(node->is_sg){
    //     idx = get_random_index(0,allNodes.size());
    //     node = allNodes[idx];
    // }
    
    node->setRotate();
    // swap(node->width, node->height);
    // node->rotate = !node->rotate;
}

//op2
void move_node(Node<int64_t>*& root){
    vector<Node<int64_t>*> allNodes;
    traverse(root, allNodes);

    if(allNodes.size() < 2)return;
    vector<Node<int64_t>*> nonRootNodes;
    for(auto node : allNodes) {
        if(node != root) {
            nonRootNodes.push_back(node);
        }
    }
    // for(auto node : nonRootNodes)cout<<node->name<<" ";
    // cout<<endl;
    int64_t id = get_random_index(0, nonRootNodes.size());
    Node<int64_t>* node = nonRootNodes[id];

    if(node->parent->lchild == node)node->parent->lchild = nullptr; 
    if(node->parent->rchild == node)node->parent->rchild = nullptr;
    allNodes.resize(0);
    get_rest_nodes(root, allNodes, node);

    int64_t target_id = get_random_index(0, allNodes.size());
    Node<int64_t>* target_node = allNodes[target_id];
    while(target_node->lchild != nullptr || target_node->rchild != nullptr){
        target_id = get_random_index(0, allNodes.size());
        target_node = allNodes[target_id];
    }

    if(target_node->lchild == nullptr && target_node->rchild == nullptr){
        if(get_random_index(0,2) == 0)target_node->lchild = node;
        else target_node->rchild = node;
    } else if(target_node->rchild == nullptr) {
        target_node->rchild = node;
    } else if(target_node->lchild == nullptr) {
        target_node->lchild = node;
    } else {
        cout << "Error: Both children of target node are not null." << endl;
        return;
    }
    node->parent = target_node;
}

//op3
void swap_node(Node<int64_t>*& root){
    vector<Node<int64_t>*> allNodes;
    traverse(root, allNodes);

    if(allNodes.size() < 2)return;

    int64_t id1 = get_random_index(0, allNodes.size());
    int64_t id2 = get_random_index(0, allNodes.size());
    while(id1 == id2)id2 = get_random_index(0, allNodes.size());
    Node<int64_t>* node1 = allNodes[id1];
    Node<int64_t>* node2 = allNodes[id2];

    swap(node1->width, node2->width);
    swap(node1->height, node2->height);
    swap(node1->rotate, node2->rotate);
    swap(node1->name, node2->name);
    swap(node1->is_sg, node2->is_sg);
    swap(node1->symaxis, node2->symaxis);
}
//op4
void move_single_node(Node<int64_t>*& root){
    vector<Node<int64_t>*> allNodes;
    traverse(root, allNodes);

    if(allNodes.size() < 2)return;
    int64_t id = get_random_index(0, allNodes.size());
    Node<int64_t>* node = allNodes[id];
    while(node->lchild != nullptr && node->rchild!=nullptr){
        id = get_random_index(0, allNodes.size());
        node = allNodes[id];
    }
    if(node == root){
        if (node->lchild != nullptr && node->rchild == nullptr) {
            root = node->lchild;
            root->parent = nullptr;
            node->lchild = nullptr;
        } 
        else if (node->rchild != nullptr && node->lchild == nullptr) {
            root = node->rchild;
            root->parent = nullptr;
            node->rchild = nullptr;
        }
    }
    else{
        if(node->lchild != nullptr){
                // cout<<"lchild = "<<node->lchild->name<<endl;
                if(node->parent->lchild == node){
                    node->parent->lchild = node->lchild;
                    node->lchild->parent = node->parent;
                    node->lchild = nullptr;
                    node->parent = nullptr;
                } else if(node->parent->rchild == node){
                    node->parent->rchild = node->lchild;
                    node->lchild->parent = node->parent;
                    node->lchild = nullptr;
                    node->parent = nullptr;
                }
        }else if(node->rchild != nullptr){
            // cout<<"rchild = "<<node->rchild->name<<endl;
            if(node->parent->lchild == node){
                node->parent->lchild = node->rchild;
                node->rchild->parent = node->parent;
                node->rchild = nullptr;
                node->parent = nullptr;
            }else if(node->parent->rchild == node){
                node->parent->rchild = node->rchild;
                node->rchild->parent = node->parent;
                node->rchild = nullptr;
                node->parent = nullptr;
                // cout<<""<<endl;
            }
        }
        else{
            if(node->parent->lchild == node)node->parent->lchild = nullptr;
            if(node->parent->rchild == node)node->parent->rchild = nullptr;
        }
    }
    
    allNodes.resize(0);
    get_rest_nodes(root, allNodes, node);
    int64_t target_id = get_random_index(0, allNodes.size());
    Node<int64_t>* target_node = allNodes[target_id];
    while(target_node->lchild != nullptr || target_node->rchild != nullptr){
        target_id = get_random_index(0, allNodes.size());
        target_node = allNodes[target_id];
    }

    if(target_node->lchild == nullptr && target_node->rchild == nullptr){
        if(get_random_index(0,2) == 0)target_node->lchild = node;
        else target_node->rchild = node;
    } else if(target_node->rchild == nullptr) {
        target_node->rchild = node;
    } else if(target_node->lchild == nullptr) {
        target_node->lchild = node;
    } else {
        cout << "Error: Both children of target node are not null." << endl;
        return;
    }
    node->parent = target_node;
}

Node<int64_t>* cloneNode(Node<int64_t>* node, Node<int64_t>* parent = nullptr) {
    if (!node) return nullptr;
    
    Node<int64_t>* newNode = new Node<int64_t>();

    newNode->x = node->x;
    newNode->y = node->y;
    newNode->width = node->width;
    newNode->height = node->height;
    newNode->rotate = node->rotate;
    newNode->name = node->name;
    newNode->is_sg = node->is_sg;
    newNode->symaxis = node->symaxis;
    newNode->parent = parent;
    
    newNode->lchild = cloneNode(node->lchild, newNode);
    newNode->rchild = cloneNode(node->rchild, newNode);
    
    return newNode;
}

BStarTree<int64_t> cloneBStarTree(const BStarTree<int64_t>& src) {
    BStarTree<int64_t> dst;
    dst.root = cloneNode(src.root);
    return dst;
}

BStarTree<int64_t> get_neighbour(const BStarTree<int64_t>& original) {
    BStarTree<int64_t> new_tree = cloneBStarTree(original);
    
    int op = get_random_index(0, 4);
    // cout<<"op="<<op<<endl;
    switch(op) {
        case 0: 
            rotate_node(new_tree.root);
            break;
        case 1:
            move_node(new_tree.root);
            break;
        case 2:
            swap_node(new_tree.root);
            break;
        case 3:
            move_single_node(new_tree.root);
            break;
    }

    return new_tree;
}

Node<int64_t>* sg_method0(SymGroup& sg){
    Node<int64_t>* node = new Node<int64_t>();
    node->name = sg.name;
    node->is_sg = true;
    node->method = 0;
    int64_t current_w = 0;
    int64_t total_height=0,current_h = 0;

    for(auto& pair : sg.pairs){
        HardBlock& hb1 = hardblocks[pair.block1];
        HardBlock& hb2 = hardblocks[pair.block2];
        if(hb1.W() < hb1.H())hb1.rotate = !hb1.rotate;
        if(hb1.W() != hb2.W())hb2.rotate = !hb2.rotate;
    }
    for(auto& ss : sg.selfs){
        HardBlock& hb = hardblocks[ss.block];
        if(hb.W() < hb.H())hb.rotate = !hb.rotate;
    }

    sort(sg.pairs.begin(), sg.pairs.end(), [&](const auto& pair1, const auto& pair2) {
        HardBlock& hb1_1 = hardblocks[pair1.block1];
        HardBlock& hb1_2 = hardblocks[pair1.block2];
        HardBlock& hb2_1 = hardblocks[pair2.block1];
        HardBlock& hb2_2 = hardblocks[pair2.block2];

        int64_t width1 = hb1_1.W() + hb1_2.W();
        int64_t width2 = hb2_1.W() + hb2_2.W();

        return width1 > width2;
    });

    sort(sg.selfs.begin(), sg.selfs.end(), [&](const auto& self1, const auto& self2) {
        HardBlock& hb1 = hardblocks[self1.block];
        HardBlock& hb2 = hardblocks[self2.block];

        int64_t width1 = hb1.W();
        int64_t width2 = hb2.W();

        return width1 > width2;
    });

    int64_t max_width = 0;

    // If pairs is not empty, use the width of the largest pair
    if (!sg.pairs.empty()) {
        HardBlock& hb1_1 = hardblocks[sg.pairs[0].block1];
        HardBlock& hb1_2 = hardblocks[sg.pairs[0].block2];
        max_width = hb1_1.W() + hb1_2.W();
    }

    // If selfs is not empty and has larger width, update max_width
    if (!sg.selfs.empty()) {
        HardBlock& hb = hardblocks[sg.selfs[0].block];
        max_width = max(max_width, hb.W());
    }
    // cout<<"max_width="<<max_width<<endl;
    for(auto& pair : sg.pairs){
        HardBlock& hb1 = hardblocks[pair.block1];
        HardBlock& hb2 = hardblocks[pair.block2];
        // cout<<" w="<<hb1.W();
        // cout<<" h="<<hb1.H();
        // cout<<" total h ="<<total_height<<endl;
        current_h = max(current_h,hb1.H());
        if(current_w + hb1.W() + hb2.W() > max_width){
            current_w = hb1.W() + hb2.W();
            current_h = hb1.H(); 
            total_height += current_h;
        }else{
            current_w += hb1.W() + hb2.W();
            current_h = max(current_h,hb1.H());
            total_height = max(total_height,current_h);
        }
        // total_width = max_width;
        // total_height = total_height + hb1.H();
    }
    for(auto& ss : sg.selfs){
        HardBlock& hb = hardblocks[ss.block];
        max_width = max(max_width,hb.W());
        total_height = total_height + hb.H();
    }
    node->setShape(max_width,total_height);
    node->symaxis = max_width/2;
    // cout<<" total area="<<max_width<<" * "<<total_height<<endl;
    return node;
}

Node<int64_t>* sg_method1(SymGroup& sg){
    Node<int64_t>* node = new Node<int64_t>();
    node->name = sg.name;
    node->is_sg = true;
    node->method = 1;

    for(auto& pair : sg.pairs){
        HardBlock& hb1 = hardblocks[pair.block1];
        HardBlock& hb2 = hardblocks[pair.block2];
        if(hb1.W() > hb1.H())hb1.rotate = !hb1.rotate;
        if(hb1.H() != hb2.H())hb2.rotate = !hb2.rotate;
    }
    for(auto& ss : sg.selfs){
        HardBlock& hb = hardblocks[ss.block];
        if(hb.W() < hb.H())hb.rotate = !hb.rotate;
    }

    sort(sg.pairs.begin(), sg.pairs.end(), [&](const auto& pair1, const auto& pair2) {
        HardBlock& hb1_1 = hardblocks[pair1.block1];
        HardBlock& hb1_2 = hardblocks[pair1.block2];
        HardBlock& hb2_1 = hardblocks[pair2.block1];
        HardBlock& hb2_2 = hardblocks[pair2.block2];

        int64_t width1 = hb1_1.W() + hb1_2.W();
        int64_t width2 = hb2_1.W() + hb2_2.W();

        return width1 > width2;
    });

    sort(sg.selfs.begin(), sg.selfs.end(), [&](const auto& self1, const auto& self2) {
        HardBlock& hb1 = hardblocks[self1.block];
        HardBlock& hb2 = hardblocks[self2.block];

        int64_t width1 = hb1.W();
        int64_t width2 = hb2.W();

        return width1 > width2;
    });

    int64_t max_width = 0;

    // If pairs is not empty, use the width of the largest pair
    if (!sg.pairs.empty()) {
        HardBlock& hb1_1 = hardblocks[sg.pairs[0].block1];
        HardBlock& hb1_2 = hardblocks[sg.pairs[0].block2];
        max_width = hb1_1.W() + hb1_2.W();
    }

    // If selfs is not empty and has larger width, update max_width
    if (!sg.selfs.empty()) {
        HardBlock& hb = hardblocks[sg.selfs[0].block];
        max_width = max(max_width, hb.W());
    }

    int64_t total_height=0;

    for(auto& pair : sg.pairs){
        HardBlock& hb1 = hardblocks[pair.block1];
        // HardBlock& hb2 = hardblocks[pair.block2];
        total_height += hb1.H();
    }

    for(auto& ss : sg.selfs){
        HardBlock& hb = hardblocks[ss.block];
        total_height += hb.H();
    }

    node->setShape(max_width,total_height);
    node->symaxis = max_width/2;

    return node;
}

void construct_all_nodes(){  
    //Create symgroup to a node
    // vector<Node<int64_t> *> sgnodes;
    nodes.clear();

    for(auto& sg : symgroups){
        Node<int64_t>* node0 = sg_method0(sg);
        Node<int64_t>* node1 = sg_method1(sg);
        // cout<<"node0="<<node0->width * node0->height<<" node1="<<node1->width*node1->height<<endl;
        if(node0->width * node0->height > node1->width*node1->height){
            node1 = sg_method1(sg);
            nodes.emplace_back(node1);
        }
        else{
            node0 = sg_method0(sg);
            nodes.emplace_back(node0);
        }
    }

    for(auto& n : freenodes){
        Node<int64_t>* node = new Node<int64_t>();
        node->name = n;
        HardBlock& hb = hardblocks[n];
        node->setShape(hb.W(),hb.H());
        nodes.emplace_back(node);
    }
}

struct CompareNode {
    bool operator()(Node<int64_t>* a, Node<int64_t>* b) {
        return (a->width * a->height) > (b->width * b->height);
    }
};

Node<int64_t>* buildPriorityTree(vector<Node<int64_t>*>& nodes,int64_t idx) {
    if (nodes.empty() || idx >= static_cast<int64_t>(nodes.size()) ) return nullptr;
    
    if (idx == 0) {
        sort(nodes.begin(), nodes.end(), CompareNode());
    }
    
    Node<int64_t>* root = nodes[idx];
    // vector<Node<int64_t>*> left, right;
    
    // for (int64_t i = 1; i < nodes.size(); i++) {
    //     if (i % 2 == 1) left.push_back(nodes[i]);
    //     else right.push_back(nodes[i]);
    // }
    // if(root->is_sg)root->setRotate();
    root->lchild = nullptr;//buildPriorityTree(left);
    root->rchild = buildPriorityTree(nodes,idx+1);
    return root;
}

// Node<int64_t>* buildTree(int64_t start,int64_t end){
//     if(start>end)return nullptr;

//     int64_t mid = start + (end-start)/2;
//     cout<<"start="<<start<<" end="<<end<<" mid="<<mid<<endl;
//     Node<int64_t>* root = nodes[mid];

//     root->lchild = buildTree(start,mid-1);
//     root->rchild = buildTree(mid+1,end);

//     return root;
// }

vector<Node<int64_t>*> inorderTraversal(Node<int64_t>* root){
    vector<Node<int64_t>*> result;
    if(!root)return result;

    vector<Node<int64_t>*> left = inorderTraversal(root->lchild);
    result.insert(result.end(),left.begin(),left.end());
    result.push_back(root);
    vector<Node<int64_t>*> right = inorderTraversal(root->rchild);
    result.insert(result.end(),right.begin(),right.end());

    return result;
}

vector<Node<int64_t>*> preorderTraversal(Node<int64_t>* root){
    vector<Node<int64_t>*> result;
    if(root == nullptr)return result;

    result.push_back(root);
    vector<Node<int64_t>*> left = preorderTraversal(root->lchild);
    result.insert(result.end(),left.begin(),left.end());
    vector<Node<int64_t>*> right = preorderTraversal(root->rchild);
    result.insert(result.end(),right.begin(),right.end());

    return result;
}

pair<vector<Node<int64_t>*>,vector<Node<int64_t>*>> initialSolution(){
    // Node<int64_t>* root = buildTree(0,nodes.size()-1);
    Node<int64_t>* root = buildPriorityTree(nodes,0);
    vector<Node<int64_t>*> inorder = inorderTraversal(root);
    vector<Node<int64_t>*> preorder = preorderTraversal(root);
    return make_pair(preorder,inorder);
}

void set_coord(Node<int64_t>* node){
    if (!node) return;
    if(node->is_sg) {
        for(auto& sg : symgroups){
            int64_t currentY= node->y;
            int64_t currentX= node->x;
            int64_t cur_w=0,cur_h=0;
            if(sg.name == node->name){
                if(node->method == 0){
                    if(node->rotate){
                        for(auto& pair : sg.pairs){
                            HardBlock& hb1 = hardblocks[pair.block1];
                            HardBlock& hb2 = hardblocks[pair.block2];
                            hb1.rotate = !hb1.rotate;hb2.rotate = !hb2.rotate;
                            if(cur_h + hb1.H() + hb2.H() > node->height){
                                currentX += cur_w;
                                hb1.x = currentX;
                                hb1.y = node->y + node->symaxis - hb1.H();
                                hb2.x = currentX;
                                hb2.y = node->y + node->symaxis;
                                cur_h = hb1.H() + hb2.H();
                                cur_w = hb1.W();
                                // currentY += hb1.H();
        
                            }else{
                                hb1.x = currentX;
                                hb2.x = currentX;
                                hb1.y = node->y + node->symaxis - cur_h/2 - hb1.H();
                                hb2.y = node->y + node->symaxis + cur_h/2;
                                cur_h += hb1.H() + hb2.H();
                                cur_w = max(cur_w,hb1.W());
                                // currentY = cur_h;
                            }
                        }
                        currentX+=cur_w;
                        for(auto& ss : sg.selfs){
                            HardBlock& hb = hardblocks[ss.block];
                            hb.rotate = !hb.rotate;
                            hb.x = currentX;
                            hb.y = node->y + node->symaxis - hb.H()/2;
                            currentX += hb.W();
                        }
                    }
                    else{
                        for(auto& pair : sg.pairs){
                            HardBlock& hb1 = hardblocks[pair.block1];
                            HardBlock& hb2 = hardblocks[pair.block2];
                            if(cur_w + hb1.W() + hb2.W() > node->width){
                                currentY += cur_h;
                                hb1.x = node->x + node->symaxis - hb1.W();
                                hb1.y = currentY;
                                hb2.x = node->x + node->symaxis;
                                hb2.y = currentY;
                                cur_w = hb1.W() + hb2.W();
                                cur_h = hb1.H();
                                // currentY += hb1.H();
        
                            }else{
                                hb1.x = node->x + node->symaxis - cur_w/2 - hb1.W();
                                hb2.x = node->x + node->symaxis + cur_w/2;
                                hb1.y = currentY;
                                hb2.y = currentY;
                                cur_w += hb1.W() + hb2.W();
                                cur_h = max(cur_h,hb1.H());
                                // currentY = cur_h;
                            }
                        }
                        currentY+=cur_h;
                        for(auto& ss : sg.selfs){
                            HardBlock& hb = hardblocks[ss.block];
                            hb.x = node->x + node->symaxis - hb.W()/2;
                            hb.y = currentY;
                            currentY += hb.H();
                        }
                    }
                }else if (node->method == 1){
                    if(node->rotate){
                        for(auto& pair : sg.pairs){
                            HardBlock& hb1 = hardblocks[pair.block1];
                            HardBlock& hb2 = hardblocks[pair.block2];
                            hb1.rotate = !hb1.rotate;hb2.rotate = !hb2.rotate;
                            hb1.x = currentX;
                            hb1.y = currentY + node->symaxis - hb1.H();
                            hb2.x = currentX;
                            hb2.y = currentY  + node->symaxis;
                            currentX += hb1.W();
                        }
                        for(auto& ss : sg.selfs){
                            HardBlock& hb = hardblocks[ss.block];
                            hb.rotate = !hb.rotate;
                            hb.x = currentX;
                            hb.y = currentY + node->symaxis - hb.H()/2;
                            currentX += hb.W();
                        }
                    }
                    else{
                        for(auto& pair : sg.pairs){
                            HardBlock& hb1 = hardblocks[pair.block1];
                            HardBlock& hb2 = hardblocks[pair.block2];
                            hb1.x = currentX + node->symaxis - hb1.W();
                            hb1.y = currentY;
                            hb2.x = currentX + node->symaxis;
                            hb2.y = currentY;
                            currentY += hb1.H();
                        }
                        for(auto& ss : sg.selfs){
                            HardBlock& hb = hardblocks[ss.block];
                            hb.x = currentX + node->symaxis - hb.W()/2;
                            hb.y = currentY;
                            currentY += hb.H();
                        }
                    }
                }
                
            }
        }
    } else {
        HardBlock& hb = hardblocks[node->name];
        hb.x = node->x;
        hb.y = node->y;
        hb.rotate = node->rotate;
    }

    set_coord(node->lchild);
    set_coord(node->rchild);
}

void OutputFile(const string &Outfile, int64_t area){
    ofstream fo(Outfile);
    if (!fo) {
        cerr << "Unable to open output file: " << Outfile << endl;
        exit(1);
    }
    fo << "Area "<<area<<endl;
    fo<< "NumHardBlocks "<<hardblocks.size()<<endl;
    for(auto& [name,hb] : hardblocks){
        fo<<name <<" "<<hb.x<<" "<<hb.y<<" "<<hb.rotate<<endl;
    }

    fo.close();
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
    cout<<"Total time used = "<< elapsedTime <<"s"<<endl;
}