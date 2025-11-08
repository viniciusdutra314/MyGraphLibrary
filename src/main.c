#include <stdio.h>
#include <time.h>
#include "data_structures.h"
#include "graph_library.h"
#include <string.h>

int main(int argc,char** argv){
    if (argc!=2){
        fprintf(stderr,"Erro: um arquivo de um grafo no formato "
            "de edgelist deve ser fornecido \n");
        return 1;
    }
    Graph graph;
    if (Graph_read_edgelist(&graph,argv[1]) != 0) {
        fprintf(stderr, "Erro lendo o arquivo edgelist \n");
        return 1;
    }
    Graph_create_adjacency_list(&graph);
    struct timespec start_time;
    timespec_get(&start_time,TIME_UTC);
    double efficiency=0;
    for (size_t source=0;source<graph.V;source++){   
        VecDouble distances;
        VecDouble_init(&distances); 
        if (dijkstra(&graph,source,&distances) != 0) {
            fprintf(stderr, "Erro na execuação do algoritmo de Dijkstra \n");
            return 1;
        }
        
        for (size_t i=0;i<graph.V;i++){
            double distance=VecDouble_get(&distances,i);
            if (distance!=0){
                efficiency+=1.0/(distance);
            }
        }

        VecDouble_free(&distances);
    }
    efficiency/=(graph.V*(graph.V-1));
    printf("Efficiency: %.8f \n",efficiency);
    struct timespec end_time;
    timespec_get(&end_time,TIME_UTC );
    printf("O tempo de execução foi de %.8f s \n",(end_time.tv_sec-start_time.tv_sec)+(end_time.tv_nsec -start_time.tv_nsec) / 1e9);
    Graph_destroy(&graph);

    char* output_file_name=malloc(strlen(argv[1])+strlen(".eff")+1);
    strcpy(output_file_name,argv[1]);
    strcat(output_file_name,".eff");
    FILE* output_file=fopen(output_file_name,"w");
    fprintf(output_file,"%.8f",efficiency);
    fclose(output_file);
    free(output_file_name);

}