#include "experimentinfoobservable.h"

ExperimentInfoObservable::ExperimentInfoObservable()
{
    //ctor
}

ExperimentInfoObservable::~ExperimentInfoObservable()
{
    //dtor
}


void ExperimentInfoObservable::reportProgramStart(int argc, char** argv)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportProgramStart(argc, argv);
    }
}


void ExperimentInfoObservable::reportGraphFile(const std::string& name)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportGraphFile(name);
    }
}


void ExperimentInfoObservable::reportAlgorithmType(const unsigned type)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportAlgorithmType(type);
    }
}


void ExperimentInfoObservable::reportMinimumQualityImprovement(const double minImprovement)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportMinimumQualityImprovement(minImprovement);
    }
}


void ExperimentInfoObservable::reportNumberOfEdges(const EdgeID numberOfEdges)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportNumberOfEdges(numberOfEdges);
    }
}


void ExperimentInfoObservable::reportNumberOfNodes(const NodeID numberOfNodes)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportNumberOfNodes(numberOfNodes);
    }
}


void ExperimentInfoObservable::reportModularityBeforeClustering(const double modularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportModularityBeforeClustering(modularity);
    }
}


void ExperimentInfoObservable::reportModularityAfterClustering(const double modularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportModularityAfterClustering(modularity);
    }
}


void ExperimentInfoObservable::reportWeightedEdgeCut(const EdgeWeight cut)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportWeightedEdgeCut(cut);
    }
}


void ExperimentInfoObservable::reportNumberOfClusters(const PartitionID clusters)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportNumberOfClusters(clusters);
    }
}


void ExperimentInfoObservable::reportNumberOfLPIterations(const unsigned iterations)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportNumberOfLPIterations(iterations);
    }
}


void ExperimentInfoObservable::reportLPLevel(const double time,
                                             const EdgeID newNumberOfEdges,
                                             const NodeID newNumberOfNodes,
                                             const double newModularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportLPLevel(time, newNumberOfEdges, newNumberOfNodes, newModularity);
    }
}


void ExperimentInfoObservable::reportSCLPClusterCoarseningFactor(const unsigned clusterCoarseningFactor)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportSCLPClusterCoarseningFactor(clusterCoarseningFactor);
    }
}


void ExperimentInfoObservable::reportLouvainLevel(const double time,
                                                  const EdgeID newNumberOfEdges,
                                                  const NodeID newNumberOfNodes,
                                                  const double newModularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportLouvainLevel(time, newNumberOfEdges, newNumberOfNodes, newModularity);
    }
}


void ExperimentInfoObservable::reportLouvainNodeMoves(const double time,
                                                      const double newModularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportLouvainNodeMoves(time, newModularity);
    }
}


void ExperimentInfoObservable::reportLouvainRefinement(const double time,
                                                       const EdgeID newNumberOfEdges,
                                                       const NodeID newNumberOfNodes,
                                                       const double newModularity)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportLouvainRefinement(time, newNumberOfEdges, newNumberOfNodes, newModularity);
    }
}


void ExperimentInfoObservable::reportTimeForGraphInput(const double time)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportTimeForGraphInput(time);
    }
}


void ExperimentInfoObservable::reportTimeForClustering(const double time)
{
    for (ExperimentInfo *observer : m_observers)
    {
        observer->reportTimeForClustering(time);
    }
}


void ExperimentInfoObservable::registerObserver(ExperimentInfo* observer)
{
    m_observers.insert(observer);
}


void ExperimentInfoObservable::unregisterObserver(ExperimentInfo* observer)
{
    m_observers.erase(observer);
}
