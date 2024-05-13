// creates and manipulates binomial heaps (only supports insert & delete-min)
// precompiled requires windows 10+ and 64-bit op system

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>

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

    // create DOT file to be visualized by Graphviz
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
        /*
        for each i = 0,...,floor(log n) + 1 pardo // Phase I: Linking Chains
            compute values g_i, p_i, and c_i // a prefix computation is required to compute c_i starting from g_i, p_i
        end for

        for i = 0,...,floor(log n) + 1 pardo // Phase II: Prefix Minima in Linking Chains
            I_lim[i] := !(p_i & c_i-1) // assume c_-1 = 0
            if g_i = 1 then
                if H1[i]↑.key < H2[i]↑.key then I_value := H1[i]
                else I_value := H2[i]
            else I_value := ChooseNotNULL(H1[i],H2[i])
        end for

        I_value := Segmented-Prefix-Min(I_lim,I_value)

        for i = 0,...,floor(log n) + 1 pardo // Phase III: Parallel Linking of Binomial Trees
            if g_i = 1 then Combine(H1[i],H2[i])
            else if (p_i = 1 and c_i-1 = 1) then // internal or ending point
                if I_value[i] = I_value[i-1] then r_i := ChooseNotNull(H1[i],H2[i]);
                                                        Combine(I_value[i],r_i)
                else Combine(I_value[i],I_value[i-1])
        end for

        for i = 0,...,floor(log n) + 1 pardo // Generating the final heap H
            H[i] := nil
            if g_i = 1 & p_i+1 = 0 then H[i+1] := I_value[i]; // independant point of first subtype
                                        I_value[i]↑.degree := i+1
            else if (p_i = 1 & c_i-1 = 0) then H[i] := I_value[i] // independant point of second subtype
            else if (p_i = 1 & c_i-1 = 1 & p_i+1 = 0) then H[i+1] := I_value[i]; // ending point
                                                           I_value[i]↑.degree := i+1
        end for

        return H
        */


        // Phase I: Linking Chains
        int n = max(top.size(),H.top.size() + 1);
        int i_lim = floor(log2(n));
        vector<int> g(n, 0); // root list of the merged heap
        vector<int> p(n, 0); // parity bits
        vector<int> c(n, 0); // carry bits

        for (int i = 0; i <= i_lim; ++i) {
            // Compute values g_i, p_i, and c_i
            int a_i = (i < top.size() && top[i] != nullptr) ? 1 : 0;
            int b_i = (i < H.top.size() && H.top[i] != nullptr) ? 1 : 0;
            g[i] = a_i & b_i; // Carry generation
            p[i] = a_i ^ b_i; // Carry propagation
            c[i + 1] = floor((a_i + b_i + c[i]) / 2); // Calculate the carry bit for the next position
        }  

        // Phase II: Prefix Minima in Linking Chains
        vector<int> I_lim(n,0); // auxiliary boolean array
        vector<Node*> I_val(n,0);

        for (int i = 0; i <= i_lim; ++i) {
            I_lim[i] = (i==0)? !(p[i] & 0) : !(p[i] & c[i-1]); // assume c[-1] = 0
            if (g[i] == 1){
                I_val[i] = (top[i]->key < H.top[i]->key) ? top[i] : H.top[i];
            }
            else {
                I_val[i] = (top[i]) ? top[i] : H.top[i]; // Choose Not Null
            }
        }

        // I_value := Segmented-Prefix-Min(I_lim,I_value)
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

// addition algorithm described in the paper
vector<int> addBinary(const vector<int>& a, const vector<int>& b) {
    int n = max(a.size(), b.size());
    vector<int> sum(n + 1, 0); // Initialize sum vector with one extra element for carry
    int carry = 0;

    for (int i = 0; i < n; ++i) {
        int ai = (i < a.size()) ? a[i] : 0;
        int bi = (i < b.size()) ? b[i] : 0;
        
        int temp = ai + bi + carry;
        sum[i] = temp % 2; // Calculate sum bit
        carry = temp / 2;  // Calculate carry bit
    }
    
    sum[n] = carry; // Set the most significant bit to the final carry

    return sum;
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
        // 2000 randomized numbers: i 
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