#ifndef GENETIC_HPP
#define GENETIC_HPP

#include "node.hpp"
#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <fstream>

#define DATA16K "../data16k.txt"
#define DATA128 "../data128.txt"
#define CYCLE_FILE_16K "cycles_16k.ndjson"
#define CYCLE_FILE_128 "cycles_128.ndjson"
#define INT_MAX 2147483647
#define POPULATION 20
#define MUTATION_RATE 0.1

using namespace std;

class tsp{
    private:
        vector<Node> nodes;
        vector<vector<Node>> population;
        vector<double> fitness_scores;
        pair<double,int> best_and_index;

    public:
        tsp() = delete;
        tsp(vector<Node> nodes);
        ~tsp();
        double sum_total_fit();
        vector<double> calc_probabilities();
        double calc_dist(const Node &a, const Node &b);
        void randomize();
        void evaluate_fitness();
        void select_and_copulate();
        void mutate();
        void dump_cycle(pair<double,int> &best_solution, ofstream &o);
        pair<double,int> get_best_fitness();
        void genetic_alg_driver();
    
};

#endif