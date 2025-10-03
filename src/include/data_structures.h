#pragma once
#include <stddef.h>
/**
 * @brief Represents a square matrix of doubles, used to store distances between vertices.
 * @param N The dimension of the square matrix (N x N).
 * @param mem_buffer A pointer to the memory buffer storing the matrix elements in row-major order.
 */
typedef struct {
    size_t N;
    double* mem_buffer; 
} DistanceMatrix;

/**
 * @brief Retrieves the value at a specific row and column in the distance matrix.
 * @param matrix A pointer to the DistanceMatrix.
 * @param i The row index.
 * @param j The column index.
 * @return The value at matrix[i][j].
 */
double get(DistanceMatrix const* matrix,size_t i,size_t j);
/**
 * @brief Sets the value at a specific row and column in the distance matrix.
 * @param matrix A pointer to the DistanceMatrix.
 * @param i The row index.
 * @param j The column index.
 * @param value The value to set at matrix[i][j].
 */
void set(DistanceMatrix const* matrix,size_t i,size_t j,double value);
/**
 * @brief Frees the memory allocated for the distance matrix's buffer.
 * @param distance_matrix A pointer to the DistanceMatrix to be destroyed.
 */


void matrix_destroy(DistanceMatrix* distance_matrix);
