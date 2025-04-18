#include "Main.h"

#ifndef NUMBERIDENTIFIERC_DEF
#define NUMBERIDENTIFIERC_DEF

inline int NumberIdentifier_main() {

	srand(getTimeInMilliseconds());

	// create the network
	int architecture[] = {
		28 * 28, 10, 10
	};
	float hamiltonVals[] = {
		0, 1, 0,
		1, 1, 1,
		0, 1, 0
	};
	Matrix hamilton = createMatrix(3, 3, hamiltonVals, sizeof(float) * 9);
	Neurals network = allocateNeurals(architecture, arrayCount(architecture), &hamilton, 1);

	randomizeNeurals(network, -1.f, 1.f);

	Matrix inputMatrix = createMatrix(1, 1, 0, 0);
	Matrix outputMatrix = createMatrix(1, 10, 0, 0);

	int maxIndex = 1;
	for (int number = 0; number < 10; number++) {
		for (int pngIndex = 0; pngIndex < maxIndex; pngIndex++) {

			char pathname[64] = { 0 };
			sprintf(pathname, "data/numbers-dataset/%d/%d.png", number, pngIndex);

			PNG png = getPNGdata(pathname);

			uint8_t* pixelData = getDecompressedPixelData(png);

			float* pixelData_f = (float*)malloc(png.dimensions.right * png.dimensions.bottom * sizeof(float));
			for (int i = 0; i < png.dimensions.right * png.dimensions.bottom; i++) {
				pixelData_f[i] = 2 * ((float)pixelData[(i + 1) * 4 - 1] / 255.f) - 1;
			}

			free(pixelData);

			inputMatrix.rows = pngIndex + 1 + maxIndex * number;
			inputMatrix.cols = png.dimensions.right * png.dimensions.bottom;
			inputMatrix.stride = png.dimensions.right * png.dimensions.bottom;
			inputMatrix.contents = realloc(inputMatrix.contents, sizeof(float) * inputMatrix.rows * inputMatrix.cols);
			memcpy(inputMatrix.contents + (inputMatrix.rows - 1) * inputMatrix.cols, pixelData_f, sizeof(float) * inputMatrix.cols);

			outputMatrix.rows = pngIndex + 1 + maxIndex * number;
			outputMatrix.contents = realloc(outputMatrix.contents, sizeof(float) * inputMatrix.rows * 10);
			memset(outputMatrix.contents + (inputMatrix.rows - 1) * 10, 0, sizeof(float) * 10);
			matrixAt(outputMatrix, outputMatrix.rows - 1, number) = 1;

			freePNG(png);
			free(pixelData_f);
		}
	}

	//printMatrix(inputMatrix, "input");
	//printMatrix(outputMatrix, "output");

	// train the network
	//long long int timeBefore = getTimeInMilliseconds();
	//trainNeurals(network, inputMatrix, outputMatrix, 1e-3, 10, 1e-3, 100, 1000);
	//printf("Total time: %llu ms\n\nResults:\n", getTimeInMilliseconds() - timeBefore);

	//Matrix AB = createMatrix(inputMatrix.rows, outputMatrix.cols, 0, 0);
	//Matrix AB_n = createMatrix(inputMatrix.rows, outputMatrix.cols, 0, 0);
	//long long int timeBefore = getTimeInMilliseconds();
	//strassenPower2(AB, outputMatrix, inputMatrix, 128);
	//printf("Strassen multiplication: %llu ms\n", getTimeInMilliseconds() - timeBefore);

	//dotMatrices(AB_n, outputMatrix, inputMatrix);
	//checkMatrices(AB, AB_n);

	//printMatrix(inputMatrix, "inputMatrix");

	printNeurals(network);

	//// save the network to disk
	//saveNeurals(network, "numberReader");

	////Neurals network = loadNeurals("numberReader");

	//freeMatrix(inputMatrix);
	//freeMatrix(outputMatrix);
	//freeNeurals(network);
	return 0;
};

#endif