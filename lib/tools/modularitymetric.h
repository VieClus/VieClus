#ifndef MODULARITYMETRIC_H
#define MODULARITYMETRIC_H

#include "data_structure/graph_access.h"
#include <vector>


/**
 *  \brief Contains functions to calculate the modularity metric.
 *
 *  Static functions compute from scratch, member functions keep a
 *  state of the current clusters to calculate the modularity faster.
 *
 *  For weighted integer graphs.
 */
class ModularityMetric
{
    public:
        ModularityMetric(graph_access &G);
        virtual ~ModularityMetric();

        /**
         *  \brief Returns the modularity of the given graph clustering.
         *
         *  \return Modularity in the range [-1,1].
         */
        double quality() const;


        /**
         *  \brief Returns the modularity gain if node "node" would be added
         *  to cluster "cluster".
         *
         *  \param node Node for which the gain is asked.
         *  \param cluster Cluster to which "node" should be added.
         *  \param edgeWeightToCluster Weight of edge from "node" to "cluster".
         *
         *  \return Modularity gain in the range [-1,1].
         *
         *  Assumes that "node" is not already contained in "cluster".
         */
        double gain(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster) const;


        /**
         *  \brief Inserts node "node" to cluster "cluster".
         *
         *  \param node Node that is inserted.
         *  \param cluster Cluster to which "node" is added.
         *  \param edgeWeightToCluster Weight of edge from "node" to "cluster".
         *
         *  Updates the internal data structures
         *  Assumes that "node" is not contained in "cluster" before.
         */
        void insertNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster);


        /**
         *  \brief Inserts node "node" to cluster "cluster".
         *
         *  \param node Node that is inserted.
         *  \param cluster Cluster to which "node" is added.
         *  \param edgeWeightToCluster Weight of edge from "node" to "cluster".
         *  \param selfLoop Edge weight of self loop.
         *
         *  Updates the internal data structures
         *  Assumes that "node" is not contained in "cluster" before.
         */
        void insertNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster, EdgeWeight selfLoop);


        /**
         *  \brief Removes node "node" from cluster "cluster".
         *
         *  \param node Node that is inserted.
         *  \param cluster Cluster fro which "node" is removed.
         *  \param edgeWeightToCluster Weight of edge from "node" to "cluster".
         *
         *  Updates the internal data structures
         *  Assumes that "node" is contained in "cluster" before.
         *  Afterwards "node" belongs to cluster -1.
         */
        void removeNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster);


        /**
         *  \brief Removes node "node" from cluster "cluster".
         *
         *  \param node Node that is inserted.
         *  \param cluster Cluster fro which "node" is removed.
         *  \param edgeWeightToCluster Weight of edge from "node" to "cluster".
         *  \param selfLoop Edge weight of self loop.
         *
         *  Updates the internal data structures
         *  Assumes that "node" is contained in "cluster" before.
         *  Afterwards "node" belongs to cluster -1.
         */
        void removeNode(NodeID node, PartitionID cluster, EdgeWeight edgeWeightToCluster, EdgeWeight selfLoop);


        /**
         *  \brief Returns the modularity of the given graph clustering.
         *
         *  \param G Weighted graph to calculate the modularity for.
         *
         *  \return Modularity in the range [-1,1].
         *
         *  Implemented according to
         *  Aaron Clauset, M. E. J. Newman, Cristopher Moore -
         *  "Finding community structure in very large networks",
         *  Formula 7
         */
        static double computeModularity(graph_access &G);
        static double computeModularityBound(graph_access &G);
        static double computeModularity(graph_access &G, int * partition_map);


        /**
         *  \brief Returns the modularity of the given graph clustering.
         *
         *  \param G Weighted graph to calculate the modularity for.
         *
         *  \return Modularity in the range [-1,1].
         *
         *  Slow implementation which runtime depends quadratic on the number of nodes.
         *
         *  Implemented according to
         *  Aaron Clauset, M. E. J. Newman, Cristopher Moore -
         *  "Finding community structure in very large networks",
         *  Formula 4
         */
        static double computeModularitySlow(graph_access &G);


        /**
         *  \brief Returns the sum of all edge weights of the weighted graph G.
         *
         *  \param G Weighted graph which edges are summed up.
         *
         *  Considers also the self loops, if any.
         *  May cause an integer overflow, so be careful when the graph has large weights.
         */
        static EdgeWeight computeSumOfAllEdgeWeights(graph_access &G);

    protected:
        /**
            \brief Updates the data structure for the weighted node degrees.
         */
        void computeWeightedNodeDegrees();


        /**
         *  \brief Computes the edges weights and weighted edge end points per cluster.
         *
         *  \param G Weighted graph to evaluate.
         *  \param edgeWeightsPerCluster [out] Edge weights inside/per cluster.
         *          vector is resized inside to the proper number of clusters.
         *  \param weightedEdgeEndsPerCluster [out] Weighted edge end points per cluster.
         *          vector is resized inside to the proper number of clusters.
         *
         *  Considers also the self loops, if any.
         */
        static void computeEdgeWeightsPerCluster(graph_access &G,
                                                 std::vector<EdgeWeight> &edgeWeightsPerCluster,
                                                 std::vector<EdgeWeight> &weightedEdgeEndsPerCluster);


        /**
         *  \brief Returns self loop if v and w are the same, else returns out edge
         *  weight from v to w, if node v has an outgoing edge to w, otherwise 0.
         */
        static EdgeWeight getWeightedOutEdgeToOtherNode(graph_access &G, NodeID v, NodeID w);


        // maybe make with this members an own class
        /// Graph of which we keep internally the modularity to answer modularity gains fast.
        graph_access &m_G;
        /// Weight of edges inside/per cluster c. Source and target node are in the same cluster c. Size equal to cluster count.
        std::vector<EdgeWeight> m_edgeWeightsPerCluster;
        /// Weight of edge end points inside/per cluster c. Source node is in cluster c. Size equal to cluster count.
        std::vector<EdgeWeight> m_weightedEdgeEndsPerCluster;
        /// Weighted node degrees of current graph. We cache it, because the degree is calculated from scratch each time otherwise.
        std::vector<EdgeWeight> m_weightedNodeDegrees;
        /** Sum of all edge weights (also self loops). We cache it, as
            it does not change and is needed often (and graph_access has
            no such property.) */
        double m_sumOfAllEdgeWeights;

    private:
};




#endif // MODULARITYMETRIC_H
