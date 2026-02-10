/******************************************************************************
 * vieclus_interface.h
 *
 * Source of VieClus -- Vienna Graph Clustering
 *****************************************************************************/

#ifndef VIECLUS_INTERFACE_H
#define VIECLUS_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

const int VIECLUS_FAST   = 0;
const int VIECLUS_ECO    = 1;
const int VIECLUS_STRONG = 2;

// Graph clustering using VieClus.
// Uses METIS CSR format (same as KaHIP).
//
// Input:
//   n             - number of nodes
//   vwgt          - node weights (array of n ints, NULL for unit weights)
//   xadj          - CSR index array (array of n+1 ints)
//   adjcwgt       - edge weights (array of m ints, NULL for unit weights)
//   adjncy        - CSR adjacency array (array of m ints)
//   suppress_output - if true, suppress console output
//   seed          - random seed
//   mode          - VIECLUS_FAST (0), VIECLUS_ECO (1), or VIECLUS_STRONG (2)
//   time_limit    - time limit in seconds
//   cluster_upperbound - max cluster size (0 = no limit)
//
// Output:
//   modularity    - modularity of the computed clustering
//   num_clusters  - number of clusters found
//   clustering    - cluster assignment array (array of n ints, must be pre-allocated)
void vieclus_clustering(int* n, int* vwgt, int* xadj,
                        int* adjcwgt, int* adjncy,
                        bool suppress_output, int seed, int mode,
                        double time_limit, int cluster_upperbound,
                        double* modularity, int* num_clusters, int* clustering);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: VIECLUS_INTERFACE_H */
