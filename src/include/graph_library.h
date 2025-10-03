#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "data_structures.h"
#pragma once

/**
 * @brief Represents a weighted edge in a graph.
 * @param from The starting vertex of the edge.
 * @param to The ending vertex of the edge.
 * @param weight The weight of the edge.
 */
typedef struct
{
    size_t from;
    size_t to;
    double weight;
} Edge;

/**
 * @brief Represents a graph using an edge list.
 * @param V The number of vertices in the graph.
 * @param E The number of edges in the graph.
 * @param edge_list A pointer to an array of Edges.
 */
typedef struct
{
    size_t V;
    size_t E;
    Edge *edge_list;
} Graph;

/**
 * @brief Reads a graph from a file in edge list format.
 * The file format is expected to be:
 * V (number of vertices)
 * E (number of edges)
 * from_1 to_1 weight_1
 * from_2 to_2 weight_2
 * ...
 * @param graph A pointer to the Graph structure to be filled.
 * @param filename The name of the file to read from.
 * @return 0 on success, 1 on failure.
 */
int read_edgelist(Graph *graph, char const *filename);
/**
 * @brief Frees the memory allocated for the graph's edge list.
 * @param graph A pointer to the Graph to be destroyed.
 */
void graph_destroy(Graph* graph);

/**
 * @brief Computes all-pairs shortest paths using the Floyd-Warshall algorithm.
 * @param graph A pointer to the input Graph.
 * @param distances A pointer to the DistanceMatrix to store the results.
 * @return 0 on success, 1 on failure.
 */
int floyd_warshall(Graph const* graph,DistanceMatrix* distances);
/**
 * @brief Calculates the global efficiency of the graph.
 * The efficiency is the average of the inverse of the shortest path distances between all pairs of vertices.
 * @param distances A pointer to the DistanceMatrix containing the shortest path distances.
 * @return The global efficiency of the graph.
 */
double calculate_efficiency(DistanceMatrix const* distances);