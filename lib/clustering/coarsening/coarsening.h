/******************************************************************************
 * coarsening.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 *****************************************************************************/


#ifndef COARSENING_H
#define COARSENING_H


#include "data_structure/graph_access.h"
#include "data_structure/graph_hierarchy.h"
#include "partition/partition_config.h"
#include <list>
#include <unordered_map>
#include <vector>

/**
    \brief Contains the functionality to transform a fine graph into a coarser
    one based on the clusters the finer one has.
 */
class Coarsening
{
    public:
        Coarsening();
        virtual ~Coarsening();

        /**
            \brief Builds coarse graph based on the clustering of the graph.

            A cluster in the fine graph is then represented by a single node
            (and cluster) in the coarse graph.

            \param config Clustering settings.
            \param G Fine graph with clusters to be coarsened.
            \param graphHierarchy [in/out] List where the fine graph and the mapping to the coarse
            graph are appended.
            \param coarseGraphsToDelete [in/out] List of coarse graphs that need to
            be freed at the end (after uncoarsening).

            \return Returns new coarse graph. The memory of this graph needs to be freed.
         */
        static graph_access *performCoarsening(const PartitionConfig &config,
                                               graph_access &G,
                                               graph_hierarchy &graphHierarchy,
                                               std::list<graph_access *> &coarseGraphsToDelete);


    protected:
        /**
            \brief Builds a look-up table old-to-new cluster ID.

            The old cluster IDs may be in the range [0,nodeCount].
            They are transformed to the new cluster ID range [0,clusterCount-1].

            \param G Fine graph with clusters to be coarsened.
            \param clusterIDLookUp [out] On return KEY is the old cluster ID and
            VALUE is the new cluster ID.
         */
        static void buildClusterIDLookUpTable(graph_access &G,
                                              std::unordered_map<PartitionID, PartitionID> &clusterIDLookUp);


        /**
            \brief Builds a mapping which nodes belong to which cluster.

            Builds a mapping "node n is part of cluster c" and a reverse
            mapping "cluster c consists of nodes..."
            Cluster IDs of the nodes are updated to the new cluster IDs
            in the "clusterIDLookUpTable".

            \param G Fine graph with clusters to be coarsened.
            \param clusterIDLookUp Maps old cluster ID to new cluster ID.
            KEY is the old cluster ID and VALUE is the new cluster ID.
            \param coarseMapping [out] On return at position i there is the
            (new) cluster ID of node i.
            \param reverseCoarseMapping [out] On return at position i there is a
            list of nodes that belong to the (new) cluster i.
         */
        static void buildCoarseMapping(graph_access &G,
                                       const std::unordered_map<PartitionID, PartitionID> &clusterIDLookUp,
                                       CoarseMapping &coarseMapping,
                                       std::vector<std::vector<NodeID> > &reverseCoarseMapping);


    private:
};

#endif // COARSENING_H
