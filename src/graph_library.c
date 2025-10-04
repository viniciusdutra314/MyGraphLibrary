#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graph_library.h"
#include "data_structures.h"

IMPLEMENT_VECTOR_INTERFACE(Edge)
IMPLEMENT_VECTOR_INTERFACE(size_t)
IMPLEMENT_VECTOR_INTERFACE(Vector_size_t)
IMPLEMENT_VECTOR_INTERFACE(double)
IMPLEMENT_SQUARE_MATRIX_INTERFACE(double)


int graph_read_edgelist(Graph *graph, char const *filename)
{
    FILE *file = fopen(filename, "r");
    Vector_Edge edge_list;
    vector_Edge_init(&edge_list);
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
    
    if (vector_Edge_reserve(&edge_list,E) != 0)
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
            fprintf(stderr, "Linha %zu com formatação inválida",num_line);
            goto clean_up;
        }
        vector_Edge_push_back(&edge_list,current_edge);
        num_line++;
    };
    if (num_line != E)
    {
        fprintf(stderr, "Número de edges incompatível com o arquivo fornecido\n");
        goto clean_up;
    };
    graph->V=V;
    graph->E=E;
    graph->edge_list=edge_list;
    return 0;
clean_up:
    vector_Edge_free(&edge_list);
    fclose(file);
    return 1;
}

void graph_init(Graph* graph){
    graph->E=0;
    graph->V=0;
    vector_Edge_init(&graph->edge_list);
    vector_Vector_size_t_init(&graph->adjacency_list);
}

void graph_destroy(Graph* graph){
    vector_Edge_free(&graph->edge_list);
    vector_Vector_size_t_free(&graph->adjacency_list);
}


int floyd_warshall(Graph const* graph,SquareMatrix_double *distances){
    size_t const V=graph->V;
    size_t const E=graph->E;
    if (square_matrix_double_init(distances,V)!=0){
        fprintf(stderr,"Alocação da matriz de distância falhou");
        goto clean_up;
    };
    for (size_t i=0;i<V;i++){
        for (size_t j=0;j<V;j++){
            square_matrix_double_set(distances,i,j,i!=j ? INFINITY : 0);
        }
    }

    for (size_t edge_index=0;edge_index<E;edge_index++){
        Edge const edge=vector_Edge_get(&graph->edge_list,edge_index);
        square_matrix_double_set(distances,edge.from,edge.to,edge.weight);
    };

    for (size_t k=0;k<V;k++){
        for (size_t i=0;i<V;i++){
            double const d_ik=square_matrix_double_get(distances,i,k);
            for (size_t j=0;j<V;j++){
                double const d_kj=square_matrix_double_get(distances,k,j);
                double const d_ij=square_matrix_double_get(distances,i,j);
                if (d_ij>d_ik+d_kj){
                    square_matrix_double_set(distances,i,j,d_ik+d_kj);
                }
            }
        }
    }
    return 0;
clean_up:
    square_matrix_double_free(distances);
    return 1;
}

/* 
int dijkstra(Graph const* graph,DistanceMatrix* distances){
    size_t const V=graph->V;
    for (size_t s=0;s<V;s++){
        PriorityQueue queue;
        for (size_t i=0;i<V;i++){
            double value= s!=i? INFINITY : 0;
            matrix_set(distances,s,i, value);
            priority_queue_add(&queue,i,value);
        }
        while (!priority_queue_is_empty(&queue)){
            size_t j=priority_queue_get(&queue);
            
        };
    };

}; */



double calculate_efficiency(SquareMatrix_double const* distances){
    double efficiency=0;
    size_t const N=distances->N;
    for (size_t i=0;i<N;i++){
        for (size_t j=0;j<N;j++){
            if (i!=j){
                efficiency+=(1.0/square_matrix_double_get(distances,i,j));
            }
        }
    };
    efficiency=efficiency/(N*(N-1));
    return efficiency;
}


