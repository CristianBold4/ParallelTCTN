#ifndef TRIANGLESWITHPREDICTIONS_UTILS_H
#define TRIANGLESWITHPREDICTIONS_UTILS_H

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include "WRPSampling.h"
#include "GraphStream.h"
#include <string>
#include <sstream>


struct hash_edge {

    template<class T1, class T2>
    size_t operator()(const std::pair<T1, T2> &p) const {

        int nu = (p.first < p.second ? p.first : p.second);
        int nv = (p.first < p.second ? p.second : p.first);
        unsigned long long id = static_cast<unsigned long long>(MAX_N_NODES)
        * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv);
        return id;
    }

};

class Utils {

public:

    static void read_oracle(std::string& oracle_filename, const char delimiter, int skip,
                             ankerl::unordered_dense::map<long, int>& oracle);

    static void build_oracle(const std::string& dataset_filepath,
                            const std::string &type, const std::string &output_path, double perc_retain);

    static void sort_edgemap(const ankerl::unordered_dense::map<Edge, int, hash_edge> &map, std::vector<std::pair<Edge, int>> &edge_map);

    static bool sort_edge_map_comp(const std::pair<Edge, int> &a, const std::pair<Edge, int> &b);

    static bool sort_edge_time(const std::pair<Edge, int> &a, const std::pair<Edge, int> &b);

    static void preprocess_data(const std::string& dataset_filepath, const char* delimiter,
                                int skip, const std::string &output_path);


private:

    std::mt19937 e2;

};


#endif //TRIANGLESWITHPREDICTIONS_UTILS_H
