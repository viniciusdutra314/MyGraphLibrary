#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DECLARE_SPAN(T, name) \
    typedef struct            \
    {                         \
        T *begin;             \
        size_t N;             \
    } name;

#define DECLARE_VECTOR_INTERFACE(T, name)                 \
    typedef struct                                        \
    {                                                     \
        size_t capacity;                                  \
        size_t size;                                      \
        T *data;                                          \
    } name;                                               \
    void name##_init(name *vector);                       \
    int name##_is_empty(name const *vector);              \
    size_t name##_size(name const *vector);               \
    int name##_reserve(name *vector, size_t capacity);    \
    void name##_push_back(name *vector, T element);       \
    void name##_pop_back(name *vector);                   \
    int name##_resize(name *vector, size_t new_size);     \
    T name##_get(name const *vector, size_t index);       \
    void name##_set(name *vector, size_t index, T value); \
    void name##_free(name *vector);

#define DECLARE_SQUARE_MATRIX_INTERFACE(T, name)                    \
    typedef struct                                                  \
    {                                                               \
        size_t size;                                                \
        T *data;                                                    \
    } name;                                                         \
    int name##_init(name *matrix, size_t size);                     \
    T name##_get(name const *matrix, size_t row, size_t col);       \
    void name##_set(name *matrix, size_t row, size_t col, T value); \
    void name##_free(name *matrix);

#define IMPLEMENT_VECTOR_INTERFACE(T, name)                            \
                                                                       \
    void name##_init(name *vector)                                     \
    {                                                                  \
        vector->capacity = 0;                                          \
        vector->size = 0;                                              \
        vector->data = NULL;                                           \
    }                                                                  \
    int name##_resize(name *vector, size_t new_size)                   \
    {                                                                  \
        if (new_size > vector->capacity)                               \
        {                                                              \
            if (name##_reserve(vector, new_size) != 0)                 \
            {                                                          \
                return 1;                                              \
            }                                                          \
        }                                                              \
        vector->size = new_size;                                       \
        return 0;                                                      \
    }                                                                  \
                                                                       \
    size_t name##_size(name const *vector)                             \
    {                                                                  \
        return vector->size;                                           \
    }                                                                  \
    int name##_is_empty(name const *vector)                            \
    {                                                                  \
        return vector->size == 0;                                      \
    }                                                                  \
    int name##_reserve(name *vector, size_t capacity)                  \
    {                                                                  \
        if (capacity > vector->capacity)                               \
        {                                                              \
            T *new_data = (T *)realloc(vector->data,                   \
                                       capacity * sizeof(T));          \
            if (new_data == NULL)                                      \
            {                                                          \
                fprintf(stderr, "Realocação de memória falhou em uma"  \
                                "operação de reserve de um vetor \n"); \
                return 1;                                              \
            }                                                          \
            vector->data = new_data;                                   \
            vector->capacity = capacity;                               \
        }                                                              \
        return 0;                                                      \
    }                                                                  \
                                                                       \
    void name##_push_back(name *vector, T element)                     \
    {                                                                  \
        if (vector->size >= vector->capacity)                          \
        {                                                              \
            if (vector->capacity == 0)                                 \
            {                                                          \
                name##_reserve(vector, 1);                             \
            }                                                          \
            else                                                       \
            {                                                          \
                name##_reserve(vector, 2 * vector->capacity);          \
            }                                                          \
        }                                                              \
        vector->data[vector->size] = element;                          \
        vector->size++;                                                \
    }                                                                  \
                                                                       \
    T name##_get(name const *vector, size_t index)                     \
    {                                                                  \
        return vector->data[index];                                    \
    }                                                                  \
    void name##_set(name *vector, size_t index, T value)               \
    {                                                                  \
        vector->data[index] = value;                                   \
    }                                                                  \
    void name##_pop_back(name *vector)                                 \
    {                                                                  \
        if (vector->size > 0)                                          \
        {                                                              \
            vector->size--;                                            \
        }                                                              \
    }                                                                  \
    void name##_free(name *vector)                                     \
    {                                                                  \
        free(vector->data);                                            \
        vector->data = NULL;                                           \
        vector->capacity = 0;                                          \
        vector->size = 0;                                              \
    }

#define IMPLEMENT_SQUARE_MATRIX_INTERFACE(T, name)                           \
    int name##_init(name *matrix, size_t size)                               \
    {                                                                        \
        matrix->size = size;                                                 \
        matrix->data = (T *)malloc(size * size * sizeof(T));                 \
        if (matrix->data == NULL)                                            \
        {                                                                    \
            fprintf(stderr, "Falha na alocação de memória para a matriz\n"); \
            matrix->size = 0;                                                \
            return 1;                                                        \
        }                                                                    \
        return 0;                                                            \
    }                                                                        \
    T name##_get(name const *matrix, size_t row, size_t col)                 \
    {                                                                        \
        return matrix->data[row * matrix->size + col];                       \
    }                                                                        \
    void name##_set(name *matrix, size_t row, size_t col, T value)           \
    {                                                                        \
        matrix->data[row * matrix->size + col] = value;                      \
    }                                                                        \
    void name##_free(name *matrix)                                           \
    {                                                                        \
        free(matrix->data);                                                  \
        matrix->data = NULL;                                                 \
        matrix->size = 0;                                                    \
    }

typedef struct
{
    size_t vertex_id;
    double distance;
} VertexWithPriority;

DECLARE_VECTOR_INTERFACE(VertexWithPriority, VecVertexWithPriority)
DECLARE_VECTOR_INTERFACE(size_t, VecSizeT)
typedef struct
{
    VecVertexWithPriority data;
    VecSizeT index_map;
} MinHeap;

void MinHeap_init(MinHeap *heap);
void MinHeap_free(MinHeap *heap);
int MinHeap_is_empty(MinHeap const *heap);
void MinHeap_add(MinHeap *heap, size_t vertex_id, double distance);
void MinHeap_decrease_key(MinHeap *heap, size_t vertex_id, double new_distance);
size_t MinHeap_get(MinHeap *heap);
