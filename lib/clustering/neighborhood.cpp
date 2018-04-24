#include "neighborhood.h"

using namespace std;

Neighborhood::Neighborhood()
    : m_G(0), m_numberOfNeighboringClusters(0)
{
    //ctor
}


Neighborhood::~Neighborhood()
{
    //dtor
}


void Neighborhood::initialize(graph_access* G)
{
    // update the graph we are working on
    m_G = G;

    // in the worst case a node has edges to all other nodes
    // this is also the maximum number of clusters
    // resize() is not enough, because we also want to have
    // -1 as content
    m_edgeWeightsToNeighboringClusters.assign(m_G->get_partition_count_compute(), -1);
    // here the old content may remain,
    // because new content is set explicitly
    m_clusterIDsOfNeighbors.resize(m_G->get_partition_count_compute(), -1);
    m_numberOfNeighboringClusters = 0;
}


void Neighborhood::update(NodeID node)
{
    // reset the neighborhood of the previous node
    for (vector<EdgeWeight>::size_type i = 0; i < m_numberOfNeighboringClusters; ++i)
    {
        m_edgeWeightsToNeighboringClusters[m_clusterIDsOfNeighbors[i]] = -1;
    }
    m_numberOfNeighboringClusters = 0;

    // we also have to store the info about the node itself
    m_clusterIDsOfNeighbors[0] = m_G->getPartitionIndex(node);
    m_edgeWeightsToNeighboringClusters[m_clusterIDsOfNeighbors[0]] = 0;
    m_numberOfNeighboringClusters++;

    // we count the cluster sizes in the neighborhood
    forall_out_edges((*m_G), e, node)
    {
        NodeID neighboringNode = m_G->getEdgeTarget(e);
        PartitionID clusterIDOfNeighbor = m_G->getPartitionIndex(neighboringNode);
        EdgeWeight weightToNeighbor = m_G->getEdgeWeight(e);

        // is this the first neighbor with such a cluster ID?
        //if (m_edgeWeightsToNeighboringClusters[clusterIDOfNeighbor] == -1)
        if (m_edgeWeightsToNeighboringClusters.at(clusterIDOfNeighbor) == -1)
        {
            m_edgeWeightsToNeighboringClusters[clusterIDOfNeighbor] = 0;
            m_clusterIDsOfNeighbors[m_numberOfNeighboringClusters] = clusterIDOfNeighbor;
            m_numberOfNeighboringClusters++;
        }

        m_edgeWeightsToNeighboringClusters[clusterIDOfNeighbor] += weightToNeighbor;
    }
    endfor
}
