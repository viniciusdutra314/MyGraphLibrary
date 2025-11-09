from time import time
from pathlib import Path
import os
from concurrent.futures import ProcessPoolExecutor,as_completed
import subprocess
from functools import partial 

def process_single_graph(graph_file: Path, c_binary_dir: Path):
    print(f"Processing graph file: {graph_file.name}")
    subprocess.run([c_binary_dir, str(graph_file)], stdout=subprocess.DEVNULL)
    result = subprocess.run([c_binary_dir, str(graph_file)], capture_output=True, text=True)
    output_lines = result.stdout.strip().split('\n')
        
    efficiency = float(output_lines[0].split(': ')[1])
    my_code_time = float(output_lines[1].split(': ')[1].split(' ')[0])
    igraph_time = float(output_lines[2].split(': ')[1].split(' ')[0])
        
    speedup = igraph_time / my_code_time 
    return {
        'graph_file': graph_file.name,
        'igraph_time_s': igraph_time,
        'my_code_time_s': my_code_time,
        'speedup': speedup,
        'efficiency': efficiency
    }


def compare_my_code_with_igraph(c_binary_dir: Path, graph_tests_files: list[Path]):
    csv_path = "performance_comparison_igraph.csv"
    with open(csv_path, "w") as f:
        f.write("graph_file,igraph_time_s,my_code_time_s,speedup,efficiency\n")
    with ProcessPoolExecutor() as executor:
        worker_func = partial(process_single_graph, c_binary_dir=c_binary_dir)
        futures = {executor.submit(worker_func, graph_file): graph_file for graph_file in graph_tests_files}
        for future in as_completed(futures):
            result = future.result() 
            with open(csv_path, "a") as f:
                f.write(f"{result['graph_file']},{result['igraph_time_s']},{result['my_code_time_s']},{result['speedup']},{result['efficiency']}\n")                
                f.flush()

def main():
    c_binary_dir = Path(__file__).parent.parent / "build" / "main_cli"
    if not os.path.exists(c_binary_dir):
        c_binary_dir = Path(input("C binary not found. Please provide the path to the compiled C binary: "))
        
    graph_tests_dir = Path(__file__).parent / "graphs_for_dijkstra"
    graph_tests_files = [f for f in graph_tests_dir.iterdir() if f.name.endswith('0.net')]
    compare_my_code_with_igraph(c_binary_dir, graph_tests_files)    

if __name__ == "__main__":
    main()