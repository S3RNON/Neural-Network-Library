#pragma once

#ifndef MATRIX_DEF
#define MATRIX_DEF

#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

typedef struct Matrix {
	int rows;
	int cols;
	int stride;
	float* contents;
	short calcSign;

} Matrix;

#define matrixAt(matrix, x, y) (matrix).contents[ (x) + (matrix).stride * (y)]
#define freeMatrix(matrix) free((matrix).contents);

inline float sigmoidf(float x) {

	return 1.f / (1.f + expf(-x));
}

inline Matrix createMatrix(int rows, int cols, float* init, int sizeOfInit) {

	Matrix returnMatrix = { 0 };

	assert(!(rows < 0 || cols < 0));

	returnMatrix.rows = rows;
	returnMatrix.cols = cols;
	returnMatrix.stride = cols;
	returnMatrix.contents = (float*)malloc(sizeof(float) * rows * cols);


	if (init != 0 || returnMatrix.contents == 0) {
		assert(rows * cols == sizeOfInit / sizeof(init[0]));
		memcpy(returnMatrix.contents, init, sizeof(float) * rows * cols);
	}
	else if (rows * cols != 0) {
		memset(returnMatrix.contents, 0, sizeof(float) * rows * cols);
	}

	returnMatrix.calcSign = 1;

	return returnMatrix;
}

#ifdef _INC_STDLIB
inline void randomizeMatrix(Matrix matrix, float lower, float upper) {

	assert(matrix.contents);

	for (int x = 0; x < matrix.cols; x++) {
		for (int y = 0; y < matrix.rows; y++) {
			matrixAt(matrix, x, y) = ((float)rand() / (float)RAND_MAX) * (upper - lower) + lower;

		}
	}

	return;
}
#endif

inline void fillMatrix(Matrix matrix, float number) {

	assert(matrix.contents);

	for (int x = 0; x < matrix.cols; x++) {
		for (int y = 0; y < matrix.rows; y++) {
			matrixAt(matrix, x, y) = number;

		}
	}

	return;
}

// Return a rowmatrix of [matrix].
// WARNING : If the original [matrix] get's freed [free()] then the rowmatrix cannot access the rowdata of the original and thus the returned rowmatix becomes unusable.
// Use copyMatrix() to copy the rowmatrix if the original might get freed
inline Matrix rowMatrix(Matrix matrix, int row) {

	assert(row < matrix.rows);

	Matrix rowMatrix = {
		.cols = matrix.cols,
		.rows = 1,
		.stride = 1,
		.contents = &matrixAt(matrix, 0, row)
	};

	return rowMatrix;
}

// Return a submatrix of [matrix].
// WARNING : If the original [matrix] get's freed [free()] then the submatrix cannot access the data of the original and thus the returned submatix becomes unusable.
// Use copyMatrix() to copy the submatrix if the original might get freed.
inline Matrix subMatrix(Matrix matrix, int xStart, int yStart, int rows, int cols) {

	assert(yStart + rows <= matrix.rows);
	assert(xStart + cols <= matrix.cols);

	Matrix returnMatrix = (Matrix){
		.rows = rows,
		.cols = cols,
		.stride = matrix.stride,
		.calcSign = matrix.calcSign,
		.contents = &matrixAt(matrix, xStart, yStart)
	};

	return returnMatrix;
}

inline void copyMatrix(Matrix destination, Matrix matrix) {

	assert(destination.rows == matrix.rows);
	assert(destination.cols == matrix.cols);

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) = matrixAt(matrix, x, y);
		}
	}

	return;
}

inline void sigmoidMatrix(Matrix matrix) {

	assert(matrix.contents);

	for (int x = 0; x < matrix.cols; x++) {
		for (int y = 0; y < matrix.rows; y++) {
			matrixAt(matrix, x, y) = sigmoidf(matrixAt(matrix, x, y));
		}
	}
}

