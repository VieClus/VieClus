/******************************************************************************
 * label_propagation.h
 *
 * Source of VieClus -- Vienna Graph Clustering 
 *****************************************************************************/

#pragma once

#include "data_structure/graph_access.h"
#include "definitions.h"
#include "tools/random_functions.h"
// #include "common/logger.h"

#include <cstdlib>
#include <cstdint>


class label_propagation {
    static constexpr bool debug = false;
public:
    label_propagation() {};
    virtual ~label_propagation() {};

    void propagate_labels(graph_access & G,
                          std::vector<NodeID> & cluster_id,
                          std::vector<std::vector<NodeID>> & reverse_mapping) {
        // in this case the _matching paramter is not used
        // coarse_mappng stores cluster id and the mapping (it is identical)
        std::vector<NodeID> permutation(G.number_of_nodes());
        cluster_id.resize(G.number_of_nodes());
        random_functions::setSeed(time(NULL));
        std::vector<PartitionID> hash_map(G.number_of_nodes(),0);

        for (size_t i = 0; i < cluster_id.size(); ++i) {
            cluster_id[i] = i;
        }

        //node_ordering n_ordering;
        //n_ordering.order_nodes(partition_config, G, permutation);
        random_functions::permutate_vector_fast(permutation, true);

        int iterations = 3;

        //LOG << "number of iterations: " << iterations;

        timer t;


        for( int j = 0; j < iterations; j++) {

            unsigned int change_counter = 0;

            forall_nodes(G, node) {
                NodeID n = permutation[node];
                //now move the node to the cluster that is most common in the neighborhood

                forall_out_edges(G, e, n) {
                    NodeID target = G.getEdgeTarget(e);
                    hash_map[cluster_id[target]]+=G.getEdgeWeight(e);
                } endfor

                //second sweep for finding max and resetting array
                PartitionID max_block = cluster_id[n];
                //PartitionID my_block  = cluster_id[node];

                PartitionID max_value = 0;
                forall_out_edges(G, e, n) {
                    NodeID target             = G.getEdgeTarget(e);
                    PartitionID cur_block     = cluster_id[target];
                    PartitionID cur_value     = hash_map[cur_block];
                    if(cur_value > max_value || (cur_value == max_value &&
                                                  random_functions::nextBool()))
                    {
                        max_value = cur_value;
                        max_block = cur_block;
                    }

                    hash_map[cur_block] = 0;
                } endfor

                change_counter += (cluster_id[n] != max_block);
                cluster_id[n] = max_block;
            } endfor
//            LOG << "Iteration " << j << ": Changed " <<
//                change_counter << " node positions";
//            LOG << "timer: " << t.elapsed();
        }

        remap_cluster_ids(G, cluster_id, reverse_mapping);
    }

    void remap_cluster_ids(
        graph_access & G,
        std::vector<NodeID> & cluster_id,
        std::vector<std::vector<NodeID>> & reverse_mapping) {

        PartitionID cur_no_clusters = 0;
        std::unordered_map<PartitionID, PartitionID> remap;

        std::vector<NodeID> part(G.number_of_nodes(), UNDEFINED_NODE);

        forall_nodes(G, node) {
            PartitionID cur_cluster = cluster_id[node];
            //check wether we already had that
            if( part[cur_cluster] == UNDEFINED_NODE) {
                part[cur_cluster] = cur_no_clusters++;
                reverse_mapping.emplace_back();
            }

            cluster_id[node] = part[cur_cluster];
            reverse_mapping[part[cur_cluster]].push_back(node);
        } endfor
    }
};
