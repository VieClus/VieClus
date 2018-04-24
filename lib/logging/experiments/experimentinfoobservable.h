#ifndef EXPERIMENTINFOOBSERVABLE_H
#define EXPERIMENTINFOOBSERVABLE_H

#include "experimentinfo.h"

#include <set>


/**
    \brief Subject class of the observer pattern.

    Observers/listener can register and experiment infos are forwarded
    to them.
 */
class ExperimentInfoObservable : public ExperimentInfo
{
    public:
        ExperimentInfoObservable();
        virtual ~ExperimentInfoObservable();

        virtual void reportProgramStart(int argc, char **argv);
        virtual void reportGraphFile(const std::string &name);
        virtual void reportAlgorithmType(const unsigned type);
        virtual void reportMinimumQualityImprovement(const double minImprovement);
        virtual void reportNumberOfEdges(const EdgeID numberOfEdges);
        virtual void reportNumberOfNodes(const NodeID numberOfNodes);

        // infos regarding the result
        virtual void reportModularityBeforeClustering(const double modularity);
        virtual void reportModularityAfterClustering(const double modularity);
        virtual void reportWeightedEdgeCut(const EdgeWeight cut);
        virtual void reportNumberOfClusters(const PartitionID clusters);

        // infos about label propagation
        virtual void reportNumberOfLPIterations(const unsigned iterations);
        // fine time infos for each level
        // new number of nodes is number of clusters
        virtual void reportLPLevel(const double time,
                                   const EdgeID newNumberOfEdges,
                                   const NodeID newNumberOfNodes,
                                   const double newModularity);
        virtual void reportSCLPClusterCoarseningFactor(const unsigned clusterCoarseningFactor);

        // info about Louvain method
        // fine time infos for each level
        virtual void reportLouvainLevel(const double time,
                                        const EdgeID newNumberOfEdges,
                                        const NodeID newNumberOfNodes,
                                        const double newModularity);
        // one iteration in the first phase of the Louvain algorithm
        virtual void reportLouvainNodeMoves(const double time,
                                            const double newModularity);
        // during uncoarsening we perform refinement
        virtual void reportLouvainRefinement(const double time,
                                             const EdgeID newNumberOfEdges,
                                             const NodeID newNumberOfNodes,
                                             const double newModularity);

        // general time infos
        virtual void reportTimeForGraphInput(const double time);
        // total time for clustering (LP + Louvain)
        virtual void reportTimeForClustering(const double time);

        void registerObserver(ExperimentInfo *observer);
        void unregisterObserver(ExperimentInfo *observer);

    protected:
        /// List of observers that notified about the different infos
        std::set<ExperimentInfo *> m_observers;

    private:
};

#endif // EXPERIMENTINFOOBSERVABLE_H
