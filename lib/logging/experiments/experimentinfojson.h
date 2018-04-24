#ifndef EXPERIMENTINFOJSON_H
#define EXPERIMENTINFOJSON_H

#include "experimentinfo.h"

#include <ostream>
#include <string>
#include <vector>

/**
    \brief Information about the computation of a new level
    (coarser graph) with label propagation or Louvain method.
 */
class LevelInfo
{
    public:
        LevelInfo(const double time,
                  const double newModularity,
                  const EdgeID newNumberOfEdges = 0,
                  const NodeID newNumberOfNodes = 0)
                  : m_time(time),
                    m_newNumberOfEdges(newNumberOfEdges),
                    m_newNumberOfNodes(newNumberOfNodes),
                    m_newModularity(newModularity) { };
        virtual ~LevelInfo() { };

        double m_time;
        EdgeID m_newNumberOfEdges;
        NodeID m_newNumberOfNodes;
        double m_newModularity;
        /**
            Contains finer sub levels part of a Louvain
            level or refinement.
        */
        std::vector<LevelInfo> m_subLevels;
};


/**
    \brief Stores experiment data and writes it in JSON format.

    Perhaps split this class, when further formats (f.e. XML) are implemented
    into a class that stores the information and a class that does the output
    in different formats (JSON, XML...)
 */
class ExperimentInfoJson : public ExperimentInfo
{
    public:
        ExperimentInfoJson();
        virtual ~ExperimentInfoJson();

        virtual void reportProgramStart(int argc, char **argv);
        virtual void reportGraphFile(const std::string &name) { m_graphFileName = name; };
        virtual void reportAlgorithmType(const unsigned type) {m_algorithmType = type; };
        virtual void reportMinimumQualityImprovement(const double minImprovement) {m_minimumQualityImprovement = minImprovement; };
        virtual void reportNumberOfEdges(const EdgeID numberOfEdges) { m_numberOfEdges = numberOfEdges; };
        virtual void reportNumberOfNodes(const NodeID numberOfNodes) { m_numberOfNodes = numberOfNodes; };

        // infos regarding the result
        virtual void reportModularityBeforeClustering(const double modularity) { m_modularityBeforeClustering = modularity; };
        virtual void reportModularityAfterClustering(const double modularity) { m_modularityAfterClustering = modularity; };
        virtual void reportWeightedEdgeCut(const EdgeWeight cut) { m_weightedEdgeCut = cut; };
        virtual void reportNumberOfClusters(const PartitionID clusters) { m_numberOfClusters = clusters; };

        // infos about label propagation
        virtual void reportNumberOfLPIterations(const unsigned iterations) { m_numberOfLPIterations = iterations; };
        // fine time infos for each level
        // new number of nodes is number of clusters
        virtual void reportLPLevel(const double time,
                                   const EdgeID newNumberOfEdges,
                                   const NodeID newNumberOfNodes,
                                   const double newModularity);
        virtual void reportSCLPClusterCoarseningFactor(const unsigned clusterCoarseningFactor) { m_clusterCoarseningFactor = clusterCoarseningFactor; };

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
        virtual void reportTimeForGraphInput(const double time) { m_timeForGraphInput = time; };
        // total time for clustering (LP + Louvain)
        virtual void reportTimeForClustering(const double time) { m_timeForClustering = time; };

        /// Writes all the information stored in this class to the stream os in JSON format
        void writeInJsonFormat(std::ostream &os) const;

    protected:
        // members to save the information
        std::vector<std::string> m_programArguments;
        std::string m_hostName;
        std::string m_startTime;
        std::string m_graphFileName;
        unsigned m_algorithmType;
        double m_minimumQualityImprovement;
        EdgeID m_numberOfEdges;
        NodeID m_numberOfNodes;

        // results
        double m_modularityBeforeClustering;
        double m_modularityAfterClustering;
        EdgeWeight m_weightedEdgeCut;
        PartitionID m_numberOfClusters;

        // label propagation
        unsigned m_numberOfLPIterations;
        std::vector<LevelInfo> m_labelPropagationLevels;
        unsigned m_clusterCoarseningFactor;

        // Louvain
        std::vector<LevelInfo> m_louvainLevels;
        std::vector<LevelInfo> m_louvainRefinements;
        // we use this temporary list for the sub levels
        // in advance we do not know whether a node move
        // belongs to a Louvain level or to the refinement
        std::vector<LevelInfo> m_tmpSubLevels;

        // general time
        double m_timeForGraphInput;
        double m_timeForClustering;

    private:
};

#endif // EXPERIMENTINFOJSON_H
