/******************************************************************************
 * diversifyer.h 
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 *****************************************************************************/

#ifndef DIVERSIFYERCLUSTERING_AZQIF42R
#define DIVERSIFYERCLUSTERING_AZQIF42R

#include "random_functions.h"

class diversifyer_clustering {
public:
        diversifyer_clustering() {} ;
        virtual ~diversifyer_clustering() {};

        void diversify(PartitionConfig & config) {
                //diversify edge rating:
                config.edge_rating                   = (EdgeRating)random_functions::nextInt(0, (unsigned)EXPANSIONSTAR2ALGDIST);
                config.permutation_quality           = PERMUTATION_QUALITY_GOOD;
                config.permutation_during_refinement = PERMUTATION_QUALITY_GOOD;
        }
};


#endif /* end of include guard: DIVERSIFYER_AZQIF42R */
