#include "louvainmethod.h"

#include "clustering/coarsening/coarsening.h"
#include "clustering/labelpropagation.h"
#include "clustering/neighborhood.h"
#include "coarsening/clustering/size_constraint_label_propagation.h"
#include "logging/experiments/experimentinfo.h"
#include "logging/log.h"
#include "partition/coarsening/clustering/node_ordering.h"
#include "partition/coarsening/contraction.h"
#include "timer.h"
#include "tools/modularitymetric.h"
#include "tools/random_functions.h"

#include <iomanip>      // setprecision()
#include <limits>       // numeric_limits<>
#include <unordered_set>
#include <vector>

using namespace std;

LouvainMethod::LouvainMethod()
    : m_G(0)
{
    //ctor
}


LouvainMethod::~LouvainMethod()
{

}


PartitionID LouvainMethod::performClustering(PartitionConfig &config, graph_access *G, bool start_w_singletons)
{
    // to avoid code duplication we just call the extensive function
    // without doing label propagation
    //config.lm_number_of_label_propagation_levels = 0;
    //config.lm_number_of_label_propagation_iterations = 0;
    //config.lm_cluster_coarsening_factor = 0;
    return LouvainMethod::performClusteringWithLPP(config, G, start_w_singletons);
}


PartitionID LouvainMethod::performClusteringWithLPP(const PartitionConfig& config,
                                                    graph_access* G, bool start_w_singletons)
{
    /// number of node moves between clusters
    NodeID numberOfMoves = 0;
    unsigned coarsenings = 0;
    /// to store the levels of coarse graphs
    graph_hierarchy graphHierarchy;
    /// list of coarse graphs we can delete at the end
    list<graph_access *> coarseGraphsToDelete;
    /// for measuring fine times
    timer timer;

    m_G = G;
    THR_EXP_IF(m_G == 0, "Graph to cluster is NULL.");

    // to make the graph rapidly smaller we apply some levels of label propagation
    // loop with two phases until no more node moves:
    //  1. Assign node to cluster, where the most neighbors belong to.
    //  2. Aggregate nodes of same cluster to single node in new cluster.
    //     Build coarse graph in which nodes represent clusters
    for (unsigned i = 0; i < config.lm_number_of_label_propagation_levels; ++i)
    {
        timer.restart();
        // initialize each node as own cluster
        if(start_w_singletons) { initializeSingletonClusters(); }

        // phase 1: perform label propagation
        // size constrained?
        if (config.lm_cluster_coarsening_factor > 0)
        {
            // KaHIP version
            NodeID no_blocks = 0;
            std::vector< NodeID > cluster_id;
            size_constraint_label_propagation sclp;

            // "cluster_id" is set per reference
            sclp.label_propagation(config, *m_G, cluster_id, no_blocks);

            // have to set the partition index for
            forall_nodes((*m_G), node) {
                m_G->setPartitionIndex(node, cluster_id[node]);
            } endfor
            // have to set it non-zero
            numberOfMoves = 1;
        }
        else // use our own version without a size constraint
        {
            numberOfMoves = LabelPropagation::performLabelPropagation(config, m_G);
        }

        // phase 2: contract nodes/clusters
        // only when there was a move we contract
        if (numberOfMoves)
        {
            m_G = Coarsening::performCoarsening(config, *m_G, graphHierarchy, coarseGraphsToDelete);
            coarsenings++;
        }


        if (!numberOfMoves)
        {
            break;
        }
    }

    // here starts the standard Louvain algorithm
    // loop with two phases until no more node moves:
    //  1. Maximize modularity by assigning nodes to other clusters.
    //  2. Aggregate nodes of same cluster to single node in new cluster.
    //     Build coarse graph in which nodes represent clusters
    do
    {
        timer.restart();

        // initialize each node as own cluster
        if(start_w_singletons) { initializeSingletonClusters(); }

        // phase 1: maximize modularity by assigning nodes to new clusters
        // as long as there is a (minimum) improvement
        numberOfMoves = performNodeMoves(config);

        // phase 2: contract nodes/clusters
        // only when there was a move we contract
        if (numberOfMoves)
        {
            m_G = Coarsening::performCoarsening(config, *m_G, graphHierarchy, coarseGraphsToDelete);
            coarsenings++;

        }
    }
    while (numberOfMoves);

    // append the last created level
    // this is also done in KaHIP
    // and a kind of necessary, otherwise we do not get
    // the 2. most coarsest graph in the uncoarsening phase
    // from the graph hierarchy
    graphHierarchy.push_back(m_G, 0);

    // uncoarsening, apply clustering to original graph
    // and do local refinement
    // XXX: if louvain or LP never did a coarsening, this would lead to an invalid
    //      read in pop_finer_and_project() (inside pop_coarsest), so only do this if
    //      coarsenings > 0. could also use m_G == G as a condition
    while (coarsenings > 0 && !graphHierarchy.isEmpty())
    {
        timer.restart();

        m_G = graphHierarchy.pop_finer_and_project(); // XXX

        // phase 1: maximize modularity by assigning nodes to new clusters
        // as long as there is a (minimum) improvement
        // refinement of result
        numberOfMoves = performNodeMoves(config);
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

    std::unordered_map<PartitionID, PartitionID> new_mapping;
    PartitionID id = 0;

    forall_nodes((*m_G), node) {
        PartitionID cluster = m_G->getPartitionIndex(node);
        if(!new_mapping.count(cluster)) { new_mapping[cluster] = id++; }
        m_G->setPartitionIndex(node, new_mapping[cluster]);
    } endfor

    m_G->set_partition_count(id);

    return m_G->get_partition_count();
}


void LouvainMethod::initializeSingletonClusters()
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


NodeID LouvainMethod::performNodeMoves(const PartitionConfig &config)
{
    /// normally modularity should be in the range [-1,1]
    double currentQuality = -2.0;
    double oldQuality = -2.0;
    /// generates random order how we traverse the nodes in the 1. phase
    node_ordering nodesOrder;
    /// random order of nodes how we traverse them in the 1. phase
    vector<NodeID> permutation;
    /// number of node moves between clusters
    NodeID numberOfMoves = 0;
    /// objective function to optimize/maximize
    /// calculates modularity and keeps track of the clustering,
    /// could be replaced by any other objective
    ModularityMetric *objective = 0;
    /// TRUE, if graph has self loops
    bool hasGraphSelfLoops = m_G->containsSelfLoops();
    /// info about neighboring clusters of the currently traversed node
    Neighborhood neighborhood;
    /// for measuring time
    timer timer;

    // the permutation vector may not be larger than the number of nodes
    permutation.resize(m_G->number_of_nodes());
    // initialize with increasing values starting with zero: 0, 1, 2, 3, 4...
    // but nodesOrder.order_nodes() does the same
    // iota(permutation.begin(), permutation.end(), 0);

    // fill and permute the node order
    // maybe it is sufficient to permute only once
    // in the outer loop and not every time in the inner one
    // nodesOrder.order_nodes(config, *m_G, permutation);

    random_functions::permutate_vector_good( permutation, true);
    //std::iota(permutation.begin(), permutation.end(), 0);
    //for(size_t i = 0; i < permutation.size(); ++i) {
        //size_t pos = rand() % (permutation.size() - i) + i;
        //std::swap(permutation[i], permutation[pos]);
    //}

    // set neighborhood data structures
    neighborhood.initialize(m_G);

    // create modularity object that internally keeps track of
    // the current clustering in the current graph
    objective = new ModularityMetric(*m_G);
    currentQuality = objective->quality();


    // phase 1: maximize modularity by assigning nodes to new clusters
    // as long as there is a (minimum) improvement
    do
    {
        timer.restart();

        // small numeric differences may be possible for large graphs
        // between the fine and the coarse graph
        LOG_WARN_IF(currentQuality < oldQuality && abs(currentQuality - oldQuality) > numeric_limits<double>::epsilon(), "Current modularity may not be worse than old one.");

//            if (currentQuality < oldQuality)
//            {
//                cout << setprecision(18) << currentQuality << endl;
//                cout << oldQuality << endl;
//                cout << (currentQuality - oldQuality) << endl;
//                cout << numeric_limits<double>::epsilon() << endl;
//            }

        oldQuality = currentQuality;

        // fill and permute the node order
        // maybe it would be sufficient to permute only once
        // in the outer loop and not every time here in the inner one
        // nodesOrder.order_nodes(config, *m_G, permutation);

        // traverse nodes in random order
        forall_nodes((*m_G), nn)
        {
            NodeID node = permutation[nn];

            // computation of neighboring clusters
            neighborhood.update(node);

            // we just have to watch for a gain, if there are
            // other clusters too
            if (neighborhood.getNumberOfNeighboringClusters() > 1)
            {
                PartitionID oldCluster = m_G->getPartitionIndex(node);
                PartitionID bestCluster = oldCluster;
                double bestGain = 0.0;
                EdgeWeight selfLoop = 0;

                if (hasGraphSelfLoops)
                {
                    selfLoop = m_G->getSelfLoop(node);
                }

                // remove the current node from its cluster
                objective->removeNode(node, oldCluster, neighborhood.getEdgeWeightToNeighboringCluster(oldCluster), selfLoop);

                // find best cluster in the neighborhood
                // as we have already calculated the neighboring clusters
                // we just iterate over them and not over all out edges
                for (NodeID i = 0; i < neighborhood.getNumberOfNeighboringClusters(); ++i)
                {
                    PartitionID newCluster = neighborhood.getClusterIDOfNeighbor(i);
                    // the gain is not normalized, it is not the real improvement
                    double gain = objective->gain(node, newCluster, neighborhood.getEdgeWeightToNeighboringCluster(newCluster));

                    if (bestGain < gain)
                    {
                        bestGain = gain;
                        bestCluster = newCluster;
                    }
                }

                // assign node to best cluster
                // at least we assign it to the old cluster again
                objective->insertNode(node, bestCluster, neighborhood.getEdgeWeightToNeighboringCluster(bestCluster), selfLoop);

                if (oldCluster != bestCluster)
                {
                    numberOfMoves++;
                }
            }
        } endfor

        currentQuality = objective->quality();
    }
    while (currentQuality - oldQuality > config.lm_minimum_quality_improvement);

    delete objective;

    return numberOfMoves;
}
