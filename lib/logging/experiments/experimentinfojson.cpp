#include "experimentinfojson.h"

using namespace std;

ExperimentInfoJson::ExperimentInfoJson()
    : m_algorithmType(),
    m_minimumQualityImprovement(-2.0),
    m_numberOfEdges(0),
    m_numberOfNodes(0),
    m_modularityBeforeClustering(-2.0),
    m_modularityAfterClustering(-2.0),
    m_weightedEdgeCut(0),
    m_numberOfClusters(0),
    m_numberOfLPIterations(0),
    m_clusterCoarseningFactor(0),
    m_timeForGraphInput(-1.0),
    m_timeForClustering(-1.0)
{
    //ctor
}

ExperimentInfoJson::~ExperimentInfoJson()
{
    //dtor
}


void ExperimentInfoJson::reportProgramStart(int argc, char** argv)
{
    m_hostName = getHostName();
    m_startTime = getCurrentDateTime();
    m_programArguments.assign(argv, argv + argc);
}


void ExperimentInfoJson::reportLPLevel(const double time,
                                       const EdgeID newNumberOfEdges,
                                       const NodeID newNumberOfNodes,
                                       const double newModularity)
{
    m_labelPropagationLevels.emplace_back(time, newModularity, newNumberOfEdges, newNumberOfNodes);
}


void ExperimentInfoJson::reportLouvainNodeMoves(const double time,
                                                const double newModularity)
{
    // as we do not know whether it belongs to a
    // Louvain level or refinement, we first add it
    // to the temporary list
    m_tmpSubLevels.emplace_back(time, newModularity);
}


void ExperimentInfoJson::reportLouvainLevel(const double time,
                                            const EdgeID newNumberOfEdges,
                                            const NodeID newNumberOfNodes,
                                            const double newModularity)
{
    m_louvainLevels.emplace_back(time, newModularity, newNumberOfEdges, newNumberOfNodes);

    // now we know that the temporary list of node moves belongs
    // to the new level (and not to the refinements)
    // so we can append the temporary list
    // and free the temporary list for the next level/refinement
    // (we free by moving empty list of last to it)
    LevelInfo &last = m_louvainLevels.back();
    m_tmpSubLevels.swap(last.m_subLevels);
}


void ExperimentInfoJson::reportLouvainRefinement(const double time,
                                                 const EdgeID newNumberOfEdges,
                                                 const NodeID newNumberOfNodes,
                                                 const double newModularity)
{
    m_louvainRefinements.emplace_back(time, newModularity, newNumberOfEdges, newNumberOfNodes);

    // now we know that the temporary list of node moves belongs
    // to the refinements (and not to a new level)
    // so we can append the temporary list
    // and free the temporary list for the next level/refinement
    // (we free by moving empty list of last to it)
    LevelInfo &last = m_louvainRefinements.back();
    m_tmpSubLevels.swap(last.m_subLevels);
}


