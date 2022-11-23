#include "../headers/traverser.hpp"

__always_inline double dist_calc(Node &current_node, Node &neighbor){
    return std::sqrt(std::pow((current_node.x - neighbor.x),2) + std::pow((current_node.y - neighbor.y),2));
}

size_t NUM_THREADS = 0;
double CURRENT_BEST = INT_MAX;


/**
 * Rewrite this to use a genetic algorithm with the following approach:
 *
 *   Initialization: The initial solutions(first generations) are randomly initialized
 *   Fitness Score: Here, the main idea is, it may be hard to find the optimal solution but once we have some solution, it's easy to attach a goodness or fitness score to it.
 *   Selection: The fittest member (solution) of the population survives and moves on to the next generation.
 *   Crossover: The fittest member(solutions) of the population combines in pairs to create new members (a possible solution).
 *   Mutation: New members (Possible new solutions) change themselves in a small amount to get even better.
 * 
 */


 /**
  * 1. Initialization: Initialize by randomizing a few solutions - just pick a node at random until a cycle has been built. 
  * 2. Fitness score: The fitness score would be based upon the length traversed to build the cycle.
  * 3. Selection: Selection would be done by picking the top solutions, a multiple of 2 to make sure we can make children from them.
  * 4. Crossover: Combine the fittest solutions by  
  */


Traverser::Traverser(){
    this->nodes = {};
    this->parent_population = {};
    this->new_generation = {};
    this->cycle = {};
    this->possible_starting_nodes = {};
    this->fitness_scores = {};
}

Traverser::~Traverser(){   
    // do absolutely nothing
}


std::vector<double> calc_probabilities(std::vector<double> &fitness_scores){
    double total_fitness = 0;
    std::vector<double> probabilities = {};

    for(size_t i = 0; i < fitness_scores.size(); i++){
        total_fitness += fitness_scores[i];
    }
    for(size_t i = 0; i < fitness_scores.size(); i++){
        probabilities.push_back(fitness_scores[i] / total_fitness);
        std::cout << "fitness score: " << fitness_scores[i] / total_fitness << "\n";
    }
    return probabilities;
}

std::vector<std::vector<std::vector<Node>>> Traverser::select_parents(std::vector<std::vector<Node>> &population){
    std::mt19937 gen(std::random_device{}());
    std::vector<double> probabilities = calc_probabilities(this->fitness_scores);
    std::discrete_distribution<std::size_t> d{probabilities.begin(), probabilities.end()};
    std::vector<std::vector<Node>> parents_a, parents_b;
    for(size_t i = 0; i < probabilities.size(); i++){
        parents_a.push_back(population[d(gen)]);
        parents_b.push_back(population[d(gen)]);
    }
    std::vector<std::vector<std::vector<Node>>> ret = {parents_a, parents_b};

    return ret;
}

std::vector<Node> make_child(std::vector<Node> &parent_a, std::vector<Node> &parent_b){
    std::vector<Node> child;
    // std::copy(parent_a.begin(), parent_a.begin() + int(parent_a.size()/3),chi    ld.begin());
    for(size_t i = 0; i < parent_a.size()/3; i++){
        child.push_back(parent_a[i]);
    }

    for (size_t i = 0; i < parent_b.size(); i++){
        // If the node isn't found in the child
        if(std::find(child.begin(), child.end(),parent_b[i]) == child.end()){
            child.push_back(parent_b[i]);
        }
    }
    return child;
}



void Traverser::mutate(){
    for (size_t i = 0; i < this->new_generation.size(); i++){
        for (size_t k = 0; k < int(MUTATION_RATE * this->nodes.size()); k++){
            int a = rand()%(this->nodes.size()-1);
            int b = rand()%(this->nodes.size()-1);
            Node tmpa = this->new_generation[i][a], tmpb = this->new_generation[i][b];
            this->new_generation[i][a] = tmpb;
            this->new_generation[i][b] = tmpa; 
        }
    }
}

void Traverser::copulate(std::vector<std::vector<std::vector<Node>>> &parents){
    std::vector<std::vector<Node>> temp_new_generation;

    for (size_t i = 0; i < parents[0].size(); i++){ // Since they are both the same size anyway.
        std::vector<Node> child = make_child(parents[0][i], parents[1][i]);
        temp_new_generation.push_back(child);
    }
    this->new_generation = temp_new_generation;
}


