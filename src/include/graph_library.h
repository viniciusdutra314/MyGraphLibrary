#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "data_structures.h"
#pragma once


typedef struct
{
    size_t from;
    size_t to;
    double weight;
} Edge;

DECLARE_VECTOR_INTERFACE(Edge)
DECLARE_VECTOR_INTERFACE(size_t)
DECLARE_VECTOR_INTERFACE(Vector_size_t)

typedef struct
{
    size_t V;
    size_t E;
    Vector_Edge edge_list;
    Vector_Vector_size_t adjacency_list;
} Graph;

void graph_init(Graph* graph);
int graph_read_edgelist(Graph *graph, char const *filename);
void graph_destroy(Graph* graph);

DECLARE_VECTOR_INTERFACE(double)
DECLARE_SQUARE_MATRIX_INTERFACE(double)

int floyd_warshall(Graph const* graph,SquareMatrix_double* distances);
//int dijkstra(Graph const* graph,SquareMatrix_double* distances);
double calculate_efficiency(SquareMatrix_double const* distances);