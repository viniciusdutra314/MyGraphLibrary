#include <stdio.h>
#include <time.h>
#include "data_structures.h"
#include "graph_library.h"
#include <string.h>

int main(int argc,char** argv){
    Graph graph;
    SquareMatrix_double distances;
    if (argc!=2){
        fprintf(stderr,"Erro: um arquivo de um grafo no formato "
            "de edgelist deve ser fornecido \n");
        return 1;
    }

    if (graph_read_edgelist(&graph,argv[1]) != 0) {
        fprintf(stderr, "Erro lendo o arquivo edgelist \n");
        return 1;
    }
    struct timespec start_time;
    timespec_get(&start_time,TIME_UTC);
    if (floyd_warshall(&graph,&distances) != 0) {
        fprintf(stderr, "Erro na execuação do algoritmo de Floyd-Warshall \n");
        return 1;
    }
    double const efficiency=calculate_efficiency(&distances);
    printf("A eficiência é de %.8f \n",efficiency);
    struct timespec end_time;
    timespec_get(&end_time,TIME_UTC );
    printf("O tempo de execução foi de %.8f s \n",(end_time.tv_sec-start_time.tv_sec)+(end_time.tv_nsec -start_time.tv_nsec) / 1e9);
    graph_destroy(&graph);
    square_matrix_double_free(&distances);

    char* output_file_name=malloc(strlen(argv[1])+strlen(".eff")+1);
    strcpy(output_file_name,argv[1]);
    strcat(output_file_name,".eff");
    FILE* output_file=fopen(output_file_name,"w");
    fprintf(output_file,"%.8f",efficiency);
    fclose(output_file);
    free(output_file_name);

}