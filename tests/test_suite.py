from pathlib import Path
import os
import subprocess
from functools import partial 

def process_single_graph(graph_file: Path, c_binary_dir: Path):
    result = subprocess.run([c_binary_dir, str(graph_file)], capture_output=True, text=True)
    print(result.stdout)
    output_lines = result.stdout.strip().split('\n')
        
    efficiency = float(output_lines[0].split(': ')[1])
    my_code_time = float(output_lines[1].split(': ')[1].split(' ')[0])
    c11_threads_time = float(output_lines[2].split(': ')[1].split(' ')[0])
    igraph_time = float(output_lines[3].split(': ')[1].split(' ')[0])
        
    speedup = igraph_time / my_code_time 
    return {
        'graph_file': str(graph_file),
        'igraph_time_s': igraph_time,
        'my_code_time_s': my_code_time,
        'c11_threads_time_s': c11_threads_time,
        'efficiency': efficiency
    }



def main():
    c_binary_dir = Path(__file__).parent.parent / "build" / "main_cli"
    if not os.path.exists(c_binary_dir):
        c_binary_dir = Path(input("C binary not found. Please provide the path to the compiled C binary: "))
        
    graph_tests_dir = Path(__file__).parent / "graphs_for_dijkstra"
    graph_tests_files = list(map(lambda x: graph_tests_dir / x, ["A_10000_2_0.net"]))
    csv_path = "parallel_implementation.csv"
    with open(csv_path, "w") as f:
        f.write("graph_file,repetition,cpu_cores,schedule,chunk_size,execution_time,efficiency\n")
        for i in range(10):
            for graph_file in graph_tests_files:
                    for schedule in ["dynamic"]:
                        for num_cores in range(1,os.cpu_count()+1):
                            chunk_size = 1
                            os.environ['OMP_SCHEDULE']=f"{schedule},{chunk_size}"
                            os.environ['OMP_NUM_THREADS'] = str(num_cores)
                            os.environ["C11_THREADS_NUM_THREADS"]=str(num_cores)
                            #os.environ['OMP_PROC_BIND'] = "true"
                            #os.environ['OMP_PLACES'] = "cores"
                            os.environ["C11_THREADS_CHUNK_SIZE"] = str(chunk_size)
                            result=process_single_graph(graph_file, c_binary_dir)
                            f.write(f"{result['graph_file']},{i},{num_cores},OMP_{schedule},{chunk_size},{result['my_code_time_s']},{result['efficiency']}\n")                
                            f.write(f"{result['graph_file']},{i},{num_cores},C11_threads,{chunk_size},{result['c11_threads_time_s']},{result['efficiency']}\n")                
                            f.flush()

if __name__ == "__main__":
    main()