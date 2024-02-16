#ifndef TCTN_SUBGRAPH_H
#define TCTN_SUBGRAPH_H

#include <string>
#include <vector>
#include "ankerl/unordered_dense.h"
#include <algorithm>

#include "Edge.h"

struct neighbor {
    int id;
    int time;
    bool direction;
    bool heavy = false;
    bool operator==(const neighbor& n) const { return (this->id == n.id &&
                                                       this->time == n.time &&
                                                       this->direction == n.direction); }
};

class Subgraph {

private:
    ankerl::unordered_dense::map<int, std::vector<neighbor>> subgraph;
    int size = 0;
public:
    void add_edge(const Edge& e, bool heavy = false);
    void subgraph_prune(int curr_time, int delta);
    std::vector<neighbor> get_neighborhood(int node) const { return subgraph.at(node); }
    bool contains_node(int node) const { return subgraph.find(node) != subgraph.end(); }
    int get_size() const { return size; }
    void remove_edge(const Edge& e);
    ~Subgraph() = default;
};


#endif //TCTN_SUBGRAPH_H
