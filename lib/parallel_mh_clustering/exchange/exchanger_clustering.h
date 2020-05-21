/******************************************************************************
 * exchanger_clustering.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 *****************************************************************************/

#ifndef EXCHANGER_YPB6QKNL
#define EXCHANGER_YPB6QKNL

#include <mpi.h>

#include "data_structure/graph_access.h"
#include "parallel_mh_clustering/population_clustering.h"
#include "partition_config.h"
#include "tools/quality_metrics.h"

class exchanger_clustering {
public:
        exchanger_clustering( MPI_Comm communicator );
        virtual ~exchanger_clustering();

        void diversify_population_clustering( PartitionConfig & config, graph_access & G, population_clustering & island, bool replace );
        void quick_start( PartitionConfig & config,  graph_access & G, population_clustering & island );
        void push_best( PartitionConfig & config,  graph_access & G, population_clustering & island );
        void recv_incoming( PartitionConfig & config,  graph_access & G, population_clustering & island );

private:
        void exchange_individum(const PartitionConfig & config, 
                                graph_access & G, 
                                int & from, 
                                int & rank, 
                                int & to, 
                                Individuum & in, Individuum & out);

        std::vector< int* >          m_partition_map_buffers;
        std::vector< MPI_Request* > m_request_pointers;
        std::vector<bool>            m_allready_send_to;

        double m_prev_best_objective;
        int m_max_num_pushes;
        int m_cur_num_pushes;

        MPI_Comm m_communicator;

        quality_metrics m_qm;
};



#endif /* end of include guard: EXCHANGER_YPB6QKNL */
