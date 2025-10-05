import numpy as np
import networkx as nx
from time import time
from pathlib import Path 
import os
import subprocess


def create_nx_graph_from_file(filepath):
    with open(filepath, 'r') as f:
        num_vertices = int(f.readline().strip())
        num_edges=f.readline()
        G = nx.DiGraph()
        for line in f:
            if line.strip():
                u, v, w = line.strip().split()
                G.add_edge(int(u), int(v), weight=float(w))
        return G



def compare_my_code_with_networkx(c_binary_dir:Path,graph_tests_files:list[Path]):
    for graph_file in graph_tests_files:
        G=create_nx_graph_from_file(graph_file)
        N=G.number_of_nodes()
        networkx_start_time=time()
        distances=nx.floyd_warshall_numpy(G)
        with np.errstate(divide='ignore'):
            inv_distances = 1 / distances
            inv_distances[np.isinf(inv_distances)] = 0
        efficiency_networkx = np.sum(inv_distances) / (N * (N - 1))    
        networkx_time=time()-networkx_start_time

        my_code_time_start=time()
        subprocess.run([c_binary_dir, str(graph_file)],stdout=subprocess.DEVNULL)
        my_code_time=time()-my_code_time_start
        with open(str(graph_file)+".eff", 'r') as f:
            efficiency_my_code = float(f.read().strip())
        assert np.isclose(efficiency_networkx, efficiency_my_code, rtol=1e-8)
        print(f"{graph_file.name} \t | networkx {int(networkx_time*1e3):E}ms \t | my_code {int(my_code_time*1e3):E}ms \t | speedup {networkx_time/my_code_time:.2f}x ")

def main():
    c_binary_dir=Path(__file__).parent.parent / "build" / "release" / "main_cli"
    if not os.path.exists(c_binary_dir):
        c_binary_dir=input("C binary not found. Please provide the path to the compiled C binary: ")
    graph_tests_dir=Path(__file__).parent/ "graphs_for_floyd_warshall"
    graph_tests_files=[graph_tests_dir / f for f in os.listdir(graph_tests_dir) if f.endswith(".net")]
    compare_my_code_with_networkx(c_binary_dir,graph_tests_files)

if __name__ == "__main__":
    main()