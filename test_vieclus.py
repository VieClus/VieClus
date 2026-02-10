"""Test script for VieClus Python interface."""
import vieclus

print("=== Test 1: Import and version ===")
print(f"VieClus version: {vieclus.__version__}")

print("\n=== Test 2: vieclus_graph helper class ===")
# Build a small graph with two clear communities:
#   Community 1: nodes 0,1,2 (triangle with weight 5)
#   Community 2: nodes 3,4,5 (triangle with weight 5)
#   Bridge: edge (2,3) with weight 1
g = vieclus.vieclus_graph()
g.set_num_nodes(6)
# Community 1
g.add_undirected_edge(0, 1, 5)
g.add_undirected_edge(1, 2, 5)
g.add_undirected_edge(0, 2, 5)
# Community 2
g.add_undirected_edge(3, 4, 5)
g.add_undirected_edge(4, 5, 5)
g.add_undirected_edge(3, 5, 5)
# Bridge
g.add_undirected_edge(2, 3, 1)

vwgt, xadj, adjcwgt, adjncy = g.get_csr_arrays()
print(f"  vwgt: {vwgt}")
print(f"  xadj: {xadj}")
print(f"  adjcwgt: {adjcwgt}")
print(f"  adjncy: {adjncy}")
print(f"  num nodes: {g.num_nodes}")
print(f"  num edges (directed): {len(adjncy)}")

print("\n=== Test 3: Clustering (two communities) ===")
modularity, clustering = vieclus.cluster(
    vwgt, xadj, adjcwgt, adjncy,
    suppress_output=False,
    seed=0,
    time_limit=1.0
)
print(f"  Modularity: {modularity:.6f}")
print(f"  Clustering: {list(clustering)}")
num_clusters = len(set(clustering))
print(f"  Number of clusters: {num_clusters}")
assert len(clustering) == 6, f"Expected 6 entries, got {len(clustering)}"
assert modularity > 0, f"Expected positive modularity, got {modularity}"
print("  PASSED")

print("\n=== Test 4: Clustering with different seed ===")
modularity, clustering = vieclus.cluster(
    vwgt, xadj, adjcwgt, adjncy,
    suppress_output=True,
    seed=42,
    time_limit=1.0
)
print(f"  Modularity: {modularity:.6f}")
print(f"  Clustering: {list(clustering)}")
num_clusters = len(set(clustering))
print(f"  Number of clusters: {num_clusters}")
assert len(clustering) == 6
assert modularity > 0
print("  PASSED")

print("\n=== Test 5: Raw CSR arrays (5-node path graph) ===")
# Simple path: 0-1-2-3-4
xadj   = [0, 1, 3, 5, 7, 8]
adjncy = [1, 0, 2, 1, 3, 2, 4, 3]
vwgt   = [1, 1, 1, 1, 1]
adjcwgt = [1, 1, 1, 1, 1, 1, 1, 1]

modularity, clustering = vieclus.cluster(
    vwgt, xadj, adjcwgt, adjncy,
    suppress_output=True,
    seed=0,
    time_limit=1.0
)
print(f"  Modularity: {modularity:.6f}")
print(f"  Clustering: {list(clustering)}")
assert len(clustering) == 5
print("  PASSED")

print("\n=== All tests passed! ===")
