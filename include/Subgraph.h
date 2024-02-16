//#ifndef PARALLELTCTN_SUBGRAPH_H
//#define PARALLELTCTN_SUBGRAPH_H
//
//#include <vector>
//#include <algorithm>
//#include "ankerl/unordered_dense.h"
//#include <iostream>
//
////struct EdgeTemp {
////    int u;
////    int v;
////    int time;
////    bool operator==(const EdgeTemp& e) const {
////        return ((this->u == e.u) && (this->v == e.v) && (this->time == e.time));
////    }
////};
//
////std::ostream& operator<<(std::ostream &os, const EdgeTemp &e) {
////    os << e.u << " " << e.v << " " << e.time;
////    return os;
////}
//
//typedef struct NeighTemp {
//    int node;
//    int timestamp;
//    bool weight;
//    int dir;
//} NeighTemp;
//
//#define MAX_N_NODES 500000000
//
////struct hash_edge {
////
////    size_t operator()(const EdgeTemp &et) const {
////
////        int nu = (et.u < et.v ? et.u : et.v);
////        int nv = (et.u < et.v ? et.v : et.u);
////        unsigned long long id = (static_cast<unsigned long long>(MAX_N_NODES)
////                                * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv))
////                                        + et.time;
////        return id;
////    }
////
////};
//
//class Subgraph {
//
//public:
//
//    bool add_edge(const EdgeTemp &e, bool heaviness);
//
//    void prune(int curr_time, int delta);
//
//    void clear();
//
//    std::vector<NeighTemp> * return_neighbors(const int u);
//
//    void return_edges(std::vector<EdgeTemp> &subgraph_edges) const;
//
//    void return_nodes(std::vector<int> &subgraph_nodes) const;
//
//    int get_degree_node(const int u);
//
//    int num_edges() const;
//
//    int num_nodes() const;
//
//    Subgraph();
//
//    ~Subgraph();
//
//private:
//
//    // -- subgraph of edges
//    ankerl::unordered_dense::map<int, std::vector<NeighTemp>> subgraph_;
//
//    // -- nodes degree
//    ankerl::unordered_dense::map<int, int> nodes_degree_;
//
//    int num_edges_;
//
//};
//
//#endif
