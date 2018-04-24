#include "modularitymetric.h"

//#include "definitions.h"
#include "logging/log.h"

#include <cassert>


using namespace std;

ModularityMetric::ModularityMetric(graph_access &G)
    : m_G(G)
{
    // initialize own data structures
    ModularityMetric::computeEdgeWeightsPerCluster(m_G, m_edgeWeightsPerCluster, m_weightedEdgeEndsPerCluster);
    // we store it as double, because we need it always as a double
    m_sumOfAllEdgeWeights = static_cast<double>(ModularityMetric::computeSumOfAllEdgeWeights(m_G));

    // cache the node degrees
    this->computeWeightedNodeDegrees();
}

ModularityMetric::~ModularityMetric()
{
    //dtor
}


double ModularityMetric::quality() const
{
    double modularity = 0.0;

    for (NodeID i = 0, iEnd = m_weightedEdgeEndsPerCluster.size(); i < iEnd; ++i)
    {
        EdgeWeight weightedEdgeEnds = m_weightedEdgeEndsPerCluster[i];

        // we do not take care for empty clusters
        if (weightedEdgeEnds > 0)
        {
            double edgeFraction = static_cast<double>(m_edgeWeightsPerCluster[i]) / m_sumOfAllEdgeWeights;
            double edgeEndFraction = static_cast<double>(weightedEdgeEnds) / m_sumOfAllEdgeWeights;

            modularity += edgeFraction - edgeEndFraction * edgeEndFraction;
        }
    }

    return modularity;
}


// inlining this function did not give a performance gain
double ModularityMetric::gain(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster) const
{
    double weightedEdgeEndsInCluster = static_cast<double>(m_weightedEdgeEndsPerCluster[cluster]);
    double edgeWeightToClusterDouble = static_cast<double>(edgeWeightToCluster);
    double weightedDegree = static_cast<double>(m_weightedNodeDegrees[node]);

    return edgeWeightToClusterDouble - weightedEdgeEndsInCluster * weightedDegree / m_sumOfAllEdgeWeights;
}


void ModularityMetric::insertNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster)
{
    EdgeWeight selfLoop = 0;

    if (m_G.containsSelfLoops())
    {
        selfLoop = m_G.getSelfLoop(node);
    }

    this->insertNode(node, cluster, edgeWeightToCluster, selfLoop);
}



void ModularityMetric::insertNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster, EdgeWeight selfLoop)
{
    m_edgeWeightsPerCluster[cluster] += 2 * edgeWeightToCluster + selfLoop;
    m_weightedEdgeEndsPerCluster[cluster] += m_G.getWeightedNodeDegree(node) + selfLoop;

    // assign to cluster
    m_G.setPartitionIndex(node, cluster);
}


void ModularityMetric::removeNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster)
{
    EdgeWeight selfLoop = 0;

    if (m_G.containsSelfLoops())
    {
        selfLoop = m_G.getSelfLoop(node);
    }

    this->removeNode(node, cluster, edgeWeightToCluster, selfLoop);
}


void ModularityMetric::removeNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster, EdgeWeight selfLoop)
{
    m_edgeWeightsPerCluster[cluster] -= 2 * edgeWeightToCluster + selfLoop;
    m_weightedEdgeEndsPerCluster[cluster] -= m_G.getWeightedNodeDegree(node) + selfLoop;

    // assign to invalid cluster
    m_G.setPartitionIndex(node, -1);
}


void ModularityMetric::computeWeightedNodeDegrees()
{
    bool hasGraphSelfLoops = m_G.containsSelfLoops();

    m_weightedNodeDegrees.resize(m_G.number_of_nodes());

    forall_nodes(m_G, n)
    {
        EdgeWeight weightedDegree = m_G.getWeightedNodeDegree(n);

        if (hasGraphSelfLoops)
        {
            weightedDegree += m_G.getSelfLoop(n);
        }

        m_weightedNodeDegrees[n] = weightedDegree;
    } endfor
}


// static members
void ModularityMetric::computeEdgeWeightsPerCluster(graph_access& G,
                                                    std::vector<EdgeWeight>& edgeWeightsPerCluster,
                                                    std::vector<EdgeWeight>& weightedEdgeEndsPerCluster)
{
    PartitionID clusterCount = G.get_partition_count();

    edgeWeightsPerCluster.assign(clusterCount, 0);
    weightedEdgeEndsPerCluster.assign(clusterCount, 0);

    // make a warning, because it is likely to crash
    // with index out of bounds later on
    LOG_WARN_IF(clusterCount <= 0, "Cluster count is less or equal to 0. It is likely that you crash because of this.");

    // count for each cluster c the weighted number of edges within this cluster
    // (begin and end of edge are in the same cluster c)
    // and count for each cluster c the weighted number of edge end points
    // that are in this cluster (end of edge is in cluster c)
    // as we need to know the source and target node for each edge
    // we have to use this 2 loops
    // with forall_edges() we have no info about the source node,
    // only the target node
    forall_nodes(G, n)
    {
        PartitionID sourceClusterIndex = G.getPartitionIndex(n);

        forall_out_edges(G, e, n)
        {
            PartitionID targetClusterIndex = G.getPartitionIndex(G.getEdgeTarget(e));
            EdgeWeight edgeWeight = G.getEdgeWeight(e);

            if (sourceClusterIndex == targetClusterIndex)
            {
                edgeWeightsPerCluster.at(sourceClusterIndex) += edgeWeight;
            }

            weightedEdgeEndsPerCluster.at(sourceClusterIndex) += edgeWeight;
        } endfor

        // we also have to take the self loops into account
        // they are not part in the normal edge data structure
        if (G.containsSelfLoops())
        {
            EdgeWeight selfLoop = G.getSelfLoop(n);
            edgeWeightsPerCluster.at(sourceClusterIndex) += selfLoop;
            weightedEdgeEndsPerCluster.at(sourceClusterIndex) += selfLoop;
        }
    } endfor
}

