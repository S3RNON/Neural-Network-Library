#include "main.h"
#include "XorGate.c"
#include "NumberIdentifier.c"

int main() {

	//int width = 512 * 2;
	//int threshold = 128;
	//
	//float* contents = malloc(sizeof(float) * width * width);

	//for (int i = 0; i < width * width; i++) contents[i] = i % 10;

	//Matrix A = createMatrix(width, width, contents, sizeof(float) * width * width);
	//Matrix B = createMatrix(width, width, contents, sizeof(float) * width * width);

	//Matrix AB_normal = createMatrix(width, width, 0, 0);
	//Matrix AB_strassen = createMatrix(width, width, 0, 0);

	//long long int timeBefore_normal = getTimeInMilliseconds();
	//dotMatrices(AB_normal, A, B);
	//printf("Regular multiplication: %lld ms.\n", getTimeInMilliseconds() - timeBefore_normal);

	//long long int timeBefore_strassen = getTimeInMilliseconds();
	//dotSquareMatrices(AB_strassen, A, B, threshold);
	//printf("Strassen multiplication: %lld ms.\n", getTimeInMilliseconds() - timeBefore_strassen);


	//checkMatrices(AB_normal, AB_strassen);

	////printMatrix(subMatrix(AB_normal, 0, 0, 10, 10), "Regular multiplication");
	////printMatrix(AB_strassen, "Strassen multiplication");

	//freeMatrix(A);
	//freeMatrix(B);
	//freeMatrix(AB_normal);
	//freeMatrix(AB_strassen);
	//free(contents);

	
	return 0;
	//return NumberIdentifier_main();
}