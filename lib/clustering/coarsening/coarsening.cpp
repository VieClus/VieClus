/******************************************************************************
 * coarsening.cpp
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


#include "coarsening.h"

#include "clustering/coarsening/contractor.h"
#include "partition/coarsening/contraction.h"
#include "tools/modularitymetric.h"

using namespace std;

Coarsening::Coarsening()
{
    //ctor
}

Coarsening::~Coarsening()
{
    //dtor
}


void Coarsening::buildClusterIDLookUpTable(graph_access &G,
                                           unordered_map<PartitionID, PartitionID>& clusterIDLookUp)
{
    PartitionID clusterIDCounter = 0;
    clusterIDLookUp.clear();

    forall_nodes(G, n)
    {
        PartitionID cluster = G.getPartitionIndex(n);
        pair<unordered_map<PartitionID, PartitionID>::iterator, bool> ret;

        ret = clusterIDLookUp.insert(make_pair(cluster, clusterIDCounter));

        // newly inserted?
        if (ret.second)
        {
            // then we have to increase the counter
            clusterIDCounter++;
        }
    } endfor
}


void Coarsening::buildCoarseMapping(graph_access &G,
                                    const std::unordered_map<PartitionID, PartitionID>& clusterIDLookUp,
                                    CoarseMapping &coarseMapping,
                                    std::vector<std::vector<NodeID> > &reverseCoarseMapping)
{
    // resize to number of nodes
    coarseMapping.assign(G.number_of_nodes(), -1);
    // clear data from before
    // resize() alone is not enough, because
    // it just makes the vector smaller
    // but leaves the content
    // resize to number of clusters
    reverseCoarseMapping.assign(clusterIDLookUp.size(), vector<NodeID>());

    // build mapping "node n is part of cluster c"
    // and reverse mapping "cluster c consists of nodes..."
    forall_nodes(G, n)
    {
        /// old cluster in the fine graph
        PartitionID cluster = G.getPartitionIndex(n);
        PartitionID newCluster = clusterIDLookUp.at(cluster);

        coarseMapping[n] = newCluster;
        reverseCoarseMapping[newCluster].push_back(n);
        // update also to the new cluster ID in fine graph
        G.setPartitionIndex(n, newCluster);
    } endfor
}


graph_access * Coarsening::performCoarsening(const PartitionConfig &config,
                                             graph_access &G,
                                             graph_hierarchy& graphHierarchy,
                                             std::list<graph_access*>& coarseGraphsToDelete)
{
    /// current coarse graph in a turn
    graph_access *coarseGraph = 0;
    /// current mapping of the nodes in the fine graph
    /// to the cluster/nodes in the coarse graph
    CoarseMapping *coarseMapping = 0;
    /// build the coarse graph based on the clusters in the fine graph
    /// KaHIP version
    // contraction contractor;
    /// number of node moves between clusters
    /// in the current iteration for the 1. phase of the algorithm
    /// look-up table where key is the old cluster ID
    /// and value is the new cluster ID in the consecutive range [0, clusterCount-1]
    unordered_map<PartitionID, PartitionID> clusterIDLookUp;
    /// current list of nodes for each cluster
    /// inner vector is a list of nodes, that belong to cluster c
    /// outer vector represents clusters
    vector<vector<NodeID> > reverseCoarseMapping;

    coarseGraph = new graph_access();
    coarseGraphsToDelete.push_back(coarseGraph);
    coarseMapping = new CoarseMapping();

    // the courseMapping may only contain values
    // from 0 till clusterCount-1
    // therefore we compute a new range
    Coarsening::buildClusterIDLookUpTable(G, clusterIDLookUp);

    // build mapping "node n is part of cluster c"
    // and reverse mapping "cluster c consists of nodes..."
    // we could combine buildClusterIDLookUpTable()
    // and buildCoarseMapping() and would just need
    // to go once through all nodes
    Coarsening::buildCoarseMapping(G, clusterIDLookUp, *coarseMapping, reverseCoarseMapping);

    G.set_partition_count(reverseCoarseMapping.size());

    // build coarse graph with KaHIP, but without self loops
    // parameters matching and nodePermutationMap
    // are actually not needed in the function
    //contractor.contract_clustering(config, *G, *coarseGraph, matching1,
    //                               *coarseMapping, reverseCoarseMapping.size(), nodePermutationMap);

    // build coarse graph with self loops
    Contractor::contractClustering(config, G, *coarseGraph, *coarseMapping, reverseCoarseMapping);

    graphHierarchy.push_back(&G, coarseMapping);

    return coarseGraph;
}
