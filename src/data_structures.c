#include "data_structures.h"
#include <stdlib.h>
void matrix_destroy(DistanceMatrix* distance_matrix){
    free(distance_matrix->mem_buffer);
}
double get(DistanceMatrix const* matrix,size_t i,size_t j){
    return matrix->mem_buffer[matrix->N*i+j];
}


void set(DistanceMatrix const* matrix,size_t i,size_t j,double value){
    matrix->mem_buffer[matrix->N*i+j]=value;
}
