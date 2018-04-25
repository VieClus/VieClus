/******************************************************************************
 * labelpropagation.cpp
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


#include "labelpropagation.h"

#include "clustering/coarsening/coarsening.h"
#include "logging/experiments/experimentinfo.h"
#include "logging/log.h"
#include "partition/coarsening/clustering/node_ordering.h"
#include "partition/coarsening/contraction.h"
#include "timer.h"
#include "tools/modularitymetric.h"

#include <list>

using namespace std;

LabelPropagation::LabelPropagation()
    : m_G(0)
{
    //ctor
}

LabelPropagation::~LabelPropagation()
{
    //dtor
}


PartitionID LabelPropagation::performMultiLevelLabelPropagation(const PartitionConfig &config,
                                                                graph_access *G)
{
    /// number of node moves between clusters
    NodeID numberOfMoves = 0;
    /// to store the levels of coarse graphs
    graph_hierarchy graphHierarchy;
    /// list of coarse graphs we can delete at the end
    list<graph_access *> coarseGraphsToDelete;
    /// for measuring fine times
    timer timer;

    m_G = G;
    THR_EXP_IF(m_G == 0, "Graph to cluster is NULL.");

    // loop with two phases until no more node moves:
    //  1. Assign node to cluster, where the most neighbors belong to.
    //  2. Aggregate nodes of same cluster to single node in new cluster.
    //     Build coarse graph in which nodes represent clusters
    for (unsigned i = 0; i < config.lm_number_of_label_propagation_levels; ++i)
    {
        timer.elapsed();

        // initialize each node as own cluster
        initializeSingletonClusters();

        // phase 1: perform label propagation
        numberOfMoves = performLabelPropagation(config);

        // phase 2: contract nodes/clusters
        // only when there was a move we contract
        if (numberOfMoves)
        {
            m_G = Coarsening::performCoarsening(config, *m_G, graphHierarchy, coarseGraphsToDelete);
        }

        {
            // we do not want to measure the modularity computation
            //double time = timer.elapsed();
            // ExperimentInfo::INSTANCE->reportLPLevel(time,
                                                    // m_G->number_of_edges(),
                                                    // m_G->number_of_nodes(),
                                                    // ModularityMetric::computeModularity(*m_G));
        }

        if (!numberOfMoves)
        {
            //if no more moves, than we can abort the label propagation
            break;
        }
    }


    // append the last created level
    // this is also done in KaHIP
    // and a kind of necessary, otherwise we do not get
    // the 2. most coarsest graph in the uncoarsening phase
    // from the graph hierarchy
    graphHierarchy.push_back(m_G, 0);

    // uncoarsening, apply clustering to original graph
    while (!graphHierarchy.isEmpty())
    {
        m_G = graphHierarchy.pop_finer_and_project();

        // phase 1: label propagation again
        // refinement of result
        // indeed it can make the result worse in terms of modularity
        performLabelPropagation(config);
    }

    // graph hierarchy does not free the coarse graphs
    // although it would have a list for that (but unused)
    // coarse mappings are deleted by the graph hierarchy
    while (!coarseGraphsToDelete.empty())
    {
        delete coarseGraphsToDelete.front();
        coarseGraphsToDelete.pop_front();
    }

    THR_EXP_IF(m_G == 0, "Uncoarsening went wrong.");

    return m_G->get_partition_count();
}


void LabelPropagation::initializeSingletonClusters()
{
    PartitionID clusterID = 0;

    // at the beginning each node in the graph
    // is an own cluster
    forall_nodes((*m_G), n)
    {
        m_G->setPartitionIndex(n, clusterID);
        clusterID++;

    } endfor

    // number of cluster needs to be updated
    m_G->set_partition_count(clusterID);
}


NodeID LabelPropagation::performLabelPropagation(const PartitionConfig& config)
{
    /// generates random order how we traverse the nodes in the 1. phase
    node_ordering nodesOrder;
    /// random order of nodes how we traverse them in the 1. phase
    vector<NodeID> permutation;
    /// number of node moves between clusters (in last iteration)
    NodeID numberOfNodeMoves = 0;
    /// edge weights to local clusters in the neighborhood
    vector<EdgeWeight> edgeWeightsToClusters;

    // the permutation vector may not be larger than the number of nodes
    permutation.resize(m_G->number_of_nodes());
    // initialize with increasing values starting with zero: 0, 1, 2, 3, 4...
    // but nodesOrder.order_nodes() does the same
    // iota(permutation.begin(), permutation.end(), 0);

    // fill and permute the node order
    // maybe it is sufficient to permute only once
    // in the outer loop and not every time in the inner one
    nodesOrder.order_nodes(config, *m_G, permutation);

    // in the worst case a node has weights to all other nodes
    // Based on Tests of Alexander Noe it is best to use
    // this large array as "hash map" than a real
    // (unordered_)map, because the allocations of the map
    // during the loop slows down
    edgeWeightsToClusters.assign(m_G->get_partition_count_compute(), 0);

    for (unsigned i = 0; i < config.lm_number_of_label_propagation_iterations; ++i)
    {
        // to know whether there was a change in the inner loop
        NodeID oldNumberOfNodeMoves = numberOfNodeMoves;

        forall_nodes((*m_G), nn)
        {
            NodeID node = permutation[nn];
            PartitionID oldCluster = m_G->getPartitionIndex(node);
            /// most occurring cluster in the neighborhood, we assign the current node to
            PartitionID bestCluster = oldCluster;
            // maybe we should add here the self loop
            // but on the other side, then in a higher level
            // we would perhaps never move a node anymore, because
            // we self loop would be to heavy (best weight)
            EdgeWeight bestWeight = 0;

            // determine edge weights to neighboring clusters
            forall_out_edges((*m_G), e, node)
            {
                NodeID neighbor = m_G->getEdgeTarget(e);
                PartitionID clusterOfNeighbor = m_G->getPartitionIndex(neighbor);

                edgeWeightsToClusters[clusterOfNeighbor] += m_G->getEdgeWeight(e);
            } endfor

            // find neighboring cluster where we have the most weighted edges to
            forall_out_edges((*m_G), e, node)
            {
                NodeID neighbor = m_G->getEdgeTarget(e);
                PartitionID clusterOfNeighbor = m_G->getPartitionIndex(neighbor);
                EdgeWeight edgeWeightToCluster = edgeWeightsToClusters[clusterOfNeighbor];

                // when the current weight is equal, then
                // we choose it by random 50%
                if ((edgeWeightToCluster > bestWeight) ||
                    (edgeWeightToCluster == bestWeight && random_functions::nextBool()))
                {
                    bestWeight = edgeWeightToCluster;
                    bestCluster = clusterOfNeighbor;
                }

                // reset for the next iteration/node
                edgeWeightsToClusters[clusterOfNeighbor] = 0;

            } endfor

            // do we have a better cluster?
            if (oldCluster != bestCluster)
            {
                // then assign new node to better cluster
                m_G->setPartitionIndex(node, bestCluster);
                numberOfNodeMoves++;
            }
        } endfor

        // when there was no move in the inner loop,
        // then we can abort
        if (oldNumberOfNodeMoves == numberOfNodeMoves)
        {
            break;
        }
    }

    return numberOfNodeMoves;
}


// static version to be used also from other classes
// e.g. from LouvainMethod
NodeID LabelPropagation::performLabelPropagation(const PartitionConfig& config,
                                                 graph_access *G)
{
    LabelPropagation lp;
    // to call the member function safely we have to set the graph
    // that we wanna cluster
    lp.m_G = G;

    return lp.performLabelPropagation(config);
}
