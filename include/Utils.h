#ifndef TRIANGLESWITHPREDICTIONS_UTILS_H
#define TRIANGLESWITHPREDICTIONS_UTILS_H

#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <sstream>
#include "ankerl/unordered_dense.h"
#include "GraphStream.h"
#include "Subgraph.h"


class Utils {

    static bool sort_edge_map_comp(const std::pair<EdgeTemp, int> &e1, const std::pair<EdgeTemp, int> &e2) {
        return e1.second > e2.second;
    }

public:



    static void sort_edgemap(const ankerl::unordered_dense::map<EdgeTemp, int, hash_edge>& map,
                                    std::vector<std::pair<EdgeTemp, int>> &edge_map);

    static void build_oracle(std::string &dataset_path, int delta, std::string &type_oracle,
                             double perc_retain, std::string &output_path);

    static void build_ground_truth(std::string &dataset_path, int delta, std::string &type_oracle,
                             double perc_retain, std::string &exact_output_path, std::string &oracle_output_path);

    static int count_triangle(EdgeTemp e1, EdgeTemp e2, EdgeTemp e3);

    static void read_oracle();


private:


};


#endif //TRIANGLESWITHPREDICTIONS_UTILS_H
