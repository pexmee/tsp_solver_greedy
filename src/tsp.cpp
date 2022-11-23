#include "../headers/genetic.hpp"

int main(){
    ifstream data_file(DATA16K);
    string line;
    vector<Node> nodes;

    while(getline(data_file,line)){
        int x, y;
        stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if (find(nodes.begin(), nodes.end(), node) == nodes.end()){ // remove duplicates.
            nodes.push_back(node);
        }
    }
    tsp searcher(nodes);
    searcher.genetic_alg_driver();

    return EXIT_SUCCESS;
}