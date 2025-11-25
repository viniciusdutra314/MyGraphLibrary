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
    Graph_init(&graph);
    struct timespec start_time;
    if (rank == 0)
    {
        timespec_get(&start_time, TIME_UTC);
        if (argc != 2)
        {
            fprintf(stderr, "Erro: um arquivo de um grafo no formato "
                            "de edgelist deve ser fornecido \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if (Graph_create_edgelist(&graph, argv[1]) != 0)
        {
            fprintf(stderr, "Erro lendo o arquivo edgelist \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&graph.V, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&graph.E, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        if (VecEdge_reserve(&graph.edge_list, graph.E) != 0)
        {
            fprintf(stderr, "Falha na alocação da edge list no processo %d\n", rank);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        VecEdge_resize(&graph.edge_list, graph.E);
    }

    MPI_Bcast(graph.edge_list.data, graph.E * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    MatrixDouble distances;
    MatrixDouble_init(&distances, 0,0);

    if (floyd_warshall_openmpi(&graph, &distances) != 0)
    {
        fprintf(stderr, "Erro executando Floyd-Warshall paralelo no processo %d\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0)
    {
        double global_efficiency = 0.0;
        for (size_t i = 0; i < graph.V; i++)
        {
            for (size_t j = 0; j < graph.V; j++)
            {
                if (i != j)
                {
                    double dist = MatrixDouble_get(&distances, i, j);
                    if (!isinf(dist) && dist != 0.0)
                    {
                        global_efficiency += 1.0 / dist;
                    }
                }
            }
        }
        global_efficiency /= (graph.V * (graph.V - 1));
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
            VECTOR(edges)[i * 2] = edge.from;
            VECTOR(edges)[i * 2 + 1] = edge.to;
            VECTOR(weights)[i] = edge.weight;
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
            MPI_Abort(MPI_COMM_WORLD, 1);
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
        fprintf(output_file, "%.8f", global_efficiency);
        fclose(output_file);
        free(output_file_name);
    }
    MatrixDouble_free(&distances);
    Graph_destroy(&graph);
    MPI_Finalize();
    return 0;
}