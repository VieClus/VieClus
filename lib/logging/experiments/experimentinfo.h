#ifndef EXPERIMENTINFO_H
#define EXPERIMENTINFO_H

#include "definitions.h"    // NodeID, EdgeID

#include <memory>           // shared_ptr

/**
    \brief Base class for reporting relevant experiments data.
 */
class ExperimentInfo
{
    public:
        ExperimentInfo();
        virtual ~ExperimentInfo();

        /**
            Returns the hostname of the computer where the program was started.

            This function works only on UNIX properly.
        */
        static std::string getHostName();

        /**
            Returns the current wall time in seconds.

            A Simple timer for measuring the walltime under UNIX (gettimeofday).
            A WINDOWS version can be found in log.h:NowTime().

            This function works only on UNIX properly
        */
        static double wallTime();

        /**
            \brief Returns current date and time as string.

            ISO 8601 format with ' ' instead of 'T' between date
            and time and without "+01" for CEST.
            Uses local time.
         */
        static std::string getCurrentDateTime();

        // static infos independent of the result
        virtual void reportProgramStart(int argc, char **argv) { };
        virtual void reportGraphFile(const std::string &name)  { };
        virtual void reportAlgorithmType(const unsigned type)  { };
        virtual void reportMinimumQualityImprovement(const double minImprovement){ };
        virtual void reportNumberOfEdges(const EdgeID numberOfEdges) { };
        virtual void reportNumberOfNodes(const NodeID numberOfNodes) { };

        // infos regarding the result
        virtual void reportModularityBeforeClustering(const double modularity) { };
        virtual void reportModularityAfterClustering(const double modularity) { };
        virtual void reportWeightedEdgeCut(const EdgeWeight cut) { };
        virtual void reportNumberOfClusters(const PartitionID clusters) { };

        // infos about label propagation
        virtual void reportNumberOfLPIterations(const unsigned iterations) { };
        // fine time infos for each level
        // new number of nodes is number of clusters
        virtual void reportLPLevel(const double time,
                                   const EdgeID newNumberOfEdges,
                                   const NodeID newNumberOfNodes,
                                   const double newModularity) { };
        // size constrained label propagation
        // we use it reverse (numberOfNodes / clusterUpperBound) is the max cluster size
        virtual void reportSCLPClusterCoarseningFactor(const unsigned clusterCoarseningFactor) { };

        // info about Louvain method
        // fine time infos for each level
        virtual void reportLouvainLevel(const double time,
                                        const EdgeID newNumberOfEdges,
                                        const NodeID newNumberOfNodes,
                                        const double newModularity) { };
        // one iteration in the first phase of the Louvain algorithm
        virtual void reportLouvainNodeMoves(const double time,
                                            const double newModularity) { };
        // during uncoarsening we perform refinement
        virtual void reportLouvainRefinement(const double time,
                                             const EdgeID newNumberOfEdges,
                                             const NodeID newNumberOfNodes,
                                             const double newModularity) { };

        // general time infos
        virtual void reportTimeForGraphInput(const double time) { };
        // total time for clustering (LP + Louvain)
        virtual void reportTimeForClustering(const double time) { };


        static std::shared_ptr<ExperimentInfo> INSTANCE;
    protected:

    private:
};

#endif // EXPERIMENTINFO_H
