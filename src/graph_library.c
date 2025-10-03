#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "graph_library.h"
#include "data_structures.h"



int read_edgelist(Graph *graph, char const *filename)
{
    FILE *file = fopen(filename, "r");
    Edge *edge_list = NULL;
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
    edge_list = malloc(E * sizeof(Edge));
    if (edge_list == NULL)
    {
        fprintf(stderr, "Alocação da edgelist não foi bem sucessida");
        goto clean_up;
    }
    size_t num_line = 0;
    while (!feof(file) && num_line != E)
    {
        Edge *current_edge = &edge_list[num_line];
        if (fscanf(file, " %zu %zu %lf ",
                   &current_edge->from,
                   &current_edge->to,
                   &current_edge->weight) != 3)
        {
            fprintf(stderr, "Linha %zu com formatação inválida",num_line);
            goto clean_up;
        }
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
    free(edge_list);
    fclose(file);
    return 1;
}

void graph_destroy(Graph* graph){
    free(graph->edge_list);
}




int floyd_warshall(Graph const* graph,DistanceMatrix* distances){
    size_t const V=graph->V;
    size_t const E=graph->E;
    distances->N=V;
    distances->mem_buffer=malloc(V*V*sizeof(double));
    if (distances->mem_buffer==NULL){
        fprintf(stderr,"Alocação da matriz de distância falhou");
        goto clean_up;
    };
    for (size_t i=0;i<V;i++){
        for (size_t j=0;j<V;j++){
            set(distances,i,j,i!=j ? INFINITY : 0);
        }
    }

    for (size_t edge_index=0;edge_index<E;edge_index++){
        Edge const edge=graph->edge_list[edge_index];
        set(distances,edge.from,edge.to,edge.weight);
    };

    for (size_t k=0;k<V;k++){
        for (size_t j=0;j<V;j++){
            for (size_t i=0;i<V;i++){
                if (get(distances,i,j)>get(distances,i,k)+get(distances,k,j)){
                    double const new_value=get(distances,i,k)+get(distances,k,j);
                    set(distances,i,j,new_value);
                }
            }
        }
    }
    return 0;
clean_up:
    free(distances->mem_buffer);
    return 1;
}


double calculate_efficiency(DistanceMatrix const* distances){
    double efficiency=0;
    size_t const N=distances->N;
    for (size_t i=0;i<N;i++){
        for (size_t j=0;j<N;j++){
            if (i!=j){
                efficiency+=(1.0/get(distances,i,j))/(N*(N-1));
            }
        }
    };
    return efficiency;
}

