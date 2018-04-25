/******************************************************************************
 * neighborhood.cpp
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
