// creates and manipulates binomial heaps (only supports insert & delete-min) (as of right now, single threaded supports insert, delete-min and union)
// precompiled requires windows 10+ and 64-bit op system

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

const regex intRegex("^[1-9][0-9]*$"); //  int > 0

class Node{ // object to store the key, the parent pointer and children pointers
    public:
    int key;
    Node* parent;
    vector<Node*> children;

    Node(int k){key = k; parent = nullptr;}

};

// Merge two binomial trees of the same degree
static Node* mergeTrees(Node* tree1, Node* tree2) {
    if (tree1->key > tree2->key) {
        swap(tree1, tree2); // Make sure tree1 has smaller root key
    }
    tree2->parent = tree1; // Make tree1 the parent of tree2
    tree1->children.push_back(tree2); // Add tree2 as a child of tree1
    return tree1;
}

class Heap{ // object to store pointers of tree roots and min.
    public:

    vector<Node*> top;
    Node* minN;

    Heap(){minN = nullptr;}

    // return key of min node
    int getMin(){return minN->key;}

    // insert node with key = val
    void insert(int val){
        /* insert x:
            H' <- makeHeap(x)
            H <- untion(H',H)
        */
        Node* tbi = new Node(val);
        Heap H;
        H.top.push_back(tbi);
        uni(H);
    }

    // delete node with minimum key
    int deleteMin(){
        /* Find root x with min key in root list and delete
            H' <- broken binomial trees
            H <- union(H',H)
        */
        Node* origMin = minN;
        top.erase(remove(top.begin(), top.end(), minN), top.end()); // shouldn't affect complexity since we still need to scan for findMin(). could fix it by having minN be the index of min instead of the min itself // src: https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
        Heap H;
        H.top = minN->children; // children are compliant with heap constraints.
        uni(H);
        return origMin->key;
    }

    // Generate Graphviz DOT representation of the heap
    string toDOT() {
        stringstream dot;
        dot << "digraph Heap {\n";
        
        // Add nodes and edges recursively
        for (Node* node : top) {
            addNodeDOT(dot, node);
        }
        
        dot << "}\n";
        return dot.str();
    }

    // recursively update the dot string to be visualized by Graphviz
    void addNodeDOT(stringstream& dot, Node* node) {
        dot << "  " << node->key << " [label=\"" << node->key << "\"];\n";
        for (Node* child : node->children) {
            dot << "  " << node->key << " -> " << child->key << ";\n";
            addNodeDOT(dot, child); // Recursively add children
        }
    }

    private:

    // check if heap is empty
    bool isEmpty(){
        return minN == nullptr;
    }

    // union between current heap and H
    void uni(Heap& H){
        vector<Node*> newTop;
        // fix the trees so conform with binomial heap restrictions
        auto it1 = top.begin();
        auto it2 = H.top.begin();
        Node* carry = nullptr;
        while (it1 != top.end() && it2 != H.top.end()){
            Node* node1 = *it1;
            Node* node2 = *it2;
            int deg1 = node1->children.size();
            int deg2 = node2->children.size();
            if (carry != nullptr){ // carry on exists
                int degCarry = carry->children.size();
                bool d1s = deg1 < deg2;
                if (d1s && degCarry < deg1){ // drop carry on directly to new heap
                    newTop.push_back(carry);
                    carry = nullptr;
                }
                else if (d1s){ // carry has same degree as node1 (new carry on degree will be >= degree node2)
                    carry = mergeTrees(node1,carry);
                    ++it1;
                }
                else if (deg1 == deg2){ // will create a carry on who's degree will be strictly greater than that of the current one
                    newTop.push_back(carry);
                    carry = mergeTrees(node1,node2);
                    ++it1; ++it2;
                }
                else if (degCarry < deg2){ // drop carry on directly to new heap
                    newTop.push_back(carry);
                    carry = nullptr;
                }
                else { // carry has same degree as node2 (new carry on degree will be >= degree node1)
                    carry = mergeTrees(node2,carry);
                    ++it2;
                }
            }
            else if (deg1 < deg2){ // drop tree from first heap to new one
                newTop.push_back(node1);
                ++it1;
            }
            else if (deg1 > deg2){ // drop tree from second heap to new one
                newTop.push_back(node2);
                ++it2;
            }
            else{ // deg1 == deg2, merge and move to carry on
                carry = mergeTrees(node1,node2);
                ++it1; ++it2;
            }
        }

        if(carry != nullptr){ // finaly carry on
            int degc = 0;
            Node* node = nullptr;
            while (it1 != top.end() && carry != nullptr) { // keep getting carries that must be handled until you don't
                node = *it1;
                int deg = node->children.size();
                degc = carry->children.size();
                if (degc == deg){ // only cases are degc == deg or degc < deg
                    carry = mergeTrees(node,carry); // keep merging till you can drop it
                    ++it1;
                }
                else{ 
                    newTop.push_back(carry);
                    carry = nullptr;
                    break;
                }
            }
            while(it2 != H.top.end() && carry != nullptr){
                node = *it2;
                int deg = node->children.size();
                degc = carry->children.size();
                if (degc == deg){ // only cases are degc == deg or degc < deg
                    carry = mergeTrees(node,carry); // keep merging till you can drop it
                    ++it2;
                }
                else{ 
                    newTop.push_back(carry);
                    carry = nullptr;
                    break;
                }
            }
            if (carry != nullptr){
                newTop.push_back(carry);
                carry = nullptr;
            }
        }
        // Append remaining nodes from the second heap, if any
        while (it1 != top.end()) {
            newTop.push_back(*it1);
            ++it1;
        }

        // Append remaining nodes from the second heap, if any
        while (it2 != H.top.end()) {
            newTop.push_back(*it2);
            ++it2;
        }
        top = newTop;
        findMin();
    }

