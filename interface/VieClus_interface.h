#ifndef VIECLUS_VIECLUS_H
#define VIECLUS_VIECLUS_H

namespace VieClus {

struct Graph {
	int n;
	int *xadj;
	int *adjncy;
};

__attribute__((visibility("default"))) double run_default(Graph graph, int time_limit, int *out_k, int *out_partition_map);

}

#endif // VIECLUS_VIECLUS_H
