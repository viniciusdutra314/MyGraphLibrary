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

typedef struct {
    size_t vertex_id;
    double weight;
} VertexWithWeight;

DECLARE_VECTOR_INTERFACE(Edge, VecEdge)
DECLARE_VECTOR_INTERFACE(VertexWithWeight, VecVertexWeight)
DECLARE_VECTOR_OF_VECTORS_INTERFACE(VecVertexWeight, VecVecVertexWeight)

typedef struct
{
    size_t V;
    size_t E;
    VecEdge edge_list;
    VecVecVertexWeight adjacency_list;
} Graph;

void Graph_init(Graph* graph);
int Graph_read_edgelist(Graph *graph, char const *filename);
int Graph_create_adjacency_list(Graph* graph);
void Graph_destroy(Graph* graph);

DECLARE_VECTOR_INTERFACE(double, VecDouble)
DECLARE_SQUARE_MATRIX_INTERFACE(double, MatrixDouble)


int floyd_warshall(Graph const* graph,MatrixDouble* distances);
int dijkstra(Graph const* graph, size_t source, VecDouble* distances);
double calculate_efficiency(MatrixDouble const* distances);