void ExperimentInfoJson::writeInJsonFormat(std::ostream& os) const
{
    os << "{" << endl;
    os << "\t\"start_time\": \"" << m_startTime << "\"," << endl;
    os << "\t\"host_name\": \"" << m_hostName << "\"," << endl;

    // program arguments as array
    os << "\t\"program_arguments\": [ ";
    for (unsigned i = 0; i < m_programArguments.size(); ++i)
    {
        if (i > 0)
        {
            os << ", ";
        }
        os << "\"" << m_programArguments.at(i) << "\"";
    }
    os << "]," << endl;

    // important input parameters
    os << "\t\"input\": {" << endl;
    os << "\t\t\"algorithm_type\": " << m_algorithmType << endl;
    os << "\t}," << endl;

    // general infos about the graph as object
    os << "\t\"graph\": {" << endl;
    os << "\t\t\"file_name\": \"" << m_graphFileName << "\"," << endl;
    os << "\t\t\"number_of_nodes\": " << m_numberOfNodes << "," << endl;
    os << "\t\t\"number_of_edges\": " << m_numberOfEdges << "" << endl;
    os << "\t}," << endl;

    // label propagation
    os << "\t\"label_propagation\": {" << endl;
    os << "\t\t\"number_of_iterations\": " << m_numberOfLPIterations << "," << endl;
    os << "\t\t\"cluster_coarsening_factor\": " << m_clusterCoarseningFactor << "," << endl;
    os << "\t\t\"levels\": [" << endl;
    for (int i = 0, iEnd = m_labelPropagationLevels.size(); i < iEnd; ++i)
    {
        const LevelInfo &level = m_labelPropagationLevels.at(i);
        os << "\t\t\t{ \"time\": " << level.m_time << ", "
                   << "\"mod\": " << level.m_newModularity << ", "
                   << "\"#nodes\": " << level.m_newNumberOfNodes << ", "
                   << "\"#edges\": " << level.m_newNumberOfEdges << " }";

        if (i < iEnd - 1)
        {
            os << ",";
        }
        os << endl;
    }
    os << "\t\t]" << endl;
    os << "\t}," << endl;

    // Louvain
    os << "\t\"louvain\": {" << endl;
    os << "\t\t\"min_quality_impr\": " << m_minimumQualityImprovement << "," << endl;
    // levels
    os << "\t\t\"levels\": [" << endl;
    for (int i = 0, iEnd = m_louvainLevels.size(); i < iEnd; ++i)
    {
        const LevelInfo &level = m_louvainLevels.at(i);
        os << "\t\t\t{" << endl;
        os << "\t\t\t\t\"time\": " << level.m_time << ", "
                   << "\"mod\": " << level.m_newModularity << ", "
                   << "\"#nodes\": " << level.m_newNumberOfNodes << ", "
                   << "\"#edges\": " << level.m_newNumberOfEdges << ", " << endl;

        // node moves
        os << "\t\t\t\t\"node_moves\": [" << endl;
        for (int j = 0, jEnd = level.m_subLevels.size(); j < jEnd; ++j)
        {
            const LevelInfo &subLevel = level.m_subLevels.at(j);
            os << "\t\t\t\t\t{ \"time\": " << subLevel.m_time << ", "
                       << "\"mod\": " << subLevel.m_newModularity << " }";

            if (j < jEnd - 1)
            {
                os << ",";
            }
            os << endl;
        }
        os << "\t\t\t\t]" << endl;
        os << "\t\t\t}";

        // for the last one we do not want to have a comma
        if (i < iEnd - 1)
        {
            os << ",";
        }
        os << endl;
    }
    os << "\t\t]," << endl;
    // refinements
    os << "\t\t\"refinements\": [" << endl;
    for (int i = 0, iEnd = m_louvainRefinements.size(); i < iEnd; ++i)
    {
        const LevelInfo &level = m_louvainRefinements.at(i);
        os << "\t\t\t{" << endl;
        os << "\t\t\t\t\"time\": " << level.m_time << ", "
                   << "\"mod\": " << level.m_newModularity << ", "
                   << "\"#nodes\": " << level.m_newNumberOfNodes << ", "
                   << "\"#edges\": " << level.m_newNumberOfEdges << ", " << endl;

        // node moves
        os << "\t\t\t\t\"node_moves\": [" << endl;
        for (int j = 0, jEnd = level.m_subLevels.size(); j < jEnd; ++j)
        {
            const LevelInfo &subLevel = level.m_subLevels.at(j);
            os << "\t\t\t\t\t{ \"time\": " << subLevel.m_time << ", "
                       << "\"mod\": " << subLevel.m_newModularity << " }";

            if (j < jEnd - 1)
            {
                os << ",";
            }
            os << endl;
        }
        os << "\t\t\t\t]" << endl;
        os << "\t\t\t}";

        if (i < iEnd - 1)
        {
            os << ",";
        }
        os << endl;
    }
    os << "\t\t]" << endl;
    os << "\t}," << endl;

    // general time info
    os << "\t\"time\": {" << endl;
    os << "\t\t\"graphinput\": " << m_timeForGraphInput << "," << endl;
    os << "\t\t\"clustering\": " << m_timeForClustering << endl;
    os << "\t}," << endl;

    // result
    os << "\t\"result\": {" << endl;
    os << "\t\t\"modularity_before\": " << m_modularityBeforeClustering << "," << endl;
    os << "\t\t\"modularity_after\": " << m_modularityAfterClustering << "," << endl;
    os << "\t\t\"edge_cut\": " << m_weightedEdgeCut << "," << endl;
    os << "\t\t\"number_of_clusters\": " << m_numberOfClusters << endl;
    os << "\t}" << endl;

    os << "}" << endl;
}
