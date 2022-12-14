#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#define NO_RESULT           0
#define INFINITE_RESULTS    1

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

Matrix CopyMatrix (Matrix matrix) {
    Matrix result = matrix;
    result.values = malloc(result.numRows * result.numCols * sizeof(double));
    memcpy(result.values, matrix.values, result.numRows * result.numCols * sizeof(double));
    return result;
}

Vector CopyVector (Vector vector) {
    Vector result = vector;
    result.values = malloc(result.dimension * sizeof(double));
    memcpy(result.values, vector.values, result.dimension * sizeof(double));
    return result;
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

    if (row >= matrix.numRows || col >= matrix.numCols) {
        fprintf(stderr, "ERROR: index out of range! (%d, %d) for a matrix of dimension (%d, %d)\n", row, col, matrix.numRows, matrix.numCols);
        exit(-1);
    }

    return matrix.values[row * matrix.numCols + col];
}

void SetMatrixValueAtPos (Matrix matrix, int row, int col, double val) {

    if (row >= matrix.numRows || col >= matrix.numCols) {
        fprintf(stderr, "ERROR: index out of range! (%d, %d) for a matrix of dimension (%d, %d)\n", row, col, matrix.numRows, matrix.numCols);
        exit(-1);
    }

    matrix.values[row * matrix.numCols + col] = val;
}

double GetVectorValueAtPos (Vector vector, int i) {

    if (i >= vector.dimension) {
        fprintf (stderr, "ERROR: index out of range!\n");
        exit(-1);
    }

    return vector.values[i];
}

void SetVectorValueAtPos (Vector vector, int i, double val) {

    if (i >= vector.dimension) {
        fprintf (stderr, "ERROR: index out of range!\n");
        exit(-1);
    }

    vector.values[i] = val;
}

void PrintMatrix (Matrix matrix) {
    for (size_t r = 0; r < matrix.numRows; r++) {
        for (size_t c = 0; c < matrix.numCols; c++) {    
            printf("%2.2lf ", GetMatrixValueAtPos(matrix, r, c));
        }
        printf("\n");
    }
}

void PrintVector (Vector vector) {
    for (size_t i = 0; i < vector.dimension; i++) {
        printf("%2.2lf\n", GetVectorValueAtPos(vector, i));
    }
}

void AddRowTimesLambdaToAnother (Matrix matrix, int rowToAdd, int resultRow, double lambda) {

    if (rowToAdd >= matrix.numRows || resultRow >= matrix.numRows) {
        fprintf (stderr, "ERROR: row out of range!\n");
        exit(-1);
    }

    for (size_t c = 0; c < matrix.numCols; c++) {
        double v1 = GetMatrixValueAtPos(matrix, rowToAdd, c) * lambda;
        double v2 = GetMatrixValueAtPos(matrix, resultRow, c);
        SetMatrixValueAtPos(matrix, resultRow, c, v1 + v2);
    }
}

