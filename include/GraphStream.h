#ifndef PARALLELTCTN_GRAPHSTREAM_H
#define PARALLELTCTN_GRAPHSTREAM_H

#include <algorithm>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>

#define CHUNK_SIZE 1000000

typedef struct EdgeStream {
    int u;
    int v;
    int time;
} EdgeStream;

class GraphStream {

public:

    // -- constructor and destructor
    GraphStream(const std::string &filename, const char &delimiter, int skip);
    virtual ~GraphStream();

    // -- methods
    void read_chunk();

    EdgeStream next();

    inline bool has_next() {
        return !chunk_queue_.empty();
    }

private:

    std::ifstream graph_stream_;
    char delimiter_;
    int skip_;
    std::queue<EdgeStream> chunk_queue_;

};


#endif