std::pair<int,double> find_best_index_and_fitness(std::vector<double> &fitness_scores){
    std::pair<int,double> best_fitness = std::pair<int,double>(-1,INT_MAX);
    for (size_t i = 0; i < fitness_scores.size(); i++){
        if(fitness_scores[i] < best_fitness.second){
            best_fitness.second = fitness_scores[i];
            best_fitness.first = i;
        }
    }
    return best_fitness;
}
char* get_timestamp(){
    time_t now = time(0);
    char *dt = ctime(&now);
    return dt; 
}
void Traverser::genetic_algorithm_driver(){
    int n = 0;
    this->initialize_nodes();
    this->randomize_parents();
    this->calculate_all_fitness(this->parent_population);
    std::vector<std::vector<std::vector<Node>>> parents = this->select_parents(this->parent_population);
    this->copulate(parents);
    this->mutate();
    std::pair<int,double> best = find_best_index_and_fitness(this->fitness_scores);
    this->dump_cycle(best.second, this->new_generation[best.first]);
    
    while(true){
        n++;
        if (n%100000 == 0){
            n = 0;
            std::cout << "we are alive and well.. " << get_timestamp() << std::endl;
        }
        this->calculate_all_fitness(this->new_generation);   
        parents = this->select_parents(this->new_generation);
        this->copulate(parents);
        this->mutate();
        std::pair<int,double> best = find_best_index_and_fitness(this->fitness_scores);
        if(best.second < CURRENT_BEST){
            
            CURRENT_BEST = best.second;
            std::cout << "Found new shorter cycle " << CURRENT_BEST << " " << get_timestamp() << "\n";
            this->dump_cycle(best.second, this->new_generation[best.first]);
        }
    }
}


void Traverser::calculate_all_fitness(std::vector<std::vector<Node>> &population){
    this->fitness_scores.clear();
    double dist = 0;

    for (size_t i = 0; i < population.size(); i++){
        dist = 0;

        for(size_t k = 0; k < population[i].size()-1; k++){ 
            dist += dist_calc(population[i][k],population[i][k+1]);
        }
        // To add the fitness for the last node which is supposed to connect to the first node
        dist += dist_calc(population[i][0],population[i][population[i].size()-1]);
        this->fitness_scores.push_back(dist);
    }
    
}

void Traverser::randomize_parents(){
    int index, cnt = 0;

    while (cnt < POPULATION_SIZE){
        cnt++;
        while (this->nodes.size() > 1){
            index = rand()%(this->nodes.size());
            this->cycle.push_back(this->nodes[index]);
            this->nodes.erase(this->nodes.begin() + index);
        }
        this->parent_population.push_back(this->cycle);
        this->initialize_nodes();
    }
    std::cout << parent_population.size() << std::endl;
}

void Traverser::initialize_nodes(){
    // We do this in case we are traversing from another starting node.
    this->nodes.clear();
    this->cycle.clear();
    this->get_data();
}

void Traverser::get_data(){
    std::ifstream data_file(DATA16K);
    std::string line;

    while(getline(data_file,line)){
        int x, y;
        std::stringstream ss(line);
        ss >> x;
        ss >> y;
        Node node(x,y);
        if(std::find(this->nodes.begin(),this->nodes.end(), node) == this->nodes.end()){
            // File contained duplicates, so we remove them.
            this->nodes.push_back(node);
        }
    }
}

// For serializing the cycle to json object
__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

void Traverser::dump_cycle(double fitness, std::vector<Node> &cycle){
    std::string start_coord = "(" + std::to_string(cycle[0].x) + "," + std::to_string(cycle[0].y) + ")";
    nlohmann::json data;
    data["dist"] = fitness;
    data["start_coord"] = start_coord;
    data["zcycle"] = cycle; // zcycle to order it to the back of the ndjson
    
    std::ofstream o, r;
    o.open(CYCLE_FILE_16K, std::ios_base::app);
    o << data << std::endl;
    o.close();
}