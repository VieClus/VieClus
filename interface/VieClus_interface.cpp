#include "VieClus_interface.h"

#include <argtable2.h>
#include <iostream>
#include <math.h>
#include <mpi.h>
#include <regex.h>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include "algorithms/cycle_search.h"
#include "balance_configuration.h"
#include "data_structure/graph_access.h"
#include "graph_io.h"
#include "macros_assertions.h"
#include "parallel_mh_clustering/parallel_mh_async_clustering.h"
#include "parse_parameters.h"
#include "partition/graph_partitioner.h"
#include "partition/partition_config.h"
#include "quality_metrics.h"
#include "random_functions.h"
#include "timer.h"

namespace VieClus {

__attribute__((visibility("default"))) void init(int *argc, char **argv[]) {
    MPI_Init(argc, argv);
    omp_set_num_threads(1);
}

__attribute__((visibility("default"))) double run(Graph graph, int time_limit, int seed, int *out_k, int *out_partition_map) {
	graph_access G;
    if (graph.vwgt == nullptr || graph.adjwgt == nullptr) {
        G.build_from_metis(graph.n, graph.xadj, graph.adjncy);
    } else {
        G.build_from_metis_weighted(graph.n, graph.xadj, graph.adjncy, graph.vwgt, graph.adjwgt);
    }

	PartitionConfig partition_config;
	configuration cfg;
	cfg.standard(partition_config);
	cfg.strong(partition_config);
	partition_config.time_limit = time_limit;
	partition_config.k = 1;
	partition_config.seed = seed;

	parallel_mh_async_clustering mh;
	mh.perform_partitioning(partition_config, G);

	G.set_partition_count(G.get_partition_count_compute());

	if (out_k != nullptr) *out_k = G.get_partition_count();
	if (out_partition_map != nullptr) {
		for (NodeID v = 0; v < G.number_of_nodes(); ++v) {
			out_partition_map[v] = G.getPartitionIndex(v);
		}
	}

	return ModularityMetric::computeModularity(G);
}

__attribute__((visibility("default"))) void finalize() {
  MPI_Finalize();
}

}