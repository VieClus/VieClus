#ifndef CONTRACTOR_H
#define CONTRACTOR_H


#include "data_structure/graph_access.h"
#include "partition/partition_config.h"

/**
 *  \brief Contracts fine graphs to coarser ones.
 */
class Contractor
{
    public:
        Contractor();
        virtual ~Contractor();

        /**
         *  \brief Transforms clustering of a fine graph to a coarser graph.
         *
         *  \param config Configuration, so far not used.
         *  \param finer Fine graph which clusters are then nodes in the coarse graph.
         *  \param coarser [out] Coarse graph that is constructed on basis of the
         *  clusters in the fine graph. Contains self loops.
         *  \param coarseMapping Contains the cluster of each node in the fine graph.
         *  \param reverseCoarseMapping Contains the nodes (of the fine graph) of each cluster.
         *  Contains for each coarse node/cluster the nodes of the fine graph that belong to it.
         *
         *  Each cluster in the fine graph is afterwards a single node (and
         *  cluster) in the coarser graph. Edge weights within clusters of the
         *  fine graph are represented by self loops in the coarser graph.
         */
        static void contractClustering(const PartitionConfig &config,
                                       graph_access &finer,
                                       graph_access &coarser,
                                       const CoarseMapping &coarseMapping,
                                       const std::vector<std::vector<NodeID> > &reverseCoarseMapping);

    protected:

    private:
};

#endif // CONTRACTOR_H
