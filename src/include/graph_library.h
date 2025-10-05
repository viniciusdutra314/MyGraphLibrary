#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "data_structures.h"
#pragma once


/**
 * @brief Representa uma aresta com peso
 */
typedef struct
{
    size_t from;
    size_t to;
    double weight;
} Edge;

DECLARE_VECTOR_INTERFACE(Edge)
DECLARE_VECTOR_INTERFACE(size_t)
DECLARE_VECTOR_INTERFACE(Vector_size_t)

/**
 * @brief Representa um grafo usando lista de arestas e/ou lista de adjacência.
 */
typedef struct
{
    size_t V;
    size_t E;
    Vector_Edge edge_list;
    Vector_Vector_size_t adjacency_list;
} Graph;

/**
 * @brief Inicializa um objeto Graph.
 * @param graph Um ponteiro para o Grafo a ser inicializado.
 */
void graph_init(Graph* graph);

/**
 * @brief Lê um grafo de um arquivo no formato de lista de arestas.
 * @param graph Um ponteiro para o objeto Graph a ser preenchido.
 * @param filename O nome do arquivo a ser lido.
 * @return int 0 em caso de sucesso, não-zero em caso de falha.
 */
int graph_read_edgelist(Graph *graph, char const *filename);

/**
 * @brief Libera a memória alocada para um objeto Graph.
 * @param graph Um ponteiro para o Grafo a ser destruído.
 */
void graph_destroy(Graph* graph);

DECLARE_VECTOR_INTERFACE(double)
DECLARE_SQUARE_MATRIX_INTERFACE(double)

/**
 * @brief Calcula os caminhos mais curtos para todos os pares de vértices usando o algoritmo Floyd-Warshall.
 * @param graph Um ponteiro para o objeto Graph constante.
 * @param distances Um ponteiro para uma matriz quadrada não inicializada onde as distâncias dos caminhos mais curtos serão armazenadas.
 * @return int 0 em caso de sucesso, não-zero em caso de falha.
 */
int floyd_warshall(Graph const* graph,SquareMatrix_double* distances);

/**
 * @brief Calcula a eficiência global do grafo.
 * @param distances Um ponteiro para uma matriz quadrada constante das distâncias dos caminhos mais curtos.
 * @return double A eficiência global calculada.
 */
double calculate_efficiency(SquareMatrix_double const* distances);