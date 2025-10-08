#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DECLARE_VECTOR_INTERFACE(T, name)                   \
    typedef struct                                          \
    {                                                       \
        uint64_t capacity;                                  \
        uint64_t size;                                      \
        T *data;                                            \
    } name;                                                 \
    void name##_init(name *vector);                         \
    int name##_is_empty(name const *vector);                \
    uint64_t name##_size(name const *vector);               \
    int name##_reserve(name *vector, uint64_t capacity);    \
    void name##_push_back(name *vector, T element);         \
    void name##_pop_back(name *vector);                     \
    T name##_get(name const *vector, size_t index);         \
    void name##_set(name *vector, uint64_t index, T value); \
    void name##_free(name *vector);

#define DECLARE_SQUARE_MATRIX_INTERFACE(T, name)                      \
    typedef struct                                                    \
    {                                                                 \
        uint64_t size;                                                \
        T *data;                                                      \
    } name;                                                           \
    int name##_init(name *matrix, uint64_t size);                     \
    T name##_get(name const *matrix, size_t row, size_t col);         \
    void name##_set(name *matrix, uint64_t row, size_t col, T value); \
    void name##_free(name *matrix);

#define __IMPLEMENT_VECTOR_CORE_INTERFACE(T, name)                                                         \
                                                                                                           \
    void name##_init(name *vector)                                                                         \
    {                                                                                                      \
        vector->capacity = 0;                                                                              \
        vector->size = 0;                                                                                  \
        vector->data = NULL;                                                                               \
    }                                                                                                      \
    uint64_t name##_size(name const *vector)                                                               \
    {                                                                                                      \
        return vector->size;                                                                               \
    }                                                                                                      \
    int name##_is_empty(name const *vector)                                                                \
    {                                                                                                      \
        return vector->size == 0;                                                                          \
    }                                                                                                      \
    int name##_reserve(name *vector, uint64_t capacity)                                                    \
    {                                                                                                      \
        if (capacity > vector->capacity)                                                                   \
        {                                                                                                  \
            T *new_data = (T *)realloc(vector->data, capacity * sizeof(T));                                \
            if (new_data == NULL)                                                                          \
            {                                                                                              \
                fprintf(stderr, "Realocação de memória falhou em uma operação de reserve de um vetor \n"); \
                return 1;                                                                                  \
            }                                                                                              \
            vector->data = new_data;                                                                       \
            vector->capacity = capacity;                                                                   \
        }                                                                                                  \
        return 0;                                                                                          \
    }                                                                                                      \
                                                                                                           \
    void name##_push_back(name *vector, T element)                                                         \
    {                                                                                                      \
        if (vector->size >= vector->capacity)                                                              \
        {                                                                                                  \
            uint64_t new_capacity = vector->capacity == 0 ? 1 : vector->capacity * 2;                      \
            name##_reserve(vector, new_capacity);                                                          \
        }                                                                                                  \
        vector->data[vector->size++] = element;                                                            \
    }                                                                                                      \
                                                                                                           \
    T name##_get(name const *vector, size_t index)                                                         \
    {                                                                                                      \
        return vector->data[index];                                                                        \
    }

#define __IMPLEMENT_VECTOR_INTERFACE_NO_DELETER(T, name)   \
    void name##_set(name *vector, uint64_t index, T value) \
    {                                                      \
        vector->data[index] = value;                       \
    }                                                      \
    void name##_pop_back(name *vector)                     \
    {                                                      \
        if (vector->size > 0)                              \
        {                                                  \
            vector->size--;                                \
        }                                                  \
    }                                                      \
    void name##_free(name *vector)                         \
    {                                                      \
        free(vector->data);                                \
        vector->data = NULL;                               \
        vector->capacity = 0;                              \
        vector->size = 0;                                  \
    }

#define __IMPLEMENT_VECTOR_INTERFACE_WITH_DELETER(T, name, DELETER) \
    void name##_set(name *vector, uint64_t index, T value)          \
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
        for (uint64_t i = 0; i < vector->size; ++i)                 \
        {                                                           \
            DELETER(&vector->data[i]);                              \
        }                                                           \
        free(vector->data);                                         \
        vector->data = NULL;                                        \
        vector->capacity = 0;                                       \
        vector->size = 0;                                           \
    }

#define IMPLEMENT_VECTOR_INTERFACE(T, name)    \
    __IMPLEMENT_VECTOR_CORE_INTERFACE(T, name) \
    __IMPLEMENT_VECTOR_INTERFACE_NO_DELETER(T, name)

#define IMPLEMENT_VECTOR_INTERFACE_WITH_DELETER(T, name, DELETER) \
    __IMPLEMENT_VECTOR_CORE_INTERFACE(T, name)                    \
    __IMPLEMENT_VECTOR_INTERFACE_WITH_DELETER(T, name, DELETER)

#define IMPLEMENT_SQUARE_MATRIX_INTERFACE(T, name)                           \
    int name##_init(name *matrix, uint64_t size)                             \
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
    void name##_set(name *matrix, uint64_t row, size_t col, T value)         \
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
    uint64_t vertex_id;
    double distance;
} VertexDistance;

DECLARE_VECTOR_INTERFACE(VertexDistance, VecVertexDistance)

typedef struct
{
    VecVertexDistance data;
} MinHeap;

void MinHeap_init(MinHeap *heap);
void MinHeap_free(MinHeap *heap);
int MinHeap_is_empty(MinHeap const *heap);
void MinHeap_add(MinHeap *heap, uint64_t vertex_id, double distance);
uint64_t MinHeap_get(MinHeap *heap);
