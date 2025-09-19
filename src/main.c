#include <stdint.h>
#include <stddef.h>  
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t from;
    size_t to;
} Edge;

typedef double weight;

int order_edges(const void *edge_a, const void *edge_b) {
    const Edge *first  = (const Edge*) edge_a;
    const Edge *second = (const Edge*) edge_b;

    int cmp = (first->from > second->from) - (first->from < second->from);
    if (cmp != 0) {
        return cmp;
    }
    return (first->to > second->to) - (first->to < second->to);
}

int main(){
    FILE* file=fopen("../tests/floyd_warshall/test1.net","r");
    if (file==NULL){
        fprintf(stderr,"O arquivo não conseguiu ser aberto\n");
        return 1;   
    }
    int V,E;
    fscanf(file,"%d",&V);
    fscanf(file,"%d",&E);
    Edge* edge_list=malloc(E*sizeof(Edge));
    weight* weight_array=malloc(E*sizeof(weight));
    uint32_t num_line=0;
    while (!feof(file) && num_line!=E){
        Edge* current_edge=&edge_list[num_line];
        weight* current_weight=&weight_array[num_line];
        int characters_read=fscanf(file," %d %d %lf ",
                                &current_edge->from,
                                &current_edge->to,
                             current_weight);
        if (characters_read!=3){
            fprintf(stderr,"Linha %d com formatação inválida");
            return 1;
        }                    
        num_line++;
    };
    if (num_line!=E){
        fprintf(stderr,"Número de edges incompatível com o arquivo fornecido\n");
        return 1;
    }
    for (int i=0;i<E;i++){
        printf("%d %d %f \n",edge_list[i].from,
        edge_list[i].to,
        weight_array[i]);
    }
    free(weight_array);
    free(edge_list);
    fclose(file);
}