#include "contractor.h"

using namespace std;

Contractor::Contractor()
{
    //ctor
}

Contractor::~Contractor()
{
    //dtor
}


void Contractor::contractClustering(const PartitionConfig &config,
                                    graph_access &finer,
                                    graph_access &coarser,
                                    const CoarseMapping &coarseMapping,
                                    const std::vector<std::vector<NodeID> > &reverseCoarseMapping)
{
    const PartitionID numberOfClusters = reverseCoarseMapping.size();
    // if from cluster i to cluster j there are multiple out edges
    // in the finer graph, then for the first out edge
    // we create an edge e in the coarse graph and for
    // the remaining out edges we want to update edge e of before,
    // to remember edge e we need this look-up table
    // the PartitionID informs whether the info about the edge is
    // from the current cluster or outdated/from previous cluster)
    vector<pair<PartitionID, EdgeID> > edgeLookUp(numberOfClusters, make_pair(UNDEFINED_NODE, UNDEFINED_EDGE));

    // we do not know the exact number of edges in advance,
    // therefore we use at first an upper bound and resize at the end
    coarser.start_construction(numberOfClusters, finer.number_of_edges());
    // resize array for self loops
    coarser.resizeSelfLoops(numberOfClusters);

    // traverse the new clusters
    for (PartitionID cluster = 0; cluster < numberOfClusters; ++cluster)
    {
        // should be the same number as "cluster"
        NodeID coarserNode = coarser.new_node();
        EdgeWeight weightOfSelfLoop = 0;
        // node weight, is important for size constrained label propagation
        NodeWeight coarserNodeWeight = 0;

        // traverse the nodes in the cluster of the finer graph
        for (NodeID node = 0, clusterSize = reverseCoarseMapping.at(cluster).size();
             node < clusterSize; ++node)
        {
            // node in finer graph
            NodeID finerNode = reverseCoarseMapping[cluster][node];

            coarserNodeWeight += finer.getNodeWeight(finerNode);

            // transfer self loops of finer graph to coarser one
            // the original graph could have no self loops
            if (finer.containsSelfLoops())
            {
                weightOfSelfLoop += finer.getSelfLoop(finerNode);
            }

            // traverse the out edges of a node in the cluster of the finer graph
            forall_out_edges(finer, e, finerNode)
            {
                EdgeWeight weightOfCurrentOutEdge = finer.getEdgeWeight(e);
                NodeID targetNodeInFinerGraph = finer.getEdgeTarget(e);
                NodeID targetClusterInCoarseGraph = coarseMapping[targetNodeInFinerGraph];
                PartitionID clusterOfEdgeLookUp;

                // target node belongs to same cluster as source node?
                if (targetClusterInCoarseGraph == cluster)
                {
                    // edge inside cluster becomes a self loop
                    weightOfSelfLoop += weightOfCurrentOutEdge;

                    // we do not represent self loops in the "normal" edge data structure
                    // only in that one for self loops
                    // therefore we continue with the next iteration
                    continue;
                }

                clusterOfEdgeLookUp = edgeLookUp[targetClusterInCoarseGraph].first;

                // is info in edge look-up already for the current cluster?
                if (clusterOfEdgeLookUp == cluster)
                {
                    // update the existing edge in the coarse graph
                    EdgeID edgeInCoarseGraph = edgeLookUp[targetClusterInCoarseGraph].second;
                    EdgeWeight weightInCoarseGraph = coarser.getEdgeWeight(edgeInCoarseGraph);

                    coarser.setEdgeWeight(edgeInCoarseGraph, weightInCoarseGraph + weightOfCurrentOutEdge);
                }
                else
                {
                    // insert a new edge in the coarse graph
                    EdgeID newEdge = coarser.new_edge(coarserNode, targetClusterInCoarseGraph);
                    // update look-up info
                    edgeLookUp[targetClusterInCoarseGraph].first = cluster;
                    edgeLookUp[targetClusterInCoarseGraph].second = newEdge;

                    coarser.setEdgeWeight(newEdge, weightOfCurrentOutEdge);
                }
            } endfor
        }

        coarser.setPartitionIndex(coarserNode, cluster);
        coarser.setSelfLoop(coarserNode, weightOfSelfLoop);
        coarser.setNodeWeight(coarserNode, coarserNodeWeight);
    }

    coarser.set_partition_count(numberOfClusters);

    // resize to actual number of nodes
    coarser.finish_construction();
}
