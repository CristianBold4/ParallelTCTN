# Tonic algorithm for estimating triangles in graph streams

Here are the instruction for running *Tonic* algorithm: **T**iangles c**O**unti**N**g with pred**IC**tiond. 
Code is deployed in *C++ 17* under *gcc 9.4.0* compiler. Additionally, *CMake 3.16+* is required.

1. Compile the code
   
   `bash compile.sh` 
3. Preprocess the raw dataset
   
   `build/DataPreprocessing (dataset_path) (delimiter) (skip) (oiutput_path)`

5. Build the Oracle

    `build/BuildOracle (preprocessed_dataset_path) (type of the oracle = [Exact, MinDeg]) (fraction_retain_top_edges) (output_path)`

8. Run *Tonic* Algorithm: 

    `build/Tonic (random_seed) (memory_budget) (alpha) (beta) (preprocessed_dataset_path) (oracle_path) (output_path)`

We provide some examples of datasets and oracles inside the respective folders, so that you can skip directly to step (4) after having compiled the sources.


## Datasets

Here are the links to the datasets we used to perform the experiments.

### Global and Local Triangles Experiments:

| Dataset    | Nodes | Edges | Triangles |
| -------- | ------- | -------- | ------- |
| <a href=https://networkrepository.com/edit-enwikibooks.php>Edit EN Wikibooks</a>  | 133k | 386k | 178k |
| <a href=https://snap.stanford.edu/data/cit-HepPh.html>ArXiv HepPh Citations</a> | 34k | 420k | 1.2M |
| <a href=https://socialnetworks.mpi-sws.org/data-wosn2009.html>Facebook Links</a> | 63k | 817k | 3.5M |
| <a href=https://networkrepository.com/soc_youtube_snap.php>SOC YouTube SNAP</a> | 1.1M | 2.9M | 3.05M |
| <a href=https://networkrepository.com/soc-youtube-growth.php>SOC YouTube Growth</a> | 3.2M | 9.3M | 12.3M |
| <a href=https://snap.stanford.edu/data/cit-Patents.html>Cit US Patents</a> | 3.7M | 16.5M | 7.5M |
| <a href=http://konect.cc/networks/actor-collaboration>Actors Collaborations</a>| 382k | 15M | 346M |
| <a href=http://konect.cc/networks/sx-stackoverflow>Stackoverflow </a> | 2.5M | 28M | 114M |
| <a href=https://snap.stanford.edu/data/soc-LiveJournal1.html>SOC Live Journals</a> | 4.8M | 42.8M | 285.7M |

### Snapshot Experiments:
This kind of datasets consists in a collection of consecutive graphs registered each in a given time of interval, 
meaning that the topology betweeen each graph is only slightly varying in terms of nodes and edges (and, consequently triangles).

| Dataset | Number of Graphs | Description | Max Nodes | Max Edges | Max Triangles |
| ------- | -------------------------- | ----------- | ---------------- | ---------------- | -------------------- |
| <a href=https://snap.stanford.edu/data/Oregon-1.html>Autonomous systems - Oregon-1</a> | 9 | 9 graphs of Autonomous Systems (AS) peering information inferred from Oregon route-views between March 31 2001 and May 26 2001 | 11k | 23k | 19k |
| <a href=https://snap.stanford.edu/data/Oregon-2.html>Autonomous systems - Oregon-2</a> | 9 | Oregon 1 dataset, looking glass data, and Routing registry, all combined | 11k | 32k | 89k |
| <a href=https://snap.stanford.edu/data/as-caida.html>CAIDA AS Relationships Datasets</a> | 122 | <a href=http://www.caida.org/data/active/as-relationships/.> CAIDA AS </a> graphs, derived from a set of RouteViews BGP table snapshots, from January 2004 to November 2007 | 26k | 106k | 36k | 
| <a href=https://snap.stanford.edu/data/as-733.html>Autonomous systems AS-733</a> | 733 | 733 Daily instances which span an interval of 785 days from November 8 1997 to January 2 2000, from the BGP logs | 6k | 13k | 6k |



