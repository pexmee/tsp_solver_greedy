#include "../headers/node.hpp"
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <stack>
#include <nlohmann/json.hpp>
#include <assert.h>

#define INFILE "../datasets/data128.txt"
#define OUTFILE "../cycles/cycle128.ndjson"

using namespace std;

__always_inline double dst(const Node &a, const Node &b){
    return sqrt(pow((a.x - b.x),2) + pow((a.y - b.y),2));
}

Node closest_node(const Node &current, vector<Node> &pop, const vector<Node> &solution){
    pair<double,Node> closest = {INT32_MAX, Node()};
    stack<Node,vector<Node>> ns(pop);
    
    double cdist;
    int index_to_closest = 0;
    while (ns.size() > 0){
        cdist = dst(current,ns.top());
        
        if (
            (ns.top() != current) && 
            (cdist < closest.first) && 
            (find(solution.begin(), solution.end(), ns.top()) == solution.end())
           ){
            index_to_closest = ns.size()-1;
            closest.first = cdist;
            closest.second = ns.top();
            ns.pop();
        }
        else{
            ns.pop();
        }
    }

    pop.erase(pop.begin() + index_to_closest);
    return closest.second;
}

double total_score(const vector<Node> &solution){
    double total = 0;

    for (size_t i = 0; i < solution.size()-1; i++){
        total += dst(solution[i],solution[i+1]);
    }
    total += dst(solution[0],solution[solution.size()-1]); // Because the last node will always have to connect back to the first.
    return total;
}

// For serializing the cycle to json object
__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

char* get_timestamp(){
    time_t now;
    now = time(nullptr);
    char* stamp = ctime(&now);
    return stamp;
}

nlohmann::json wrap_data(const vector<Node> &solution, const double best_score, const char* stamp){
    nlohmann::json data;
    data["dist"] = best_score;
    data["start_node"] = "(" + to_string(solution[0].x) + "," + to_string(solution[0].y) + ")";
    data["timestamp"] = stamp;
    data["zcycle"] = solution;
    return data;
}

void dump_data(vector<Node> &solution, const double best_score){
    ofstream o;
    o.open(OUTFILE);
    if(!o){
        cout << "could not open OUTFILE" << endl;
        exit(EXIT_FAILURE);
    }
    char* stamp = get_timestamp();
    cout << stamp << "best solution is: " << fixed << best_score << endl;
    nlohmann::json data = wrap_data(solution,best_score,stamp);
    o << data << endl; 
}

Node reset(vector<Node> &pop, vector<Node> &nodes, vector<Node> &solution, size_t &i){
    pop = nodes; 
    solution.clear(); 
    Node current = nodes[i]; 
    pop.erase(pop.begin() + i); 
    solution.push_back(current);
    return current;
}

int main(){
    ifstream data_file(INFILE);
    string line;
    vector<Node> nodes, solution, pop;

    if(!data_file){
        cout << "could not open INFILE" << endl;
        return EXIT_FAILURE;
    }

    while (getline(data_file,line)){
        int x, y;
        stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if (find(nodes.begin(), nodes.end(), node) == nodes.end()){
            nodes.push_back(node); // Only append unique nodes, ignore dupliates
        }
    }

    double best_score = INT32_MAX, current_score = 0;
    
    for (size_t i = 0; i < nodes.size(); i++){
        Node current = reset(pop,nodes,solution,i); 

        for (size_t j = 0; j < nodes.size()-1; j++){ // -1 since we removed the starting node
            current = closest_node(current,pop,solution);
            solution.push_back(current);
        }

        assert(solution.size() == nodes.size());
        current_score = total_score(solution);

        if (current_score < best_score){
            best_score = current_score;
            cout << "best score updated to: " << best_score << endl; 
        }
        
    }
    dump_data(solution,best_score);
    return EXIT_SUCCESS;
}
