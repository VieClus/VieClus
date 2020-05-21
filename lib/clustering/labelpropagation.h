/******************************************************************************
 * labelpropagation.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 *****************************************************************************/


#ifndef LABELPROPAGATION_H
#define LABELPROPAGATION_H

#include "data_structure/graph_access.h"
#include "data_structure/graph_hierarchy.h"
#include "partition/partition_config.h"


/**
    \brief To perform (multi level) label propagation.
 */
class LabelPropagation
{
    public:
        LabelPropagation();
        virtual ~LabelPropagation();


        /**
            \brief Performs multi level label propagation.

            Perform 2 phases:
            Phase 1: Perform label propagation to get a clustering.
            Phase 2: Aggregate nodes of same cluster to a single node in a new clustering.
            Build coarse graph in which nodes represent clusters.

            \param config Clustering settings.
            \param G Graph to be clustered. Any previous partitioning
            or clustering is withdrawn. Found clusters will be set.
            \param numberOfLevels Number of coarse graphs (size of graph hierarchy)
            where the label propagation is applied.
            Use 1 to perform label propagation without coarsening.
            Using a value of <=0 does nothing.
            \param numberOfLPIterations How often the tree is traversed at most in
            random order while applying label propagation.

            \return Number of computed clusters.
         */
        PartitionID performMultiLevelLabelPropagation(const PartitionConfig &config,
                                                      graph_access *G);


        /**
            \brief Performs label propagation on the graph without coarsening.

            Tree is traversed in random order and a node is assigned to the cluster
            to which it has the most weighted edges.
            Graph shall already have (singleton) clusters.

            \param config Clustering settings.
            \param G Graph on that label propagation is performed.
            \param numberOfLPIterations How often the whole tree is traversed in
            random order at most.

            \return Number of node moves, total count of all turns.
         */
        static NodeID performLabelPropagation(const PartitionConfig &config,
                                              graph_access *G);

    protected:
        /**
            \brief Assigns each node to an own cluster.
         */
        void initializeSingletonClusters();


        /**
            \brief Performs label propagation on the current graph without coarsening.

            Tree is traversed in random order and a node is assigned to the cluster
            to which it has the most weighted edges.
            Graph shall already have (singleton) clusters.

            \param config Clustering settings.
            \param numberOfLPIterations How often the whole tree is traversed in
            random order at most.

            \return Number of node moves, total count of all turns.
         */
        NodeID performLabelPropagation(const PartitionConfig &config);


        /// Current graph that is evaluated.
        graph_access *m_G;
    private:
};

#endif // LABELPROPAGATION_H