#ifdef _INC_STDIO
inline void printMatrix(Matrix matrix, char *name) {

	assert(matrix.contents);
	
	printf("%s = [\n", name);
	for (int y = 0; y < matrix.rows; y++) {
		printf("\t");
		for (int x = 0; x < matrix.cols; x++) {
			printf("%.02f ", matrixAt(matrix, x, y));
		}
		printf("\n");
	}
	printf("]\n\n");

	return;
}
#endif

inline void calc_equation_matrices(Matrix destination, int equationMembers,...) {
	
	va_list matricesArguments;
	va_start(matricesArguments, equationMembers);

	Matrix* matrices = malloc(sizeof(Matrix) * equationMembers);

	for (int i = 0; i < equationMembers; i++) {
		matrices[i] = va_arg(matricesArguments, Matrix);
	}

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) = 0;
			for (int i = 0; i < equationMembers; i++) {
				matrixAt(destination, x, y) += matrices[i].calcSign * matrixAt(matrices[i], x, y);
			}
		}
	}

	free(matrices);
	va_end(matricesArguments);
}

inline void subtractMatrices(Matrix destination, Matrix matrix) {
	assert(destination.rows == matrix.rows && destination.cols == matrix.cols);

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) -= matrixAt(matrix, x, y);
		}
	}
}

inline void sumMatrices(Matrix destination, Matrix matrix) {

	assert(!(destination.rows != matrix.rows || destination.cols != matrix.cols));

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) += matrixAt(matrix, x, y);
		}
	}

	return;
}

inline void sumMatrix(Matrix destination, float number) {

	assert(destination.contents);

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) += number;
		}
	}

	return;
}

inline void scaleMatrix(Matrix destination, float scalar) {

	assert(destination.contents);

	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) *= scalar;
		}
	}

	return;
}

inline void dot_2x2strassen(Matrix destination, Matrix matrix1, Matrix matrix2) {

	float p1 = matrixAt(matrix1, 0, 0) * (matrixAt(matrix2, 0, 1) - matrixAt(matrix2, 1, 1));
	float p2 = (matrixAt(matrix1, 0, 0) + matrixAt(matrix1, 0, 1)) * matrixAt(matrix2, 1, 1);
	float p3 = (matrixAt(matrix1, 1, 0) + matrixAt(matrix1, 1, 1)) * matrixAt(matrix2, 0, 0);
	float p4 = matrixAt(matrix1, 1, 1) * (matrixAt(matrix2, 1, 0) - matrixAt(matrix2, 0, 0));
	float p5 = (matrixAt(matrix1, 0, 0) + matrixAt(matrix1, 1, 1)) * (matrixAt(matrix2, 0, 0) + matrixAt(matrix2, 1, 1));
	float p6 = (matrixAt(matrix1, 0, 1) - matrixAt(matrix1, 1, 1)) * (matrixAt(matrix2, 1, 0) + matrixAt(matrix2, 1, 1));
	float p7 = (matrixAt(matrix1, 0, 0) - matrixAt(matrix1, 1, 0)) * (matrixAt(matrix2, 0, 0) + matrixAt(matrix2, 0, 1));

	matrixAt(destination, 0, 0) = p5 + p4 - p2 + p6;
	matrixAt(destination, 0, 1) = p1 + p2;
	matrixAt(destination, 1, 0) = p3 + p4;
	matrixAt(destination, 1, 1) = p1 + p5 - p3 - p7;
}



inline void dotMatrices(Matrix destination, Matrix matrix1, Matrix matrix2) {

	assert(!(matrix1.cols != matrix2.rows));



	for (int x = 0; x < destination.cols; x++) {
		for (int y = 0; y < destination.rows; y++) {
			matrixAt(destination, x, y) = 0;
			for (int i = 0; i < matrix1.cols; i++) {
				matrixAt(destination, x, y) += matrixAt(matrix1, i, y) * matrixAt(matrix2, x, i);
			}
		}
	}

	return;
}

inline void dotSquareMatrices(Matrix destination, Matrix A, Matrix B, int threshold);

