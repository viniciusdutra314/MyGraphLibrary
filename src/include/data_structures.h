 /** 
 * As macros neste arquivo permitem a criação de tipos de vetores dinâmicos e matrizes quadradas
 * para qualquer tipo de dado. Isso é alcançado através da geração de código em tempo de pré-processamento,
 * simulando templates de C++
 *
 * - `DECLARE_VECTOR_INTERFACE(T)`: Declara a struct e as assinaturas das funções para um vetor do tipo T.
 * - `IMPLEMENT_VECTOR_INTERFACE(T)`: Fornece a implementação concreta das funções do vetor para o tipo T.
 * - `DECLARE_SQUARE_MATRIX_INTERFACE(T)`: Declara a struct e as assinaturas das funções para uma matriz quadrada do tipo T.
 * - `IMPLEMENT_SQUARE_MATRIX_INTERFACE(T)`: Fornece a implementação concreta das funções da matriz para o tipo T.
 */
#pragma once
#include <stddef.h>

#define DECLARE_VECTOR_INTERFACE(T) \
typedef struct \
{ \
    size_t capacity; \
    size_t size;    \
    T* data;        \
} Vector_##T; \
void vector_##T##_init(Vector_##T* vector); \
int vector_##T##_reserve(Vector_##T* vector, size_t capacity); \
void vector_##T##_push_back(Vector_##T* vector, T element); \
T vector_##T##_get(Vector_##T const* vector, size_t index); \
void vector_##T##_set(Vector_##T* vector, size_t index, T value); \
void vector_##T##_free(Vector_##T* vector);\

#define IMPLEMENT_VECTOR_INTERFACE(T) \
\
void vector_##T##_init(Vector_##T* vector) \
{ \
    vector->capacity = 0; \
    vector->size = 0; \
    vector->data = NULL; \
} \
\
int vector_##T##_reserve(Vector_##T* vector, size_t capacity) \
{ \
    if (capacity > vector->capacity) \
    { \
        T* new_data = (T*)realloc(vector->data, capacity * sizeof(T)); \
        if (new_data == NULL) { \
            fprintf(stderr, "Realocação de memória falhou em uma operação de reserve de um vetor \n"); \
            return 1; \
        } \
        vector->data = new_data; \
        vector->capacity = capacity; \
    } \
    return 0; \
} \
\
void vector_##T##_push_back(Vector_##T* vector, T element) \
{ \
    if (vector->size >= vector->capacity) \
    { \
        size_t new_capacity = vector->capacity == 0 ? 1 : vector->capacity * 2; \
        vector_##T##_reserve(vector, new_capacity); \
    } \
    vector->data[vector->size++] = element; \
} \
\
T vector_##T##_get(Vector_##T const* vector, size_t index) \
{ \
    return vector->data[index]; \
} \
\
void vector_##T##_set(Vector_##T* vector, size_t index, T value) \
{ \
    vector->data[index] = value; \
} \
\
void vector_##T##_free(Vector_##T* vector) \
{ \
    free(vector->data); \
    vector->data = NULL; \
    vector->capacity = 0; \
    vector->size = 0; \
}


#define DECLARE_SQUARE_MATRIX_INTERFACE(T) \
typedef struct { \
    size_t N; \
    Vector_##T inner_vector; \
} SquareMatrix_##T; \
int square_matrix_##T##_init(SquareMatrix_##T *matrix, size_t N); \
T square_matrix_##T##_get(SquareMatrix_##T const* matrix, size_t i, size_t j); \
void square_matrix_##T##_set(SquareMatrix_##T* matrix, size_t i, size_t j, T value); \
void square_matrix_##T##_free(SquareMatrix_##T* matrix);

#define IMPLEMENT_SQUARE_MATRIX_INTERFACE(T) \
int square_matrix_##T##_init(SquareMatrix_##T *matrix, size_t N){ \
   matrix->N=N; \
   Vector_##T inner_vector; \
   vector_##T##_init(&inner_vector); \
   if (vector_##T##_reserve(&inner_vector,N*N)!=0){\
     fprintf(stderr,"Alocação dinâmica da matriz falhou"); \
     return 1; \
   } ;\
   matrix->inner_vector=inner_vector; \
   return 0; \
} \
T square_matrix_##T##_get(SquareMatrix_##T const* matrix, size_t i, size_t j) {\
    return vector_##T##_get(&matrix->inner_vector,i*matrix->N+j); \
} \
void square_matrix_##T##_set(SquareMatrix_##T* matrix, size_t i, size_t j, T value){ \
    vector_##T##_set(&matrix->inner_vector,i*matrix->N+j,value); \
} \
void square_matrix_##T##_free(SquareMatrix_##T* matrix){\
    vector_##T##_free(&matrix->inner_vector); \
}