    // recalculate current min
    void findMin(){ 
        minN = nullptr;
        for (Node* root : top){
            if (minN == nullptr || root->key < minN->key){ // use first root then check entire list
                minN = root;
            }
        }
    }


};

vector<string> split(string input, char delimiter = ' '){
    istringstream stream(input);
    string token;
    vector<string> parsed;
    while(getline(stream,token,delimiter)){
        parsed.push_back(token);
    }
    return parsed;
}

int main(int argc, char *argv[]) {
    
    Heap heap;

    // take command line inputs (default is no image output)
    string cmd = "";
    vector<string> parsed;

    cout<<"///////////////////////////////////\nCommands are:\n-i x (insert x, an int)\n-i x y z ... (insert multiple int's)\n-d (delete-min)\n-d x (x delete-min's)\n-img (get a visual of the current heap)\n-end (stop taking inputs)\n*warning* avoid inserting a key that currently exists\n///////////////////////////////////"<<endl;
    while (true){
        cout<<"Input command: ";
        getline(cin,cmd);
        parsed = split(cmd);
        int sz = parsed.size();
        if (sz < 1){
            cout<<"No command given"<<endl;
            continue;
        }

        cmd = parsed[0];
        if (cmd == "i" || cmd == "-i"){ // insert (ex: i 5 3 8 2 1 16 7 12 10 4 9 14 6 11 13 15 17 18 20 19)
            for (int i = 1; i < sz; i++){
                heap.insert(stoi(parsed[i])); // single threaded version
            }
        }
        else if(cmd == "d" || cmd == "-d"){ // delete-min
            if (sz == 1){ // 1 delete-min
                heap.deleteMin();
            }
            else{ // multiple delete min
                int count = stoi(parsed[1]);
                for (int i = 0; i<count; i++){// single threaded version
                    heap.deleteMin();
                }
            }
        }
        else if (cmd == "img" || cmd == "-img"){ // visualization
            // Generate DOT representation
            string dot = heap.toDOT();

            // Output DOT representation to a file
            ofstream file("heap.dot");
            file << dot;
            file.close();

            // Notify the user
            cout << "DOT file generated. Use Graphviz to visualize the heap.\n";

            // Generate visualization image using Graphviz
            system("dot -Tpng heap.dot -o heap.png");

            // Notify the user
            cout << "Visualization image generated as heap.png.\n";
            system("start heap.png"); // Open with default program on Windows
        }
        else if(cmd == "end" || cmd == "-end"){ // end execution
            break;
        }
        else{ // invalid command
            cout<<"invalid command: "<< cmd <<endl;
            continue;
        }
    }

    return 0;
}