void AddValueTimeLambdaToAnother (Vector vector, int indexToAdd, int resultIndex, double lambda) {

    if (indexToAdd >= vector.dimension || resultIndex >= vector.dimension) {
        fprintf (stderr, "ERROR: indexToAdd out of range!\n");
        exit(-1);
    }

    double v1 = GetVectorValueAtPos(vector, indexToAdd) * lambda;
    double v2 = GetVectorValueAtPos(vector, resultIndex);
    SetVectorValueAtPos(vector, resultIndex, v1 + v2);
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

void SwapValues (Vector vector, int i, int j) {

    if (i >= vector.dimension || j >= vector.dimension) {
        fprintf(stderr, "ERROR: index out of range!\n");
        exit(-1);
    }

    double v = GetVectorValueAtPos(vector, i);
}

void MultiplyRowByLambda (Matrix matrix, int row, double lambda) {

    if (row >= matrix.numRows) {
        fprintf (stderr, "ERROR: row out of range!\n");
        exit(-1);
    }

    for (size_t c = 0; c < matrix.numCols; c++) {
        double v = GetMatrixValueAtPos(matrix, row, c);
        v *= lambda;
        SetMatrixValueAtPos(matrix, row, c, v);
    }
}

void MultiplyVectorValueByLambda (Vector vector, int i, double lambda) {

    if (i >= vector.dimension) {
        fprintf(stderr, "ERROR: index out of range!\n");
        exit(-1);
    }

    double v = GetVectorValueAtPos(vector, i);
    v *= lambda;
    SetVectorValueAtPos(vector, i, v);
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

bool IsZeroRow (Matrix matrix, int row) {

    if (row >= matrix.numRows) {
        fprintf(stderr, "ERROR: row out of range!\n");
        exit(-1);
    }

    for (size_t c = 0; c < matrix.numCols; c++)
        if (GetMatrixValueAtPos(matrix, row, c) != 0)
            return false;

    return true;
}

bool IsVectorZero (Vector vector) {
    for (size_t i = 0; i < vector.dimension; i++)
        if (GetVectorValueAtPos(vector, i) != 0)
            return false;

    return true;
}

int GetLastNonZeroRow (Matrix matrix) {
    for (int row = matrix.numRows - 1; row >= 0; row--)
        if (IsZeroRow(matrix, row) == false)
            return row;

    return -1;
}

Vector* GaussEliminate (Matrix A, Vector b) {

    Matrix matrix = CopyMatrix(A);
    Vector vector = CopyVector(b);

    int row = 0;
    int col = 0;

    // making the upper-triangle form
    while (row < matrix.numRows && col < matrix.numCols) {

        printf("row: %d, col: %d\n", row, col);
        PrintMatrix(matrix);
        PrintVector(vector);

        double leadingTerm = GetMatrixValueAtPos(matrix, row, col);

        if (leadingTerm == 0) {

            if (IsZeroRow(matrix, row)) {

                if (GetVectorValueAtPos(vector, row) != 0) {
                    // 0x+0y+0z != 0
                    // --> there is no solution
                    FreeMatrix(matrix);
                    FreeVector(vector);
                    return (Vector*)NO_RESULT;
                }

                int rowToSwap = GetLastNonZeroRow (matrix);

                printf("Row is zero, it is ok, row: %d, swap: %d\n", row, rowToSwap);

                if (rowToSwap <= row) {
                    //0x=0
                    FreeMatrix(matrix);
                    FreeVector(vector);
                    return (Vector*)INFINITE_RESULTS;
                }

                if (rowToSwap == -1) {
                    // matrix is null matrix
                    if (IsVectorZero(vector) == false) {
                        // 0x!=0
                        FreeMatrix(matrix);
                        FreeVector(vector);
                        return (Vector*)NO_RESULT;
                    }

                    FreeMatrix(matrix);
                    FreeVector(vector);
                    // 0x=0
                    return (Vector*)INFINITE_RESULTS;
                }

                SwapRows(matrix, row, rowToSwap);
                SwapValues(vector, row, rowToSwap);

                continue;    
            }

            int rowToSwap = GetIndexOfNonZero (matrix, row + 1, col);

            // there is no leading 1 in this column
            if (rowToSwap == -1) {
                col++;
                continue;
            }

            SwapRows(matrix, row, rowToSwap);
            SwapValues(vector, row, rowToSwap);    
            
            continue; // without changing the column or row index
        }

        // we have a valid leading-term

        if (leadingTerm != 1.0) {
            // adjust this row

            MultiplyRowByLambda(matrix, row, 1.0 / leadingTerm);
            MultiplyVectorValueByLambda(vector, row, 1.0 / leadingTerm);
        }

        // we have a leading 1

        for (size_t adjustedRow = row + 1; adjustedRow < matrix.numRows; adjustedRow++) {
            
            // subtract the current row from the others (and multiply accordingly)

            double leadingTermInAdjustedRow = GetMatrixValueAtPos(matrix, adjustedRow, col);

            AddRowTimesLambdaToAnother (matrix, row, adjustedRow, -leadingTermInAdjustedRow);
            AddValueTimeLambdaToAnother (vector, row, adjustedRow, -leadingTermInAdjustedRow);
        }

        // we are ready to continue to the next row and column
        row++;
        col++;
        continue;
    }

    // we have an upper-triangle

    /*
    
    eg:

    [[1, 2],
     [0, 1]]
    
    */

    if (row != matrix.numRows) {
        // there are 0 rows beneath
        // there are infinite solutions
        FreeMatrix(matrix);
        FreeVector(vector);
        return (Vector*)INFINITE_RESULTS;
    }


    // in the previous loop, we added +1, +1, but we want to revert these
    col--;
    row--;

    // work towards reduced diagonal form
    while (row >= 0 && col >= 0) {

        // adjust rows

        for (int adjustedRow = row - 1; adjustedRow >= 0; adjustedRow--) {
            
            // subtract the current row from the others (and multiply accordingly)

            double leadingTermInAdjustedRow = GetMatrixValueAtPos(matrix, adjustedRow, col);

            AddRowTimesLambdaToAnother (matrix, row, adjustedRow, -leadingTermInAdjustedRow);
            AddValueTimeLambdaToAnother (vector, row, adjustedRow, -leadingTermInAdjustedRow);
        }

        col--;
        row--;
    }

    Vector* result = malloc(sizeof(Vector));
    memcpy(result, &vector, sizeof(Vector));

    FreeMatrix(matrix);

    return result;
}

int main () {

    Matrix A = ZeroMatrix (3, 3);
    Vector b = ZeroVector (3);

    SetMatrixValueAtPos(A, 0, 0, 1);
    SetMatrixValueAtPos(A, 0, 1, 0);
    SetMatrixValueAtPos(A, 0, 2, 0);
    SetMatrixValueAtPos(A, 1, 0, 0);
    SetMatrixValueAtPos(A, 1, 1, 0);
    SetMatrixValueAtPos(A, 1, 2, 0);
    SetMatrixValueAtPos(A, 2, 0, 0);
    SetMatrixValueAtPos(A, 2, 1, 0);
    SetMatrixValueAtPos(A, 2, 2, 1);

    SetVectorValueAtPos(b, 0, 1);
    SetVectorValueAtPos(b, 1, 0);
    SetVectorValueAtPos(b, 2, 0);

    PrintMatrix(A);
    PrintVector(b);

    Vector* result = GaussEliminate(A, b);

    if (result == NO_RESULT) {
        printf("No result!\n");
    } else if (result == INFINITE_RESULTS) {
        printf("Infinite results!\n");
    } else {
        printf("Result:\n");
        PrintVector(*result);
        FreeVector(*result);
        free(result);
    }

    return 0;
}