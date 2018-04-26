VieClus v1.0 
=====

The graph clustering framework VieClus -- Vienna Graph Clustering.

Graph clustering is the problem of detecting tightly connected regions of a
graph. Depending on the task, knowledge about the structure of the graph can
reveal information such as voter behavior, the formation of new trends, existing
terrorist groups and recruitment~\cite{survey} or a natural partitioning of
data records onto pages~\cite{cluster-paging}. Further application areas
include the study of protein interaction~\cite{cluster-protein}, gene
expression networks~\cite{cluster-geneexp}, fraud
detection~\cite{cluster-anomalies}, program optimization~\cite{cluster-opt1,cluster-opt2} and the spread of
epidemics~\cite{cluster-epidemic}---possible applications are plentiful, as
almost all systems containing interacting or coexisting entities can be modeled
as a graph. 


Here we release a memetic algorithm, VieClus (Vienna Graph Clustering), to tackle the graph clustering problem. 
A key component of our contribution are natural recombine operators that employ ensemble clusterings as well as multi-level techniques. 
In machine learning, ensemble methods combine multiple weak classification (or clustering) algorithms to obtain a strong algorithm for classification (or clustering).
More precisely, given a number of clusterings, the \emph{overlay/ensemble clustering} is a clustering in which two vertices belong to the same cluster if and only if they belong to the same cluster in each of the input clusterings. 
Our recombination operators use the overlay of two clusterings from the population to decide whether pairs of vertices should belong to the same cluster~\cite{OvelgoenneG13ensemble,staudtmeyerhenke13high}.
This is combined with a local search algorithm to find further improvements and also embedded into a multi-level algorithm to find even better clusterings.
Our general principle is to randomize tie-breaking whenever possible. This diversifies the search and also improves solutions.
Lastly, we combine these techniques with a scalable communication protocol, producing a system that is able to compute high-quality solutions in a short amount of time.
In our experimental evaluation, we show that our algorithm successfully improves or reproduces all entries of the 10th DIMACS implementation~challenge under consideration in a small amount of time. In fact, for most of the small instances, we can improve the old benchmark result \emph{in less than a minute}.
Moreover, while the previous best result for different instances has been computed by a variety of solvers, our algorithm can now be used as a single tool to compute the result.
For more details, we refer the reader to~\cite{clusteringpaper}.

## Main project site:
http://viem.taa.univie.ac.at

Installation Notes
=====

Before you can start you need to install the following software packages:

- Scons (http://www.scons.org/)
- Argtable (http://argtable.sourceforge.net/)
- OpenMPI (http://www.open-mpi.de/). Note: due to removed progress threads in OpenMPI > 1.8, please use an OpenMPI version < 1.8 or Intel MPI to obtain a scalable parallel algorithm.

Once you installed the packages, just type ./compile.sh. Once you did that you can try to run the following command:

mpirun -n 2 ./deploy/vieclus examples/astro-ph.graph --time_limit=60

For a description of the graph format please have a look into the manual.


