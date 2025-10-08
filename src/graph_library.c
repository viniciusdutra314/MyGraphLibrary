#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graph_library.h"
#include "data_structures.h"

IMPLEMENT_VECTOR_INTERFACE(Edge, VecEdge)
IMPLEMENT_VECTOR_INTERFACE(VertexWithWeight, VecVertexWeight)
IMPLEMENT_VECTOR_OF_VECTORS_INTERFACE(VecVertexWeight, VecVecVertexWeight)
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
    size_t V, E;
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
    size_t num_line = 0;
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

int Graph_create_adjacency_list(Graph *graph)
{
    if (graph->edge_list.data == NULL)
    {
        fprintf(stderr, "A edgelist está vazia, não é possível criar a lista de adjacência\n");
        return 1;
    };

    VecVecVertexWeight_init(&graph->adjacency_list);
    VecVecVertexWeight_reserve(&graph->adjacency_list, graph->V);
    for (size_t i = 0; i < graph->E; i++)
    {
        Edge current_edge = VecEdge_get(&graph->edge_list, i);
        VecVertexWeight *vec = VecVecVertexWeight_get_ptr(&graph->adjacency_list, current_edge.from);
        VertexWithWeight element = {current_edge.to, current_edge.weight};
        VecVertexWeight_push_back(vec, element);
    }
    return 0;
}

void Graph_init(Graph *graph)
{
    graph->E = 0;
    graph->V = 0;
    VecEdge_init(&graph->edge_list);
    VecVecVertexWeight_init(&graph->adjacency_list);
}

void Graph_destroy(Graph *graph)
{
    VecEdge_free(&graph->edge_list);
    VecVecVertexWeight_free(&graph->adjacency_list);
}

int floyd_warshall(Graph const *graph, MatrixDouble *distances)
{
    size_t const V = graph->V;
    size_t const E = graph->E;
    if (MatrixDouble_init(distances, V) != 0)
    {
        fprintf(stderr, "Alocação da matriz de distância falhou");
        goto clean_up;
    };
    for (size_t i = 0; i < V; i++)
    {
        for (size_t j = 0; j < V; j++)
        {
            MatrixDouble_set(distances, i, j, i != j ? INFINITY : 0);
        }
    }

    for (size_t edge_index = 0; edge_index < E; edge_index++)
    {
        Edge const edge = VecEdge_get(&graph->edge_list, edge_index);
        MatrixDouble_set(distances, edge.from, edge.to, edge.weight);
    };

    for (size_t k = 0; k < V; k++)
    {
        for (size_t i = 0; i < V; i++)
        {
            double const d_ik = MatrixDouble_get(distances, i, k);
            for (size_t j = 0; j < V; j++)
            {
                double const d_kj = MatrixDouble_get(distances, k, j);
                double const d_ij = MatrixDouble_get(distances, i, j);
                double const new_distance = d_ik + d_kj;
                if (new_distance > d_ij)
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

int dijkstra(Graph const *graph, size_t source, VecDouble *distances)
{
    if (graph->adjacency_list.data == NULL)
    {
        fprintf(stderr, "A lista de adjacência está vazia, não é possível executar o algoritmo de Dijkstra\n");
        return 1;
    };
    size_t const V = graph->V;
    if (source >= V)
    {
        fprintf(stderr, "O vértice fonte é inválido\n");
        return 1;
    }
    VecDouble_init(distances);
    if (VecDouble_reserve(distances, V) != 0)
    {
        fprintf(stderr, "Alocação do vetor de distâncias falhou");
        return 1;
    };
    MinHeap heap;
    MinHeap_init(&heap);
    for (size_t i = 0; i < V; i++)
    {
        MinHeap_add(&heap, i, i != source ? INFINITY : 0);
        VecDouble_push_back(distances, i != source ? INFINITY : 0);
    };

    while (!MinHeap_is_empty(&heap))
    {
        size_t vertex_id = MinHeap_get(&heap);
        VecVertexWeight vec = VecVecVertexWeight_get(
            &graph->adjacency_list, vertex_id);
        size_t num_neighbors = VecVertexWeight_size(&vec);
        size_t const d_j = VecDouble_get(distances, vertex_id);
        for (size_t i = 0; i < num_neighbors; i++)
        {
            VertexWithWeight neighbor = VecVertexWeight_get(&vec, i);
            double const new_distance= d_j + neighbor.weight;
            if (new_distance<VecDouble_get(distances, neighbor.vertex_id))
            {
                VecDouble_set(distances,neighbor.vertex_id,new_distance);
                MinHeap_decrease_key(&heap,neighbor.vertex_id,new_distance);
            }
        }
    }
    return 0;
}

