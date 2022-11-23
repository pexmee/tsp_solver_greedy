#ifndef NODE_HPP
#define NODE_HPP
#include <string.h>
#include <iostream>
#include <algorithm>

struct Node{
        int x;
        int y;
        
        // Constructors
        Node(){this->x = this->y = -1;}; 
        ~Node(){};
        Node(int x, int y){this->x = x; this->y = y;};
        Node(const Node &other){
            this->x = other.x;
            this->y = other.y;
        }
        Node& operator=(const Node &other){
            this->x = other.x;
            this->y = other.y;
            return *this;
        }

        // Operators
        bool operator==(const Node &other) const{
        return (this->x == other.x && this->y == other.y);
        }
        bool operator!=(const Node &other) const {
            return (this->x != other.x || this->y != other.y);
        }

        // To help serialize to json.
        std::pair<int,int> serialize() const{
            return std::pair<int,int>(this->x, this->y);
        }

};

// To make it hashable.
template <>
struct std::hash<Node>{
    int x;
    int y;

    std::size_t operator()(const Node& node) const{
        using std::size_t;
        using std::hash;

        return ((std::hash<int>()(node.x) ^ (std::hash<int>()(node.y) << 1)) >> 1);
    }
};

#endif