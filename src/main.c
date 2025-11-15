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
    struct timespec start_time;
    timespec_get(&start_time, TIME_UTC);
    MatrixDouble distances;
    if (floyd_warshall(&graph,&distances)!=0)
    {
        fprintf(stderr, "Erro ao calcular as distâncias entre os vértices \n");
        Graph_destroy(&graph);
        return 1;
    }
    double efficiency = 0;
    for (size_t i = 0; i < graph.V; i++)
    {
        for (size_t j = 0; j < graph.V; j++)
        {
            if (i != j)
            {
                efficiency +=1.0/MatrixDouble_get(&distances, i, j);
            }
        }
    }
    efficiency /= (graph.V * (graph.V - 1));
    printf("Efficiency: %.8f \n", efficiency);
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
    MatrixDouble_free(&distances);
    char *output_file_name = malloc(strlen(argv[1]) + strlen(".eff") + 1);
    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".eff");
    FILE *output_file = fopen(output_file_name, "w");
    fprintf(output_file, "%.8f", efficiency);
    fclose(output_file);
    free(output_file_name);
    MPI_Finalize();
    return 0;
}