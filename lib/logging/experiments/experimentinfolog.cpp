#include "experimentinfolog.h"

#include "logging/log.h"    // FILE_LOG()
#include <iostream>
#include <sstream>      // ostringstream

using namespace std;

ExperimentInfoLog::ExperimentInfoLog()
{
    //ctor
}

ExperimentInfoLog::~ExperimentInfoLog()
{
    //dtor
}


void ExperimentInfoLog::reportProgramStart(int argc, char** argv)
{
    ostringstream oss;

    oss << "Program started @'" << getHostName() << "' at " << getCurrentDateTime() << ".";

    this->print(oss.str());
}


void ExperimentInfoLog::reportGraphFile(const std::string& name)
{
    ostringstream oss;

    oss << "Name of graph file: " << name;

    this->print(oss.str());
}


void ExperimentInfoLog::reportAlgorithmType(const unsigned type)
{
    ostringstream oss;

    oss << "Algorithm type: " << type;

    this->print(oss.str());
}


void ExperimentInfoLog::reportMinimumQualityImprovement(const double minImprovement)
{
    ostringstream oss;

    oss << "Min. Quality Improvement: " << minImprovement;

    this->print(oss.str());
}


void ExperimentInfoLog::reportNumberOfEdges(const EdgeID numberOfEdges)
{
    ostringstream oss;

    oss << "Number of edges: " << numberOfEdges;

    this->print(oss.str());
}


void ExperimentInfoLog::reportNumberOfNodes(const NodeID numberOfNodes)
{
    ostringstream oss;

    oss << "Number of nodes: " << numberOfNodes;

    this->print(oss.str());
}


void ExperimentInfoLog::reportModularityBeforeClustering(const double modularity)
{
    ostringstream oss;

    oss << "Modularity before: " << modularity;

    this->print(oss.str());
}



void ExperimentInfoLog::reportModularityAfterClustering(const double modularity)
{
    ostringstream oss;

    oss << "Modularity after: " << modularity;

    this->print(oss.str());
}


void ExperimentInfoLog::reportWeightedEdgeCut(const EdgeWeight cut)
{
    ostringstream oss;

    oss << "Weighted edge cut: " << cut;

    this->print(oss.str());
}


void ExperimentInfoLog::reportNumberOfClusters(const PartitionID clusters)
{
    ostringstream oss;

    oss << "Number of clusters: " << clusters;

    this->print(oss.str());
}

void ExperimentInfoLog::reportNumberOfLPIterations(const unsigned iterations)
{
    ostringstream oss;

    oss << "Number of LP iterations: " << iterations;

    this->print(oss.str());
}


void ExperimentInfoLog::reportLPLevel(const double time,
                                      const EdgeID newNumberOfEdges,
                                      const NodeID newNumberOfNodes,
                                      const double newModularity)
{
    ostringstream oss;

    oss << "Label prop. level:\tt: " << time
        << "\tmod: " << newModularity
        << "\t#nodes: " << newNumberOfNodes
        << "\t#edges: " << newNumberOfEdges;

    this->print(oss.str());
}


void ExperimentInfoLog::reportSCLPClusterCoarseningFactor(const unsigned clusterCoarseningFactor)
{
    ostringstream oss;

    oss << "Cluster coarsening factor: " << clusterCoarseningFactor;

    this->print(oss.str());
}


void ExperimentInfoLog::reportLouvainLevel(const double time,
                                           const EdgeID newNumberOfEdges,
                                           const NodeID newNumberOfNodes,
                                           const double newModularity)
{
    ostringstream oss;

    oss << "Louvain level:\t\tt: " << time
        << "\tmod: " << newModularity
        << "\t#nodes: " << newNumberOfNodes
        << "\t#edges: " << newNumberOfEdges;

    this->print(oss.str());
}


void ExperimentInfoLog::reportLouvainNodeMoves(const double time,
                                               const double newModularity)
{
    ostringstream oss;

    oss << "Louvain node moves:\tt: " << time
        << "\tmod: " << newModularity;

    this->print(oss.str());
}


void ExperimentInfoLog::reportLouvainRefinement(const double time,
                                                const EdgeID newNumberOfEdges,
                                                const NodeID newNumberOfNodes,
                                                const double newModularity)
{
    ostringstream oss;

    oss << "Louvain refinement:\tt: " << time
        << "\tmod: " << newModularity
        << "\t#nodes: " << newNumberOfNodes
        << "\t#edges: " << newNumberOfEdges;

    this->print(oss.str());
}


void ExperimentInfoLog::reportTimeForGraphInput(const double time)
{
    ostringstream oss;

    oss << "Time for reading graph file: " << time;

    this->print(oss.str());
}


void ExperimentInfoLog::reportTimeForClustering(const double time)
{
    ostringstream oss;

    oss << "Time for clustering: " << time;

    this->print(oss.str());
}





void ExperimentInfoLog::print(const string& msg) const
{
    FILE_LOG(logINFO) << msg;
}
