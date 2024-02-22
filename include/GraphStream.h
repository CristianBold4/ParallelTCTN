#ifndef TRIANGLESWITHPREDICTIONS_GRAPHSTREAM_H
#define TRIANGLESWITHPREDICTIONS_GRAPHSTREAM_H

#include <algorithm>
#include <vector>
#include <queue>
#include <fstream>

#define CHUNK_SIZE 500000

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
    inline std::vector<EdgeStream> get_edges() { return stream_vector_; }


private:

    std::ifstream graph_stream_;
    char delimiter_;
    int skip_;
    std::vector<EdgeStream> stream_vector_;

};


#endif //TRIANGLESWITHPREDICTIONS_GRAPHSTREAM_H
