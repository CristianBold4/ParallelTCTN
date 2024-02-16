#ifndef TCTN_DEGREEMAP_H
#define TCTN_DEGREEMAP_H

#include <string>
#include <vector>
#include "ankerl/unordered_dense.h"
#include <algorithm>
#include "Edge.h"
#include "Hashing.h"

class DegreeMap {
private:
    ankerl::unordered_dense::map<std::pair<int, int>, int, pair_hashing> degree_map;

public:
    inline void add_pair(const int& u, const int& t) { degree_map[{u, t}] = 0; };
    inline void increment_value(const int& u, const int& t, int val = 1) { degree_map[{u, t}] += val; };
    inline int get_value(const int& u, const int& t) { return degree_map.at({u, t}); }
    inline bool contains(int u, int t) {return degree_map.find(std::make_pair(u, t)) != degree_map.end();}
    inline ankerl::unordered_dense::map<std::pair<int, int>, int, pair_hashing> get_map() { return degree_map; };

    ~DegreeMap() = default;
};


#endif //TCTN_DEGREEMAP_H
