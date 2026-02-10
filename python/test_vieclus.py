"""Test script for VieClus Python interface."""
import vieclus
import subprocess
import re
import os

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


def read_metis_graph(filename):
    """Read a METIS graph file and return CSR arrays."""
    with open(filename, 'r') as f:
        lines = f.readlines()

    # Parse header: n m [fmt] [ncon]
    header = lines[0].split()
    n = int(header[0])
    # m = int(header[1])  # not needed directly

    xadj = [0]
    adjncy = []
    adjcwgt = []
    vwgt = [1] * n

    for i in range(1, n + 1):
        neighbors = lines[i].split()
        for nb in neighbors:
            adjncy.append(int(nb) - 1)  # convert 1-indexed to 0-indexed
            adjcwgt.append(1)
        xadj.append(len(adjncy))

    return n, vwgt, xadj, adjcwgt, adjncy


print("\n=== Test 6: Facebook graph - Python vs command line ===")
graph_file = "/home/c_schulz/projects/graph_collection/facebook100/Amherst41.graph"
vieclus_bin = os.path.join(os.path.dirname(__file__), "deploy", "vieclus")

if not os.path.exists(graph_file):
    print(f"  SKIPPED (graph file not found: {graph_file})")
elif not os.path.exists(vieclus_bin):
    print(f"  SKIPPED (vieclus binary not found: {vieclus_bin})")
else:
    time_limit = 5.0
    seed = 0

    # Run command-line tool
    print(f"  Graph: {graph_file}")
    print(f"  Time limit: {time_limit}s, Seed: {seed}")
    print(f"  Running command-line vieclus...")
    result = subprocess.run(
        [vieclus_bin, graph_file,
         f"--time_limit={time_limit}",
         f"--seed={seed}"],
        capture_output=True, text=True, timeout=60
    )
    cli_output = result.stdout + result.stderr
    # Parse modularity from output: "modularity \t\t\t0.xxxxx"
    match = re.search(r'modularity\s+([\d.]+)', cli_output)
    if match:
        cli_modularity = float(match.group(1))
        print(f"  CLI modularity:    {cli_modularity:.6f}")
    else:
        print(f"  Could not parse CLI modularity from output:")
        print(f"  {cli_output}")
        cli_modularity = None

    # Run Python interface
    print(f"  Running Python vieclus...")
    n, vwgt, xadj, adjcwgt, adjncy = read_metis_graph(graph_file)
    print(f"  Graph: {n} nodes, {len(adjncy)//2} edges")

    py_modularity, clustering = vieclus.cluster(
        vwgt, xadj, adjcwgt, adjncy,
        suppress_output=True,
        seed=seed,
        time_limit=time_limit
    )
    num_clusters = len(set(clustering))
    print(f"  Python modularity: {py_modularity:.6f}")
    print(f"  Number of clusters: {num_clusters}")

    if cli_modularity is not None:
        diff = abs(py_modularity - cli_modularity)
        print(f"  Difference:        {diff:.6f}")
        # Both should find roughly the same modularity (within 5%)
        assert diff < 0.05, (
            f"Modularity difference too large: CLI={cli_modularity:.6f}, "
            f"Python={py_modularity:.6f}, diff={diff:.6f}"
        )
        print("  PASSED")
    else:
        print("  SKIPPED (could not compare)")


print("\n=== All tests passed! ===")
