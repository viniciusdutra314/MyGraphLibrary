#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graph_library.h"
#include "data_structures.h"

IMPLEMENT_VECTOR_INTERFACE(Edge, VecEdge)
IMPLEMENT_VECTOR_INTERFACE(uint64_t, VecU64)
IMPLEMENT_VECTOR_INTERFACE(VecU64, VecVecU64)
IMPLEMENT_VECTOR_INTERFACE(double, VecDouble)
IMPLEMENT_SQUARE_MATRIX_INTERFACE(double, MatrixDouble)

int Graph_read_edgelist(Graph *graph, char const *filename)
{
    FILE *file = fopen(filename, "r");
    VecEdge edge_list;
    VecEdge_init(&edge_list);
    if (file == NULL)
    {
        fprintf(stderr, "O arquivo não conseguiu ser aberto\n");
        goto clean_up;
    }
    uint64_t V, E;
    if (fscanf(file, "%zu", &V) != 1)
    {
        fprintf(stderr, "Não foi possível ler o número de vérticies");
        goto clean_up;
    };
    if (fscanf(file, "%zu", &E) != 1)
    {
        fprintf(stderr, "Não foi possível ler o número de edges");
        goto clean_up;
    };

    if (VecEdge_reserve(&edge_list, E) != 0)
    {
        fprintf(stderr, "Alocação da edgelist não foi bem sucessida");
        goto clean_up;
    }
    uint64_t num_line = 0;
    while (!feof(file) && num_line != E)
    {
        Edge current_edge;
        if (fscanf(file, " %zu %zu %lf ",
                   &current_edge.from,
                   &current_edge.to,
                   &current_edge.weight) != 3)
        {
            fprintf(stderr, "Linha %zu com formatação inválida", num_line);
            goto clean_up;
        };
        if (current_edge.from >= V || current_edge.to >= V)
        {
            fprintf(stderr, "Erro: Vértice com índice inválido na linha %zu.\n", num_line);
            goto clean_up;
        }

        if (current_edge.weight <= 0)
        {
            fprintf(stderr, "Erro: Peso não-positivo na linha %zu.\n", num_line);
            goto clean_up;
        }
        VecEdge_push_back(&edge_list, current_edge);
        num_line++;
    };
    if (num_line != E)
    {
        fprintf(stderr, "Número de edges incompatível com o arquivo fornecido\n");
        goto clean_up;
    };
    graph->V = V;
    graph->E = E;
    graph->edge_list = edge_list;
    return 0;
clean_up:
    VecEdge_free(&edge_list);
    fclose(file);
    return 1;
}

void Graph_init(Graph *graph)
{
    graph->E = 0;
    graph->V = 0;
    VecEdge_init(&graph->edge_list);
    VecVecU64_init(&graph->adjacency_list);
}

void Graph_destroy(Graph *graph)
{
    VecEdge_free(&graph->edge_list);
    VecVecU64_free(&graph->adjacency_list);
}

int floyd_warshall(Graph const *graph, MatrixDouble *distances)
{
    uint64_t const V = graph->V;
    uint64_t const E = graph->E;
    if (MatrixDouble_init(distances, V) != 0)
    {
        fprintf(stderr, "Alocação da matriz de distância falhou");
        goto clean_up;
    };
    for (uint64_t i = 0; i < V; i++)
    {
        for (uint64_t j = 0; j < V; j++)
        {
            MatrixDouble_set(distances, i, j, i != j ? INFINITY : 0);
        }
    }

    for (uint64_t edge_index = 0; edge_index < E; edge_index++)
    {
        Edge const edge = VecEdge_get(&graph->edge_list, edge_index);
        MatrixDouble_set(distances, edge.from, edge.to, edge.weight);
    };

    for (uint64_t k = 0; k < V; k++)
    {
        for (uint64_t i = 0; i < V; i++)
        {
            double const d_ik = MatrixDouble_get(distances, i, k);
            for (uint64_t j = 0; j < V; j++)
            {
                double const d_kj = MatrixDouble_get(distances, k, j);
                double const d_ij = MatrixDouble_get(distances, i, j);
                double const new_distance = d_ik + d_kj;
                if (new_distance> d_ij)
                {
                    MatrixDouble_set(distances, i, j, new_distance);
                }
            }
        }
    }
    return 0;
clean_up:
    MatrixDouble_free(distances);
    return 1;
}


double calculate_efficiency(MatrixDouble const *distances)
{
    double efficiency = 0;
    uint64_t const N = distances->size;
    for (uint64_t i = 0; i < N; i++)
    {
        for (uint64_t j = 0; j < N; j++)
        {
            if (i != j)
            {
                efficiency += (1.0 / MatrixDouble_get(distances, i, j));
            }
        }
    };
    efficiency = efficiency / (N * (N - 1));
    return efficiency;
}