double ModularityMetric::computeModularity(graph_access &G)
{
    double modularity = 0.0;
    // the number of edges counts for ingoing and outgoing edges separately
    // so this is already multiplied by 2
    // we use double as type, because in the division later on
    // we need doubles anyway
    double sumOfEdgeWeights = static_cast<double>(computeSumOfAllEdgeWeights(G));
    vector<EdgeWeight> edgeWeightsPerCluster;    // source and end node are in same cluster c
    vector<EdgeWeight> weightedEdgeEndsPerCluster;     // source nodes of edges in cluster c

    // compute the weighted number of edges and edge ends per cluster
    computeEdgeWeightsPerCluster(G, edgeWeightsPerCluster, weightedEdgeEndsPerCluster);

    forall_blocks(G, c)
    {
        double edgeFraction = static_cast<double>(edgeWeightsPerCluster.at(c)) / sumOfEdgeWeights;
        double edgeEndFraction = static_cast<double>(weightedEdgeEndsPerCluster.at(c)) / sumOfEdgeWeights;

        modularity += edgeFraction - edgeEndFraction * edgeEndFraction;
    } endfor

    return modularity;
}

double ModularityMetric::computeModularityBound(graph_access &G)
{
    double modularity = 0.0;
    // the number of edges counts for ingoing and outgoing edges separately
    // so this is already multiplied by 2
    // we use double as type, because in the division later on
    // we need doubles anyway
    double sumOfEdgeWeights = static_cast<double>(computeSumOfAllEdgeWeights(G));
    vector<EdgeWeight> edgeWeightsPerCluster;    // source and end node are in same cluster c
    vector<EdgeWeight> weightedEdgeEndsPerCluster;     // source nodes of edges in cluster c

    // compute the weighted number of edges and edge ends per cluster
    computeEdgeWeightsPerCluster(G, edgeWeightsPerCluster, weightedEdgeEndsPerCluster);

    forall_blocks(G, c)
    {
        double edgeEndFraction = static_cast<double>(weightedEdgeEndsPerCluster.at(c)) / sumOfEdgeWeights;

        modularity -=  edgeEndFraction * edgeEndFraction;
    } endfor

    return 1+modularity;
}

double ModularityMetric::computeModularitySlow(graph_access &G)
{
    double modularity = 0.0;
    // the number of edges counts for ingoing and outgoing edges separately
    // so this is already multiplied by 2
    // we use double as type, because in the division later on
    // we need doubles anyway
    double sumOfEdgeWeights = static_cast<double>(computeSumOfAllEdgeWeights(G));

    forall_nodes(G, v)
    {
        // we cast to double, because otherwise later on an integer multiplication
        // could cause an overflow for large values
        double vWeightedDegree = static_cast<double>(G.getWeightedNodeDegree(v));
        PartitionID vClusterIndex = G.getPartitionIndex(v);

        // self loops also count for node degree
        // but so far they are not considered for the node degree
        if (G.containsSelfLoops())
        {
            vWeightedDegree += static_cast<double>(G.getSelfLoop(v));
        }

        forall_nodes(G, w)
        {
            double wWeightedDegree = static_cast<double>(G.getWeightedNodeDegree(w));
            PartitionID wClusterIndex = G.getPartitionIndex(w);

            if (G.containsSelfLoops())
            {
                wWeightedDegree += static_cast<double>(G.getSelfLoop(w));
            }

            // we are only interested in the nodes in the same cluster
            if (vClusterIndex == wClusterIndex)
            {
                // if v and w are connected, then weight of out edge from v to w, otherwise 0.0
                double a = static_cast<double>(getWeightedOutEdgeToOtherNode(G, v, w));

                // here we multiply doubles to avoid an overview
                modularity += a - (vWeightedDegree * wWeightedDegree) / sumOfEdgeWeights;
            }
        } endfor
    } endfor

    modularity /= sumOfEdgeWeights;

    return modularity;
}


EdgeWeight ModularityMetric::computeSumOfAllEdgeWeights(graph_access& G)
{
    EdgeWeight sum = 0;

    forall_edges(G, e)
    {
        sum += G.getEdgeWeight(e);

    } endfor

    // self loops also count as edges
    // this is especially important for the coarser graphs
    if (G.containsSelfLoops())
    {
        forall_nodes(G, n)
        {
            sum += G.getSelfLoop(n);
        } endfor
    }

    return sum;
}


EdgeWeight ModularityMetric::getWeightedOutEdgeToOtherNode(graph_access& G, NodeID v, NodeID w)
{
    // take care for self loop
    if (v == w && G.containsSelfLoops())
    {
        return G.getSelfLoop(v);
    }
    // determine if v and w are connected
    // so far I know there is no better way than searching
    forall_out_edges(G, vOutEdge, v)
    {
        // are v and w connected?
        if (G.getEdgeTarget(vOutEdge) == w)
        {
            return G.getEdgeWeight(vOutEdge);
        }
    } endfor


    return 0;
}

