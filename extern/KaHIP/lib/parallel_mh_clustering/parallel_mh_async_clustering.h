/******************************************************************************
 * parallel_mh_async_clustering.h 
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 *****************************************************************************/

#ifndef PARALLEL_MH_ASYNC_HF106Y0G
#define PARALLEL_MH_ASYNC_HF106Y0G

#include <mpi.h>
#include "data_structure/graph_access.h"
#include "partition_config.h"
#include "population_clustering.h"
#include "timer.h"

class parallel_mh_async_clustering {
public:
        parallel_mh_async_clustering();
        parallel_mh_async_clustering(MPI_Comm communicator);
        virtual ~parallel_mh_async_clustering();

        void perform_partitioning(const PartitionConfig & graph_partitioner_config, graph_access & G);
        void initialize(PartitionConfig & graph_partitioner_config, graph_access & G);
        double perform_local_partitioning(PartitionConfig & graph_partitioner_config, graph_access & G);
        double collect_best_partitioning(graph_access & G, const PartitionConfig & config);
        void perform_cycle_partitioning(PartitionConfig & graph_partitioner_config, graph_access & G);

private:
        //misc
        const unsigned MASTER;
        timer    m_t;
        int      m_rank;
        int      m_size;
        double   m_time_limit;
        bool     m_termination;
        unsigned m_rounds;

        //the best cut found so far
        PartitionID* m_best_global_map;
        int          m_best_global_objective;
        int          m_best_cycle_objective;

        //island
        population_clustering* m_island;
        MPI_Comm m_communicator;
};


#endif /* end of include guard: PARALLEL_MH_ASYNC_HF106Y0G */
