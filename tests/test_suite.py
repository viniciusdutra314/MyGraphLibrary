import numpy as np
import igraph as ig  
from time import time
from pathlib import Path
import os
from concurrent.futures import ProcessPoolExecutor,as_completed
import pandas as pd
import subprocess
from functools import partial 
import threading

# This function is unchanged
def create_ig_graph_from_file(filepath):
    with open(filepath, 'r') as f:
        num_vertices = int(f.readline().strip())
        f.readline() 
        g = ig.Graph(n=num_vertices, directed=True)
        edges = []
        weights = []
        for line in f:
            if line.strip():
                u, v, w = line.strip().split()
                edges.append((int(u), int(v)))
                weights.append(float(w))
        g.add_edges(edges)
        g.es['weight'] = weights
        return g


# 2. CREATE A TOP-LEVEL WORKER FUNCTION
# This contains all the logic from your old for loop
def process_single_graph(graph_file: Path, c_binary_dir: Path):
    """
    Worker function to process a single graph file in a separate process.
    """
    print(f"Processing graph file: {graph_file.name}")
    g = create_ig_graph_from_file(graph_file)
    N = g.vcount()  
    igraph_start_time = time()
    efficiency_igraph = 0.0
    for node in range(N):
        distances_list = g.distances(source=node, weights="weight",algorithm='dijkstra')[0]
        distances_array = np.array(distances_list)
        distances_array[node] = np.inf
        reciprocals = 1.0 / distances_array
        efficiency_igraph += np.sum(reciprocals)
    efficiency_igraph /= (N * (N - 1))
    igraph_time = time() - igraph_start_time
    
    my_code_time_start = time()
    subprocess.run([c_binary_dir, str(graph_file)], stdout=subprocess.DEVNULL)
    my_code_time = time() - my_code_time_start
    with open(str(graph_file) + ".eff", 'r') as f:
        efficiency_my_code = float(f.read().strip())
        
    print(f"{graph_file.name} \t | igraph {int(igraph_time*1e3):E}ms \t | my_code {int(my_code_time*1e3):E}ms \t | speedup {igraph_time/my_code_time:.2f}x ")
    print(f"Efficiency comparison: igraph={efficiency_igraph:.8f}, my_code={efficiency_my_code:.8f}")
    assert np.isclose(efficiency_igraph, efficiency_my_code, rtol=1e-8)
    
    return {
        "graph_file": graph_file.name,
        "igraph_time_ms": igraph_time * 1e3,
        "my_code_time_ms": my_code_time * 1e3,
        "speedup": igraph_time / my_code_time,
        "efficiency_igraph": efficiency_igraph,
        "efficiency_my_code": efficiency_my_code
    }


def compare_my_code_with_igraph(c_binary_dir: Path, graph_tests_files: list[Path]):
    csv_path = "performance_comparison_igraph.csv"
    with open(csv_path, "w") as f:
        f.write("graph_file,igraph_time_ms,my_code_time_ms,speedup,efficiency_igraph,efficiency_my_code\n")
    with ProcessPoolExecutor(8) as executor:
        worker_func = partial(process_single_graph, c_binary_dir=c_binary_dir)
        futures = {executor.submit(worker_func, graph_file): graph_file for graph_file in graph_tests_files}
        try:
            for future in as_completed(futures):
                graph_file = futures[future] 
                result = future.result() 
                with open(csv_path, "a") as f:
                    f.write(f"{result['graph_file']},{result['igraph_time_ms']}, \
                            {result['my_code_time_ms']},{result['speedup']}, \
                            {result['efficiency_igraph']},{result['efficiency_my_code']}\n")                
                    f.flush()
        except Exception as e:
            print(f"A worker process for {graph_file.name} failed: {e}")
            raise

# 5. NO CHANGES NEEDED to main() or the __name__ guard
def main():
    c_binary_dir = Path(__file__).parent.parent / "build" / "main_cli"
    if not os.path.exists(c_binary_dir):
        # This line was missing str() conversion, which can cause issues
        c_binary_dir = Path(input("C binary not found. Please provide the path to the compiled C binary: "))
        
    graph_tests_dir = Path(__file__).parent / "graphs_for_dijkstra"
    graph_tests_files = [f for f in graph_tests_dir.iterdir() if f.name.endswith('0.net')]
    
    print(f"Starting parallel comparison on {len(graph_tests_files)} files...")
    compare_my_code_with_igraph(c_binary_dir, graph_tests_files)    

if __name__ == "__main__":
    main()