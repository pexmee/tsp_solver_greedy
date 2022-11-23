#include "../headers/genetic.hpp"
using namespace std;
auto def_gen = default_random_engine{};
mt19937 gen(random_device{}());
string IFILE = DATA16K;
string OFILE = CYCLE_FILE_16K;

tsp::tsp(vector<Node> nodes){
    this->nodes = nodes;
    this->fitness_scores = {};
    this->population = {};
}
tsp::~tsp(){};

double tsp::sum_total_fit(){
    return accumulate(this->fitness_scores.begin(), fitness_scores.end(), 0.0);
}

vector<double> tsp::calc_probabilities(){
    double total = this->sum_total_fit();
    vector<double> probabilities;

    for(vector<double>::iterator it = this->fitness_scores.begin(); it != this->fitness_scores.end(); it++){
        probabilities.push_back(*it);
    }
    return probabilities;
}

__always_inline double tsp::calc_dist(const Node &a, const Node &b){
    return sqrt(pow((a.x - b.x),2) + pow((a.y - b.y),2));
}

void tsp::randomize(){
    vector<Node> solution = this->nodes;

    for(size_t i = 0; i < POPULATION; i++){
        shuffle(begin(solution), end(solution), def_gen);
        this->population.push_back(solution);
    }
}

void tsp::evaluate_fitness(){
    double total = 0;
    this->fitness_scores = {};

    for(size_t i = 0; i < this->population.size(); i++){
        total = 0;
        for(size_t j = 0; j < this->population[i].size()-1; j++){
            total += this->calc_dist(this->population[i][j], this->population[i][j+1]);
        }
        total += calc_dist(this->population[i][0],this->population[i][this->population.size()-1]);
        this->fitness_scores.push_back(total);
    }
}

void tsp::select_and_copulate(){
    vector<double> probabilities = this->calc_probabilities();
    discrete_distribution<size_t> d{probabilities.begin(),probabilities.end()};
    vector<Node> p1,p2;
    vector<vector<Node>> new_pop;

    for(size_t i = 0; i < probabilities.size(); i++){
        p1 = this->population[d(gen)];
        p2 = this->population[d(gen)];


        unordered_set<Node> c;
        for(size_t j = 0; j < p1.size()/2; j++){
            c.insert(p1[j]);
        }
        for(size_t j = 0; j < p2.size(); j++){
            c.insert(p2[j]);
        }

        vector<Node> cld;
        for (auto it = c.begin(); it != c.end();) {
            cld.push_back(std::move(c.extract(it++).value()));
        }
        new_pop.push_back(cld);
    }
    this->population = new_pop;
}

void tsp::mutate(){
    int a,b;
    for (size_t i = 0; i < this->population.size(); i++){
        for (size_t j = 0; j < this->population[i].size()*MUTATION_RATE; j++){
            a = rand()%(this->population[i].size());
            b = rand()%(this->population[i].size());
            Node tmpa = this->population[i][a];
            Node tmpb = this->population[i][b];
            this->population[i][a] = tmpa;
            this->population[i][b] = tmpb;
        }
    }
}

__always_inline void to_json(nlohmann::json &j, const Node &node) {
    j = node.serialize();
}

pair<double,int> tsp::get_best_fitness(){
    pair<double,int> best_and_index = {INT_MAX, -1};
    for (size_t i = 0; i < this->fitness_scores.size(); i++){
        if (this->fitness_scores[i] < best_and_index.first){
            best_and_index.first = this->fitness_scores[i];
            best_and_index.second = i;
        }
    }
    
    return best_and_index;
}

void tsp::dump_cycle(pair<double,int> &best_solution, ofstream &o){
    time_t now;
    now = time(nullptr);
    char* dt = ctime(&now);
    cout << dt << " found new best solution: " << fixed << best_solution.first << endl;
    string start_coord = "(" + to_string(this->population[best_solution.second][0].x) + "," + to_string(this->population[best_solution.second][0].y) + ")";
    cout << start_coord << endl;
    nlohmann::json data;
    data["dist"] = best_solution.first;
    data["start_coord"] = start_coord;
    data["timestamp"] = dt;
    data["zcycle"] = this->population[best_solution.second]; // zcycle to order it to the back of the ndjson
    o << data << endl;
    
}
void tsp::genetic_alg_driver(){
    ofstream o;
    o.open(OFILE, ios_base::app);
    pair<double,int> current_best = {INT_MAX,-1};
    this->randomize();
    unsigned long long cnt = 0;
    while (true){
        cnt++;
        if(cnt % 5000 == 0){
            time_t now;
            now = time(nullptr);
            char* dt = ctime(&now);
            cout << "still finding next generation.." << dt << endl;
        }
        this->evaluate_fitness();
        this->select_and_copulate();
        this->mutate();
        pair<double,int> best = this->get_best_fitness();
        if(best.first < current_best.first){
            current_best = best;
            dump_cycle(current_best,o);
        }   
        

    }
}