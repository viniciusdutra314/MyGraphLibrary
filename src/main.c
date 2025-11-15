#include <stdio.h>
#include <time.h>
#include "data_structures.h"
#include "graph_library.h"
#include <string.h>
#include <threads.h>
#include <stdlib.h>
#include <mpi.h>

#ifdef COMPARE_WITH_IGRAPH
#include <igraph.h>
#endif

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int nprocs;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Graph graph;
    struct timespec start_time;
    if (rank == 0)
    {
        timespec_get(&start_time, TIME_UTC);
        if (argc != 2)
        {
            fprintf(stderr, "Erro: um arquivo de um grafo no formato "
                            "de edgelist deve ser fornecido \n");
            return 1;
        }
    }
    if (Graph_create_edgelist(&graph, argv[1]) != 0)
    {
        fprintf(stderr, "Erro lendo o arquivo edgelist \n");
        return 1;
    };
    if (Graph_create_adjacency_list(&graph)!=0){
        fprintf(stderr, "Erro criando a lista de adjacência \n");
        return 1;
    }
    VecDouble distances;
    VecDouble_init(&distances);
    double local_efficiency = 0;
    for (size_t source = rank; source < graph.V; source += nprocs)
    {
        dijkstra(&graph, source, &distances);
        for (size_t target = 0; target < graph.V; target++)
        {
            if (source != target)
            {
                local_efficiency += 1.0 / VecDouble_get(&distances, target);
            }
        }
    }
    double global_efficiency=0.0;
    MPI_Reduce(&local_efficiency,&global_efficiency,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    if (rank == 0)
    {
        global_efficiency/= (graph.V * (graph.V - 1));
        printf("Efficiency: %.8f \n", global_efficiency);
        struct timespec end_time;
        timespec_get(&end_time, TIME_UTC);
        printf("MyCode(time): %.8f s \n", (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9);
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

            if ((fabs(ig_efficiency - global_efficiency) / ig_efficiency) > 1e-6)
            {
                fprintf(stderr, "Erro: A eficiência calculada pelo igraph (%.8f) difere da eficiência calculada pelo meu código (%.8f)\n", ig_efficiency, global_efficiency);
                return 1;
            }

            printf("Igraph(time): %.8f s  \n", (ig_end_time.tv_sec - ig_start_time.tv_sec) + (ig_end_time.tv_nsec - ig_start_time.tv_nsec) / 1e9);

            igraph_vector_int_destroy(&edges);
            igraph_vector_destroy(&weights);
            igraph_destroy(&ig_graph);
        #endif
        char *output_file_name = malloc(strlen(argv[1]) + strlen(".eff") + 1);
        strcpy(output_file_name, argv[1]);
        strcat(output_file_name, ".eff");
        FILE *output_file = fopen(output_file_name, "w");
        fprintf(output_file, "%.8f", local_efficiency);
        fclose(output_file);
        free(output_file_name);
    }

    Graph_destroy(&graph);
    VecDouble_free(&distances);
    MPI_Finalize();
    return 0;
}