#include <stdio.h>
#include <time.h>
#include "graph_library.h"

int main(int argc,char** argv){
    Graph graph;
    DistanceMatrix distances;
    if (argc!=2){
        fprintf(stderr,"Erro: um arquivo de um grafo no formato "
            "de edgelist deve ser fornecido \n");
        return 1;
    }

    if (read_edgelist(&graph,argv[1]) != 0) {
        fprintf(stderr, "Erro lendo o arquivo edgelist \n");
        return 1;
    }
    clock_t start_time=clock();
    if (floyd_warshall(&graph,&distances) != 0) {
        fprintf(stderr, "Erro na execuação do algoritmo de Floyd-Warshall \n");
        return 1;
    }
    printf("A eficiência é de %.17g \n",calculate_efficiency(&distances));
    clock_t end_time=clock();
    printf("O tempo de execução foi de %f s \n",((double) (end_time - start_time)) / CLOCKS_PER_SEC);
    graph_destroy(&graph);
    matrix_destroy(&distances);
}