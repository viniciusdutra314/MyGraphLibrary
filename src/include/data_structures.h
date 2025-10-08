#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
    T name##_get(name const *vector, size_t index);       \
    void name##_set(name *vector, size_t index, T value); \
    void name##_free(name *vector);

#define DECLARE_VECTOR_OF_VECTORS_INTERFACE(T, name)       \
    typedef struct                                         \
    {                                                      \
        size_t capacity;                                   \
        size_t size;                                       \
        T *data;                                           \
    } name;                                                \
    void name##_init(name *vec_vec);                       \
    int name##_is_empty(name const *vec_vec);              \
    size_t name##_size(name const *vec_vec);               \
    int name##_reserve(name *vec_vec, size_t capacity);    \
    T *name##_get_ptr(name *vec_vec, size_t index);        \
    T name##_get(name const *vec_vec, size_t index);       \
    void name##_set(name *vec_vec, size_t index, T value); \
    void name##_free(name *vec_vec);

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

#define IMPLEMENT_VECTOR_OF_VECTORS_INTERFACE(T, name)                        \
    void name##_init(name *vec_vec)                                           \
    {                                                                         \
        vec_vec->capacity = 0;                                                \
        vec_vec->size = 0;                                                    \
        vec_vec->data = NULL;                                                 \
    }                                                                         \
    int name##_is_empty(name const *vector)                                   \
    {                                                                         \
        return vector->size == 0;                                             \
    }                                                                         \
    size_t name##_size(name const *vector)                                    \
    {                                                                         \
        return vector->size;                                                  \
    }                                                                         \
    int name##_reserve(name *vec_vec, size_t capacity)                        \
    {                                                                         \
        T *data = malloc(sizeof(T) * capacity);                               \
        if (data == NULL)                                                     \
        {                                                                     \
            fprintf(stderr, "Realocação de memória falhou em uma"             \
                            "operação de reserve de um vetor de vetores \n"); \
            return 1;                                                         \
        };                                                                    \
        vec_vec->size = capacity;                                             \
        for (size_t i = 0; i < capacity; i++)                                 \
        {                                                                     \
            T##_init(data + i);                                               \
        };                                                                    \
        return 0;                                                             \
    }                                                                         \
    T *name##_get_ptr(name *vec_vec, size_t index)                            \
    {                                                                         \
        return vec_vec->data + index;                                         \
    }                                                                         \
    T name##_get(name const *vec_vec, size_t index)                       \
    {                                                                         \
        return vec_vec->data[index];                                          \
    }                                                                         \
    void name##_free(name *vec_vec)                                           \
    {                                                                         \
        for (size_t i = 0; i < vec_vec->size; ++i)                            \
        {                                                                     \
            T##_free(vec_vec->data + i);                                      \
        }                                                                     \
        free(vec_vec->data);                                                  \
        vec_vec->data = NULL;                                                 \
        vec_vec->capacity = 0;                                                \
        vec_vec->size = 0;                                                    \
    }

#define __IMPLEMENT_VECTOR_INTERFACE_WITH_DELETER(T, name, DELETER) \
    void name##_set(name *vector, size_t index, T value)            \
    {                                                               \
        DELETER(&vector->data[index]);                              \
        vector->data[index] = value;                                \
    }                                                               \
    void name##_pop_back(name *vector)                              \
    {                                                               \
        if (vector->size > 0)                                       \
        {                                                           \
            DELETER(&vector->data[vector->size - 1]);               \
            vector->size--;                                         \
        }                                                           \
    }                                                               \
    void name##_free(name *vector)                                  \
    {                                                               \
        for (size_t i = 0; i < vector->size; ++i)                   \
        {                                                           \
            DELETER(&vector->data[i]);                              \
        }                                                           \
        free(vector->data);                                         \
        vector->data = NULL;                                        \
        vector->capacity = 0;                                       \
        vector->size = 0;                                           \
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
