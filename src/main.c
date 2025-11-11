#include <stdio.h>
#include <time.h>
#include "data_structures.h"
#include "graph_library.h"
#include <string.h>
#include <threads.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <omp.h>

#ifdef COMPARE_WITH_IGRAPH
    #include <igraph.h>
#endif

#ifdef COMPARE_WITH_C11_THREADS
    typedef struct
    {
        Graph const *graph;
        double *global_efficiency;
        mtx_t *efficiency_mutex;
        atomic_size_t *next_source_vertex;
    } ThreadArgs;

    int worker_dijkstra(void *arg)
    {
        ThreadArgs args = *(ThreadArgs *)(arg);
        VecDouble distances;
        VecDouble_init(&distances);
        const size_t V = args.graph->V;
        double local_efficiency = 0.0;
        while (1)
        {
            size_t source = atomic_fetch_add(args.next_source_vertex, 1);
            if (source >= V)
            {
                break;
            }
            dijkstra(args.graph, source, &distances);
            for (size_t target = 0; target < V; target++)
            {
                if (source != target)
                {
                    double d = VecDouble_get(&distances, target);
                    if (!isinf(d))
                    {
                        local_efficiency += 1.0 / d;
                    }
                }
            }
        }
        VecDouble_free(&distances);
        mtx_lock(args.efficiency_mutex);
        *args.global_efficiency += local_efficiency;
        mtx_unlock(args.efficiency_mutex);
        return thrd_success;
    }
#endif

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Erro: um arquivo de um grafo no formato "
                        "de edgelist deve ser fornecido \n");
        return 1;
    }
    Graph graph;
    if (Graph_create_edgelist(&graph, argv[1]) != 0)
    {
        fprintf(stderr, "Erro lendo o arquivo edgelist \n");
        return 1;
    }
    Graph_create_adjacency_list(&graph);
    struct timespec start_time;
    timespec_get(&start_time, TIME_UTC);
    double efficiency = 0;

#pragma omp parallel reduction(+ : efficiency) default(none) shared(graph)
    {
        VecDouble distances;
        VecDouble_init(&distances);
#pragma omp for schedule(runtime)
        for (size_t source = 0; source < graph.V; source++)
        {
            dijkstra(&graph, source, &distances);
            for (size_t target = 0; target < graph.V; target++)
            {
                if (source != target)
                {
                    double d = VecDouble_get(&distances, target);
                    if (!isinf(d))
                    {
                        efficiency += 1.0 / d;
                    }
                }
            }
        }
        VecDouble_free(&distances);
    }

    efficiency /= (graph.V * (graph.V - 1));
    printf("Efficiency: %.8f \n", efficiency);
    struct timespec end_time;
    timespec_get(&end_time, TIME_UTC);
    printf("MyCode(time): %.8f s \n", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9);
    #ifdef COMPARE_WITH_C11_THREADS
        char *num_threads_str = getenv("OMP_NUM_THREADS");
        long num_threads=1;
        if (num_threads_str == NULL) {
            num_threads = omp_get_num_threads();
        } else{
            char *strtol_end;
            num_threads = strtol(num_threads_str, &strtol_end, 10);
        }

        thrd_t *vec_of_threads = (thrd_t *)malloc(sizeof(thrd_t) * num_threads);
        double efficiency_c11=0.0;
        mtx_t efficiency_mutex;
        mtx_init(&efficiency_mutex,mtx_plain);
        atomic_size_t next_source_vertex = 0;
        ThreadArgs args = {.graph = &graph,
                        .global_efficiency = &efficiency_c11,
                        .next_source_vertex = &next_source_vertex,
                            .efficiency_mutex=&efficiency_mutex };
        struct timespec c11_start_time;
        timespec_get(&c11_start_time, TIME_UTC);
        for (size_t i = 0; i < (size_t)num_threads; i++)
        {
            thrd_create(&vec_of_threads[i], worker_dijkstra,&args);
        };
        for (size_t i = 0; i < (size_t)num_threads; i++)
        {
            thrd_join(vec_of_threads[i], NULL);
        }
        efficiency_c11 /= (graph.V * (graph.V - 1));
        struct timespec c11_end_time;
        timespec_get(&c11_end_time, TIME_UTC);

        printf("C11 Threads(time): %.8f s \n", (c11_end_time.tv_sec - c11_start_time.tv_sec) + (c11_end_time.tv_nsec - c11_start_time.tv_nsec) / 1e9);
        if (fabs(efficiency - efficiency_c11)/efficiency > 1e-6) {
            fprintf(stderr,"OpenMP efficiency (%.8f) differs from C11 threads efficiency (%.8f)\n", efficiency, efficiency_c11);
            return 1;
        }
        mtx_destroy(&efficiency_mutex);
        free(vec_of_threads);
    #endif
    #ifdef COMPARE_WITH_IGRAPH
        igraph_t ig_graph;
        igraph_vector_int_t edges;
        igraph_vector_t weights;
        igraph_vector_int_init(&edges, graph.E * 2);
        igraph_vector_init(&weights, graph.E);
        for (size_t i = 0; i < graph.E; i++)
        {
            Edge edge = VecEdge_get(&graph.edge_list, i);
            VECTOR(edges)
            [i * 2] = edge.from;
            VECTOR(edges)
            [i * 2 + 1] = edge.to;
            VECTOR(weights)
            [i] = edge.weight;
        }

        igraph_create(&ig_graph, &edges, graph.V, IGRAPH_DIRECTED);
        struct timespec ig_start_time;
        timespec_get(&ig_start_time, TIME_UTC);
        igraph_real_t ig_efficiency;
        igraph_global_efficiency(&ig_graph, &ig_efficiency, &weights, IGRAPH_DIRECTED);
        struct timespec ig_end_time;
        timespec_get(&ig_end_time, TIME_UTC);

        if ((fabs(ig_efficiency - efficiency) / ig_efficiency) > 1e-6)
        {
            fprintf(stderr, "Erro: A eficiência calculada pelo igraph (%.8f) difere da eficiência calculada pelo meu código (%.8f)\n", ig_efficiency, efficiency);
            return 1;
        }

        printf("Igraph(time): %.8f s  \n", (ig_end_time.tv_sec - ig_start_time.tv_sec) + (ig_end_time.tv_nsec - ig_start_time.tv_nsec) / 1e9);

        igraph_vector_int_destroy(&edges);
        igraph_vector_destroy(&weights);
        igraph_destroy(&ig_graph);
    #endif
    Graph_destroy(&graph);
    char *output_file_name = malloc(strlen(argv[1]) + strlen(".eff") + 1);
    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".eff");
    FILE *output_file = fopen(output_file_name, "w");
    fprintf(output_file, "%.8f", efficiency);
    fclose(output_file);
    free(output_file_name);
}