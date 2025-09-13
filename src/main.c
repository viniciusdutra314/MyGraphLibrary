#include <stdint.h>
#include <stddef.h>  
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t from;
    size_t to;
} Edge;

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
    Edge array[5]={{0,10},{3,4},{4,6},{6,1},{1,4}};
    qsort(array, 5, sizeof(Edge), order_edges);

    for (int i=0; i<5; i++){
        printf("(%zu,%zu)\n", array[i].from, array[i].to);
    }

    Edge key = {4, 6};
    Edge *found = bsearch(&key, array, 5, sizeof(Edge), order_edges);

    if (found) {
        printf("Found: (%zu,%zu)\n", found->from, found->to);
    } else {
        printf("Not found\n");
    }
}