inline void strassenPower2(Matrix C, Matrix A, Matrix B, int threshold) {
	
	//if (checkIfEitherZero(A, B)) {
	//	copyMatrix(C, A);
	//	return;
	//}
	
	int n = A.rows;

	if (n <= threshold) {
		// Fallback to naive
		dotMatrices(C, A, B);
		return;
	}

	if (n % 2 == 1) {
		dotSquareMatrices(C, A, B, threshold);
		return;
	}

	int newSize = n / 2;
	float* buffer = malloc(sizeof(float) * newSize * newSize * 9);
	assert(buffer);

	// Submatrices of A
	Matrix A11 = subMatrix(A, 0, 0, newSize, newSize);
	Matrix A12 = subMatrix(A, newSize, 0, newSize, newSize);
	Matrix A21 = subMatrix(A, 0, newSize, newSize, newSize);
	Matrix A22 = subMatrix(A, newSize, newSize, newSize, newSize);

	// Submatrices of B
	Matrix B11 = subMatrix(B, 0, 0, newSize, newSize);
	Matrix B12 = subMatrix(B, newSize, 0, newSize, newSize);
	Matrix B21 = subMatrix(B, 0, newSize, newSize, newSize);
	Matrix B22 = subMatrix(B, newSize, newSize, newSize, newSize);


	Matrix M1 = createMatrix(newSize, newSize, buffer + 0 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M2 = createMatrix(newSize, newSize, buffer + 1 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M3 = createMatrix(newSize, newSize, buffer + 2 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M4 = createMatrix(newSize, newSize, buffer + 3 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M5 = createMatrix(newSize, newSize, buffer + 4 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M6 = createMatrix(newSize, newSize, buffer + 5 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix M7 = createMatrix(newSize, newSize, buffer + 6 * newSize * newSize, sizeof(float) * newSize * newSize);

	Matrix T1 = createMatrix(newSize, newSize, buffer + 7 * newSize * newSize, sizeof(float) * newSize * newSize);
	Matrix T2 = createMatrix(newSize, newSize, buffer + 8 * newSize * newSize, sizeof(float) * newSize * newSize);

	// M1 = (A11 + A22)(B11 + B22)
	calc_equation_matrices(T1, 2, A11, A22);
	calc_equation_matrices(T2, 2, B11, B22);
	strassenPower2(M1, T1, T2, threshold);

	// M2 = (A21 + A22)B11
	calc_equation_matrices(T1, 2, A21, A22);
	strassenPower2(M2, T1, B11, threshold);

	// M3 = A11(B12 - B22)
	B22.calcSign = -1;
	calc_equation_matrices(T1, 2, B12, B22);
	B22.calcSign = 1;
	strassenPower2(M3, A11, T1, threshold);

	// M4 = A22(B21 - B11)
	B11.calcSign = -1;
	calc_equation_matrices(T1, 2, B21, B11);
	B11.calcSign = 1;
	strassenPower2(M4, A22, T1, threshold);

	// M5 = (A11 + A12)B22
	calc_equation_matrices(T1, 2, A11, A12);
	strassenPower2(M5, T1, B22, threshold);

	// M6 = (A21 - A11)(B11 + B12)
	A11.calcSign = -1;
	calc_equation_matrices(T1, 2, A21, A11);
	A11.calcSign = 1;

	calc_equation_matrices(T2, 2, B11, B12);
	strassenPower2(M6, T1, T2, threshold);

	// M7 = (A12 - A22)(B21 + B22)
	A22.calcSign = -1;
	calc_equation_matrices(T1, 2, A12, A22);
	A22.calcSign = 1;
	
	calc_equation_matrices(T2, 2, B21, B22);
	strassenPower2(M7, T1, T2, threshold);

	// C submatrices
	Matrix C11 = subMatrix(C, 0, 0, newSize, newSize);
	Matrix C12 = subMatrix(C, newSize, 0, newSize, newSize);
	Matrix C21 = subMatrix(C, 0, newSize, newSize, newSize);
	Matrix C22 = subMatrix(C, newSize, newSize, newSize, newSize);

	// C11 = M1 + M4 - M5 + M7
	M5.calcSign = -1;
	calc_equation_matrices(C11, 4, M1, M4, M5, M7);
	M5.calcSign = 1;

	// C12 = M3 + M5
	calc_equation_matrices(C12, 2, M3, M5);

	// C21 = M2 + M4
	calc_equation_matrices(C21, 2, M2, M4);

	// C22 = M1 - M2 + M3 + M6
	M2.calcSign = -1;
	calc_equation_matrices(C22, 4, M1, M2, M3, M6);
	M2.calcSign = 1;

	free(buffer);
}


inline void naiveStrasser(Matrix destination, Matrix A, Matrix B, int threshold) {
	int greatestSide = max(max(A.rows, A.cols), max(B.rows, B.cols));

	if (greatestSide < threshold) {
		
		dotMatrices(destination, A, B);
		return;
	}

	int powerTwo = 1;
	while (powerTwo < greatestSide) powerTwo *= 2;
	
	Matrix A_0 = createMatrix(powerTwo, powerTwo, 0, 0);
	Matrix B_0 = createMatrix(powerTwo, powerTwo, 0, 0);
	Matrix dest_0 = createMatrix(powerTwo, powerTwo, 0, 0);

	copyMatrix(subMatrix(A_0, 0, 0, A.rows, A.cols), A);
	copyMatrix(subMatrix(B_0, 0, 0, B.rows, B.cols), B);

	strassenPower2(dest_0, A_0, B_0, threshold);

	copyMatrix(destination, subMatrix(dest_0, 0, 0, destination.rows, destination.cols));

	freeMatrix(A_0);
	freeMatrix(B_0);
	freeMatrix(dest_0);

}

inline void dotSquareMatrices(Matrix destination, Matrix A, Matrix B, int threshold) {
	if (A.cols % 2 == 1) {
		Matrix A_even = createMatrix(A.rows + 1, A.cols + 1, 0, 0);
		copyMatrix(subMatrix(A_even, 0, 0, A.rows, A.cols), A);

		Matrix B_even = createMatrix(B.rows + 1, B.cols + 1, 0, 0);
		copyMatrix(subMatrix(B_even, 0, 0, B.rows, B.cols), B);

		Matrix destination_even = createMatrix(destination.rows + 1, destination.cols + 1, 0, 0);

		strassenPower2(destination_even, A_even, B_even, threshold);

		copyMatrix(destination, subMatrix(destination_even, 0, 0, destination.rows, destination.cols));

		freeMatrix(A_even);
		freeMatrix(B_even);
		freeMatrix(destination_even);

	}
	else {
		strassenPower2(destination, A, B, threshold);
	}

	return;
}

inline float deviation(Matrix expected, Matrix actual) {

	assert(!(expected.cols != actual.cols || expected.rows != expected.rows));

	float totalSum = 0;
	for (int x = 0; x < expected.cols; x++) {
		for (int y = 0; y < expected.rows; y++) {
			totalSum += (matrixAt(expected, x, y) - matrixAt(actual, x, y)) * (matrixAt(expected, x, y) - matrixAt(actual, x, y));
		}
	}

	return totalSum / (float)(expected.cols * expected.rows);
}

inline float checkMatrices(Matrix A, Matrix B) {
	const float EPSILON = 1e-3f;

	assert(!(A.cols != B.cols || A.rows != B.rows));

	for (int x = 0; x < A.cols; x++) {
		for (int y = 0; y < A.rows; y++) {
			float diff = fabsf(matrixAt(A, x, y) - matrixAt(B, x, y));
			if (diff > EPSILON) {
				printf("Mismatch at (%d, %d): A=%.5f B=%.5f Diff=%.5f\n",
					x, y, matrixAt(A, x, y), matrixAt(B, x, y), diff);
				return 0;
			}
		}
	}

	return 1;
}

inline int checkIfEitherZero(Matrix A, Matrix B) {
	assert(!(A.cols != B.cols || A.rows != B.rows));

	for (int x = 0; x < A.cols; x++) {
		for (int y = 0; y < A.rows; y++) {
			if (matrixAt(A, x, y) != 0 || matrixAt(B, x, y)) return 0;
		}
	}

	return 1;
}

#endif