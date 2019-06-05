#ifndef VIECLUS_VIECLUS_H
#define VIECLUS_VIECLUS_H

namespace VieClus {

struct Graph {
	int n;
    int *xadj; //! Nodes array
    int *adjncy; //! Edges array
    int *vwgt; //! Node weights
    int *adjwgt; //! Edge weights
};

__attribute__((visibility("default"))) void init(int *argc, char **argv[]);

__attribute__((visibility("default"))) double run(Graph graph, int time_limit, int seed, int *out_k, int *out_partition_map);

__attribute__((visibility("default"))) void finalize();

}

#endif // VIECLUS_VIECLUS_H
