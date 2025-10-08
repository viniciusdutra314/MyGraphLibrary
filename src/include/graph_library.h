#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "data_structures.h"
#pragma once


typedef struct
{
    uint64_t from;
    uint64_t to;
    double weight;
} Edge;

DECLARE_VECTOR_INTERFACE(Edge, VecEdge)
DECLARE_VECTOR_INTERFACE(uint64_t, VecU64)
DECLARE_VECTOR_INTERFACE(VecU64, VecVecU64)

typedef struct
{
    uint64_t V;
    uint64_t E;
    VecEdge edge_list;
    VecVecU64 adjacency_list;
} Graph;

void Graph_init(Graph* graph);
int Graph_read_edgelist(Graph *graph, char const *filename);
void Graph_destroy(Graph* graph);

DECLARE_VECTOR_INTERFACE(double, VecDouble)
DECLARE_SQUARE_MATRIX_INTERFACE(double, MatrixDouble)


int floyd_warshall(Graph const* graph,MatrixDouble* distances);
double calculate_efficiency(MatrixDouble const* distances);