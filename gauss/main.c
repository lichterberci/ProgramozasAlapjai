#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"

typedef struct Matrix {

    double* values;
    int numRows;
    int numCols;

} Matrix;

typedef struct Vector {

    double* values;
    int dimension;

} Vector;

void FreeMatrix (Matrix matrix) {
    free (matrix.values);
}

void FreeVector (Vector vector) {
    free (vector.values);
}

Matrix ZeroMatrix (int numRows, int numCols) {

    Matrix result;
    result.numRows = numRows;
    result.numCols = numCols;

    result.values = malloc (result.numRows * result.numCols * sizeof (double));

    return result;
}

Vector ZeroVector (int dimension) {

    Vector result;

    result.dimension = dimension;
    result.values = malloc (result.dimension * sizeof (double));

    return result;
}

double GetMatrixValueAtPos (Matrix matrix, int row, int col) {

    if (row >= matrix.numRows || col >= matrix.numColumns) {
        fprintf(stderr, "ERROR: index out of range! (%d, %d) for a matrix of dimension (%d, %d)\n", row, col, matrix.numRows, matrix.numColumns);
        exit(-1);
    }

    return matrix.values[row * matrix.numColumns + col];
}

void SetMatrixValueAtPos (Matrix matrix, int row, int col, double val) {

    if (row >= matrix.numRows || col >= matrix.numColumns) {
        fprintf(stderr, "ERROR: index out of range! (%d, %d) for a matrix of dimension (%d, %d)\n", row, col, matrix.numRows, matrix.numColumns);
        exit(-1);
    }

    matrix.values[row * matrix.numColumns + col] = val;
}

void AddRowTimesLambdaToAnother (Matrix matrix, int rowToAdd, int resultRow, double lambda) {

    if (rowToAdd >= matrix.numRows || resultRow >= matrix.numRows) {
        fprintf (stderr, "ERROR: row out of range!\n");
        exit(-1);
    }

    for (size_t c = 0; c < matrix.numCols; c++) {
        double v = GetMatrixValueAtPos(matrix, rowToAdd, c);
        SetMatrixValueAtPos(matrix, resultRow, c, val * lambda);
    }
}

void SwapRows (Matrix matrix, int row1, int row2) {

    if (row1 >= matrix.numRows || row2 >= matrix.numRows) {
        fprintf (stderr, "ERROR: row out of range!\n");
        exit(-1);
    }

    for (size_t c = 0; c < matrix.numCols; c++) {
        double v1 = GetMatrixValueAtPos(matrix, row1, c);
        double v2 = GetMatrixValueAtPos(matrix, row2, c);
        SetMatrixValueAtPos(matrix, row1, c, v2);
        SetMatrixValueAtPos(matrix, row2, c, v1);
    }
}

/// @brief Searches for the first non-zero value in the given column
/// @param startRow included
/// @return -1 if not found
int GetIndexOfNonZero (Matrix matrix, int startRow, int col) {

    for (size_t r = startRow; r < matrix.numRows; r++) {
        if (GetMatrixValueAtPos(matrix, r, col) != 0)
            return r;
    }

    return -1;
}

Vector* GaussEliminate (Matrix matrix, Vector b) {

}

int main () {

    printf("Hello, World!\n");

    return 0;
}