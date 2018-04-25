/******************************************************************************
 * neighborhood.h
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


#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "data_structure/graph_access.h"
#include <vector>


/**
    \brief Contains information about the clusters in the local
    neighborhood of a node.
 */
class Neighborhood
{
    public:
        Neighborhood();
        virtual ~Neighborhood();


        /**
            \brief Resets the neighborhood data structures.

            \param G The new current graph we use.
         */
        void initialize(graph_access *G);


        /**
            \brief Computes the edge weights to the local clusters
            in the neighborhood of "node".

            \param node Current node for which the neighboring clusters are counted.
         */
        void update(NodeID node);


        /// Returns the total edge weight to all nodes in the neighborhood that are in "cluster".
        EdgeWeight getEdgeWeightToNeighboringCluster(PartitionID cluster) const;
        /// Returns the cluster ID of "neighbor".
        PartitionID getClusterIDOfNeighbor(NodeID neighbor) const;
        /// Returns the number of clusters in the current neighborhood.
        std::vector<EdgeWeight>::size_type getNumberOfNeighboringClusters() const;

    protected:
        /// Current graph that is evaluated.
        graph_access *m_G;
        /// Edge weights to the clusters in the neighborhood of the current node.
        std::vector<EdgeWeight> m_edgeWeightsToNeighboringClusters;
        /// Occurring cluster IDs in the neighborhood of the current node.
        std::vector<PartitionID> m_clusterIDsOfNeighbors;
        /// Cluster count in the neighborhood of the current node.
        std::vector<EdgeWeight>::size_type m_numberOfNeighboringClusters;

    private:
};


inline EdgeWeight Neighborhood::getEdgeWeightToNeighboringCluster(PartitionID cluster) const
{
#ifdef NDEBUG
    return m_edgeWeightsToNeighboringClusters[cluster];
#else
    return m_edgeWeightsToNeighboringClusters.at(cluster);
#endif // NDEBUG
}


inline PartitionID Neighborhood::getClusterIDOfNeighbor(NodeID neighbor) const
{
#ifdef NDEBUG
    return m_clusterIDsOfNeighbors[neighbor];
#else
    return m_clusterIDsOfNeighbors.at(neighbor);
#endif // NDEBUG
}


inline std::vector<EdgeWeight>::size_type Neighborhood::getNumberOfNeighboringClusters() const
{
    return m_numberOfNeighboringClusters;
}

#endif // NEIGHBORHOOD_H
