/******************************************************************************
 * louvainmethod.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 ******************************************************************************
 * Copyright (C) 2017 Sonja Biedermann, Christian Schulz and Bernhard Schuster
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#ifndef LOUVAINMETHOD_H
#define LOUVAINMETHOD_H

#include "data_structure/graph_access.h"
#include "data_structure/graph_hierarchy.h"
#include "partition/partition_config.h"

/**
 *  \brief Represents the "Louvain" clustering algorithm.
 *
 *  Maximizing modularity is used as objective for the clustering.
 *  A multi level label propagation can be used as preprocessing to speed up.
 *
 *  The implementation is tailored for weighted integer graphs.
 *
 *  "Fast unfolding of communities in large networks" by
 *  Vincent D Blondel, Jean-Loup Guillaume,Renaud Lambiotte and Etienne Lefebvre
 *  Journal of Statistical Mechanics: Theory and Experimen, P10008, 2008
 */
class LouvainMethod
{
    public:
        LouvainMethod();
        virtual ~LouvainMethod();

        /**
            \brief Performs the "Louvain" algorithm for clustering.

            Uses modularity as objective, but could also use other functions.
            Perform 2 phases as long there is an improvement:
            Phase 1: Maximize modularity by assigning nodes to other clusters.
            Phase 2: Aggregate nodes of same cluster to a single node in new clustering.
            Build coarse graph in which nodes represent clusters

            \param config Clustering settings.
            \param G Graph to be clustered. Any previous partitioning
            or clustering is withdrawn. Found clusters will be set.
            \param minQualityImprovement Minimum improvement
            necessary to make another turn in the first phase of the
            clustering algorithm.

            \return Number of computed clusters.
         */
        PartitionID performClustering(PartitionConfig &config,
                                      graph_access *G, bool = true);


        /**
            \brief Performs the "Louvain" algorithm for clustering with some levels
            of label propagation as preprocessing.

            LPP stands for Label Propagation Preprocessing.
            At first some levels of label propagation are applied to make the graph
            smaller in a fast way, then the Louvain method is used for clustering.
            One can also use size constrained label propagation depending on
            config.cluster_coarsening_factor.
            With a level we mean that we apply label propagation and then coarsen the graph.
            Louvain uses modularity as objective, but could also use other functions.
            Perform 2 phases as long there is an improvement in Louvain:
            Phase 1: Maximize modularity by assigning nodes to other clusters.
            Phase 2: Aggregate nodes of same cluster to a single node in new clustering.
            Build coarse graph in which nodes represent clusters

            \param config Clustering settings.
            \param G Graph to be clustered. Any previous partitioning
            or clustering is withdrawn. Found clusters will be set.
            \param minQualityImprovement Minimum improvement
            necessary to make another turn in the first phase of the
            Louvain algorithm.
            \param numberOfLPLevels Number of coarse graphs (size of graph hierarchy)
            where the label propagation is applied.
            Using a value of <=0 means that no label propagation is done.
            Use a rather small number (0-3) depending on the tree size, otherwise you get only
            a single node or modularity decreases.
            \param numberOfLPIterations How often the tree is traversed at most in
            random order while applying label propagation, f.e. 3.


            \return Number of computed clusters.
         */
        PartitionID performClusteringWithLPP(const PartitionConfig &config,
                                             graph_access *G, bool = true);
    protected:
        /**
            \brief Assigns each node to an own cluster.
         */
        void initializeSingletonClusters();


        /**
            \brief Computes the number of clusters in the current graph.

            \deprecated
         */
        PartitionID computeNumberOfClusters() const;


        /**
            \brief Moves node between clusters as long there is a minimal
            quality improvement of the objective (modularity).

            Phase 1 of the Louvain Algorithm.
            Graph is traversed in random order and nodes are assign to that
            cluster in the local neighborhood that gives the most gain (in
            modularity) as long as there is at least a minimal quality improvement.
            This function can be also used to perform a refinement of a
            given clustering.
            Graph shall already have (singleton) clusters.

            \param config Clustering settings.
            \param minQualityImprovement Minimum quality gain that is necessary
            to make another turn and traverse the whole tree and move nodes.

            \return Number of node moves, total count of all turns.
         */
        NodeID performNodeMoves(const PartitionConfig &config);


        /// Current graph that is evaluated.
        graph_access *m_G;
    private:
};

#endif // LOUVAINMETHOD_H
