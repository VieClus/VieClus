VieClus v1.2
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PyPI](https://img.shields.io/pypi/v/vieclus)](https://pypi.org/project/vieclus/)
=====

The graph clustering framework VieClus -- Vienna Graph Clustering.

Graph clustering is the problem of detecting tightly connected regions of a
graph. Depending on the task, knowledge about the structure of the graph can
reveal information such as voter behavior, the formation of new trends, existing
terrorist groups and recruitment or a natural partitioning of
data records onto pages. Further application areas
include the study of protein interaction, gene
expression networks, fraud
detection, program optimization and the spread of
epidemics---possible applications are plentiful, as
almost all systems containing interacting or coexisting entities can be modeled
as a graph. 



This is the release of our memetic algorithm, VieClus (Vienna Graph Clustering), to tackle the graph clustering problem. 
A key component of our contribution are natural recombine operators that employ ensemble clusterings as well as multi-level techniques. 
In our experimental evaluation, we show that **our algorithm successfully improves or reproduces all entries of the 10th DIMACS implementation challenge** under consideration in a small amount of time. In fact, for most of the small instances, we can improve the old benchmark result in less than a minute.
Moreover, while the previous best result for different instances has been computed by a variety of solvers, our algorithm can now be used as a single tool to compute the result. **In short our solver is the currently best modularity based clustering algorithm available.**

<p align="center">
<img src="./img/example_clustering.png"
  alt="example clustering"
  width="538" height="468">
</p>

Installation Notes
=====

Before you can start you need to install the following software packages:

- Argtable (http://argtable.sourceforge.net/)
- OpenMPI (http://www.open-mpi.org/). Note: due to removed progress threads in OpenMPI > 1.8, please use an OpenMPI version < 1.8 or Intel MPI to obtain a scalable parallel algorithm.

Once you installed the packages, just type 
``./compile_withcmake.sh``

To compile without MPI support (using pseudo_mpi.h), use:
``./compile_withcmake.sh NOMPI``

**Note:** MPI support is preferred as it enables the parallel algorithm which typically yields better solutions.

Once you did that you can try to run the following command:

With MPI support:
``mpirun -n 2 ./deploy/vieclus examples/astro-ph.graph --time_limit=60``

Without MPI support:
``./deploy/vieclus examples/astro-ph.graph --time_limit=60``

For a description of the graph format please have a look into the manual.

Python Interface
=====

You can install the Python interface via pip:
``pip install vieclus``

Or build from source:
``pip install .``

### Example: Using the vieclus_graph class

```python
import vieclus

# Build a graph using the vieclus_graph helper class
g = vieclus.vieclus_graph()
g.set_num_nodes(6)

# Add edges (undirected, with weights)
g.add_undirected_edge(0, 1, 5)
g.add_undirected_edge(1, 2, 5)
g.add_undirected_edge(0, 2, 5)
g.add_undirected_edge(3, 4, 5)
g.add_undirected_edge(4, 5, 5)
g.add_undirected_edge(3, 5, 5)
g.add_undirected_edge(2, 3, 1)  # weak bridge between two communities

# Convert to CSR format and cluster
vwgt, xadj, adjcwgt, adjncy = g.get_csr_arrays()
modularity, clustering = vieclus.cluster(vwgt, xadj, adjcwgt, adjncy,
                                         mode=vieclus.STRONG, time_limit=1.0)

print(f"Modularity: {modularity}")
print(f"Clustering: {clustering}")
```

### Example: Using raw CSR arrays

```python
import vieclus

# Graph in METIS CSR format (same as KaHIP)
xadj   = [0, 2, 5, 7, 9, 12]
adjncy = [1, 4, 0, 2, 4, 1, 3, 2, 4, 0, 1, 3]
vwgt   = [1, 1, 1, 1, 1]
adjcwgt = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]

modularity, clustering = vieclus.cluster(vwgt, xadj, adjcwgt, adjncy,
                                         suppress_output=True,
                                         seed=0,
                                         mode=vieclus.ECO,
                                         time_limit=2.0)

print(f"Modularity: {modularity}")
print(f"Clustering: {clustering}")
```

### Parameters

The `vieclus.cluster` function takes the following arguments:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `vwgt` | list | *required* | Node weights (length n) |
| `xadj` | list | *required* | CSR index array (length n+1) |
| `adjcwgt` | list | *required* | Edge weights (length m) |
| `adjncy` | list | *required* | CSR adjacency array (length m) |
| `suppress_output` | bool | `True` | Suppress console output |
| `seed` | int | `0` | Random seed |
| `mode` | int | `STRONG` | Clustering mode: `vieclus.FAST`, `vieclus.ECO`, or `vieclus.STRONG` |
| `time_limit` | float | `1.0` | Time limit in seconds |
| `cluster_upperbound` | int | `0` | Max cluster size (0 = no limit) |

Returns a tuple `(modularity, clustering)` where `modularity` is a float in [-1, 1] and `clustering` is a list of cluster IDs for each node.

Release Notes
=====

### v1.2
- Added Python interface (`pip install vieclus`) with pybind11 bindings
- Added `vieclus_graph` helper class for easy graph construction (same interface as KaHIP)
- Added `vieclus.cluster()` function with FAST, ECO, and STRONG modes
- Added PyPI packaging with scikit-build-core
- Added GitHub Actions CI and automated PyPI publishing
- Added NOMPI compilation support

### v1.1
- Added cmake build system
- Added option to compile without MPI support

### v1.0
- Initial release of the memetic graph clustering algorithm

Licence
=====
The program is licenced under MIT licence.
If you publish results using our algorithms, please acknowledge our work by quoting the following paper:

```
@inproceedings{BiedermannHSS18,
             AUTHOR = {Biedermann, Sonja and Henzinger, Monika and Schulz, Christian and Schuster, Bernhard},
             TITLE = {{Memetic Graph Clustering}},
             BOOKTITLE = {{Proceedings of the 17th International Symposium on Experimental Algorithms (SEA'18)}},
             SERIES = {{LIPIcs}},
             PUBLISHER = {Dagstuhl},
             NOTE = {Technical Report, arXiv:1802.07034},
             YEAR = {2018}
}
```

