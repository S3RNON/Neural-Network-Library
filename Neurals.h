#pragma once

#ifndef NEURALS_DEF
#define NEURALS_DEF

#ifndef _NEURAL_MATRIX
#define _NEURAL_MATRIX

#include "Matrix.h"

#endif

#ifndef _NEURAL_ALLOC
#ifndef _NEURAL_PRINT

#include <stdlib.h>

#define _NEURAL_PRINT(string, ...) printf((string), __VA_ARGS__)
#define _NEURAL_ALLOC(size) malloc((size))

#endif
#endif

typedef struct Neurals {

	int layerCount;
	int* architecture;
	Matrix* weights;
	Matrix* biases;
	Matrix* activations;
	Matrix* convolutions;
	int convolutionCount;
	void (*forwardFunction)(struct Neurals network);

} Neurals;

#define arrayCount(arr) sizeof((arr))/sizeof((arr)[0]) 

inline void forwardNeurals(Neurals network) {

	for (int i = 0; i < network.layerCount; i++) {

		dotMatrices(network.activations[i + 1], network.activations[i], network.weights[i]);
		//naiveStrasser(network.activations[i + 1], network.activations[i], network.weights[i], 512);
		sumMatrices(network.activations[i + 1], network.biases[i]);
		sigmoidMatrix(network.activations[i + 1]);
		scaleMatrix(network.activations[i + 1], 1.f);
	}

	return;
}

inline Neurals allocateNeurals(int* architecture, int architectureCount, Matrix* convolutions, int convolutionsCount) {

	assert(architectureCount > 0);
	assert(convolutionsCount < architectureCount);

	Neurals network;
	network.layerCount = architectureCount - 1;

	network.architecture = malloc(sizeof(architectureCount) * architectureCount);
	for (int i = 0; i < architectureCount; i++) {
		network.architecture[i] = architecture[i];
	}

	network.weights = _NEURAL_ALLOC(sizeof(*network.weights) * network.layerCount);
	assert(network.weights != 0);

	network.biases = _NEURAL_ALLOC(sizeof(*network.biases) * network.layerCount);
	assert(network.biases != 0);

	network.activations = _NEURAL_ALLOC(sizeof(*network.activations) * (network.layerCount + 1));
	assert(network.activations != 0);

	network.activations[0] = createMatrix(1, architecture[0], 0, 0);

	if (!(convolutions == 0 || convolutionsCount == 0)) {
		network.convolutionCount = convolutionsCount;
		network.convolutions = _NEURAL_ALLOC(sizeof(*network.convolutions) * network.convolutionCount);
		for (int i = 0; i < convolutionsCount; i++) {
			network.convolutions[i] = createMatrix(convolutions[i].rows, convolutions[i].cols, convolutions[i].contents, sizeof(float) * convolutions[i].rows * convolutions[i].cols);
		}

		network.forwardFunction = forwardNeurals;

		for (int i = 1; i < architectureCount; i++) {
			int convolutedColumnCount = network.activations[i - 1].cols;
			//	(network.activations[i - 1].cols - network.convolutions[ i % network.convolutionCount ].cols + 1) *
			//	(network.activations[i - 1].cols - network.convolutions[ i % network.convolutionCount ].cols + 1);
			network.weights[i - 1] = createMatrix(convolutedColumnCount, architecture[i], 0, 0);
			network.biases[i - 1] = createMatrix(1, architecture[i], 0, 0);
			network.activations[i] = createMatrix(1, architecture[i], 0, 0);
		}
	}
	else {
		network.forwardFunction = forwardNeurals;

		for (int i = 1; i < architectureCount; i++) {
			network.weights[i - 1] = createMatrix(network.activations[i - 1].cols, architecture[i], 0, 0);
			network.biases[i - 1] = createMatrix(1, architecture[i], 0, 0);
			network.activations[i] = createMatrix(1, architecture[i], 0, 0);
		}
	}



	return network;
}

inline void freeNeurals(Neurals network) {

	for (int i = 0; i < network.layerCount; i++) {
		freeMatrix(network.weights[i]);
		freeMatrix(network.biases[i]);
		freeMatrix(network.activations[i]);
	}
	freeMatrix(network.activations[network.layerCount]);

	free(network.architecture);
	free(network.weights);
	free(network.biases);
	free(network.activations);
}

inline void randomizeNeurals(Neurals network, float lower, float upper) {

	for (int i = 0; i < network.layerCount; i++) {
		randomizeMatrix(network.weights[i], lower, upper);
		randomizeMatrix(network.biases[i] , lower, upper);
	}

	return;
}

inline float costNeurals(Neurals network, Matrix trainingInput, Matrix trainingOutput) {

	assert(trainingInput.rows == trainingOutput.rows);

	float costSum = 0;
	for (int i = 0; i < trainingInput.rows; i++) {

		Matrix X = rowMatrix(trainingInput, i);
		Matrix Y = rowMatrix(trainingOutput, i);
		//printMatrix(X, "X");
		//printMatrix(Y, "Y");

		copyMatrix(network.activations[0], X);

		network.forwardFunction(network);

		for (int j = 0; j < trainingOutput.cols; j++) {
			float difference = matrixAt(network.activations[network.layerCount], j, 0) - matrixAt(Y, j, 0);
			costSum += difference * difference;
		}
	}

	//printf("cost: %f\n", costSum / trainingInput.rows);
	return costSum / trainingInput.rows;
}

inline void finiteDifferenceNeurals(Neurals network, Neurals gradientNetwork, float epsilon, Matrix trainingInput, Matrix trainingOutput) {

	float saved = 0;
	float costValue = costNeurals(network, trainingInput, trainingOutput);

	for (int i = 0; i < network.layerCount; i++) {
		for (int x = 0; x < network.weights[i].cols; x++) {
			for (int y = 0; y < network.weights[i].rows; y++) {
				//printf("working on weight %d %d in layer %d\n", x, y, i);
				saved = matrixAt(network.weights[i], x, y);
				matrixAt(network.weights[i], x, y) += epsilon;
				matrixAt(gradientNetwork.weights[i], x, y) = (costNeurals(network, trainingInput, trainingOutput) - costValue) / epsilon;
				matrixAt(network.weights[i], x, y) = saved;
			}
		}

		for (int x = 0; x < network.biases[i].cols; x++) {
			for (int y = 0; y < network.biases[i].rows; y++) {
				saved = matrixAt(network.biases[i], x, y);
				matrixAt(network.biases[i], x, y) += epsilon;
				matrixAt(gradientNetwork.biases[i], x, y) = (costNeurals(network, trainingInput, trainingOutput) - costValue) / epsilon;
				matrixAt(network.biases[i], x, y) = saved;
			}
		}
	}

	return;
}

inline void applyGradientNeurals(Neurals network, Neurals gradientNetwork, float rate) {

	for (int i = 0; i < network.layerCount; i++) {
		for (int x = 0; x < network.weights[i].cols; x++) {
			for (int y = 0; y < network.weights[i].rows; y++) {
				matrixAt(network.weights[i], x, y) -= rate * matrixAt(gradientNetwork.weights[i], x, y);;
			}
		}

		for (int x = 0; x < network.biases[i].cols; x++) {
			for (int y = 0; y < network.biases[i].rows; y++) {
				matrixAt(network.biases[i], x, y) -= rate * matrixAt(gradientNetwork.biases[i], x, y);;
			}
		}
	}

	return;
}

inline void trainNeurals(Neurals network, Matrix trainingInput, Matrix trainingOutput, float epsilon, float rate, float costTarget, float costMax, float maxSteps) {

	Neurals gradientNetwork = allocateNeurals(network.architecture, network.layerCount + 1, 0, 0);

	long long int steps = 0;
	float costValue = 10;

	while (costValue > costTarget && costValue < costMax && steps < maxSteps) {
		costValue = costNeurals(network, trainingInput, trainingOutput);
		printf("%f\n", costValue);

		finiteDifferenceNeurals(network, gradientNetwork, epsilon, trainingInput, trainingOutput);

		applyGradientNeurals(network, gradientNetwork, rate);

		steps++;
	}
	//printf("%f%%\n", 100 - 100 * costValue);

	freeNeurals(gradientNetwork);
}

inline Matrix inputNeurals(Neurals network, Matrix inputs) {
	assert(inputs.cols == network.activations[0].cols);

	copyMatrix(network.activations[0], inputs);

	forwardNeurals(network);

	return network.activations[network.layerCount];
}


inline void printNeurals(Neurals network) {

	char buffer[32];
	for (int i = 0; i < network.layerCount; i++) {
		printf("------------------------------\nLayer %d:\n\n", i);
		printMatrix(network.weights[i], "Weight");
		printMatrix(network.biases[i], "Bias");
		printMatrix(network.activations[i], "Activation");
	}
	printf("------------------------------\nLayer %d:\n\n", network.layerCount);
	printMatrix(network.activations[network.layerCount], "Activation");

	return;
}

inline void drawNeurals(Neurals network) {

	// 0 \
	//	  |-> 0
	// 0 /





	return;
}

inline int saveNeurals(Neurals network, char* networkName) {

	char location[64];

	sprintf(location, "networks/%s.nnd", networkName);

	FILE* file = fopen(location, "wb");
	size_t bitsWritten = 0;

	// save layerCount to .nnd
	fwrite(&network.layerCount, sizeof(network.layerCount), 1, file);

	// save architecture to .nnd
	fwrite(network.architecture, sizeof(network.architecture[0]), network.layerCount + 1, file);
	
	// get all weights and save them to .nnd

	for (int i = 0; i < network.layerCount; i++) {
		fwrite(
			network.weights[i].contents, 
			sizeof(network.weights[i].contents[0]),
			network.weights[i].rows * network.weights[i].cols,
			file
		);
	}

	// get all biases and save them to .nnd

	for (int i = 0; i < network.layerCount; i++) {
		fwrite(
			network.biases[i].contents,
			sizeof(network.biases[i].contents[0]),
			network.biases[i].rows * network.biases[i].cols,
			file
		);
	}

	fclose(file);

	return 0;
}

inline Neurals loadNeurals(char* networkName) {

	char location[64];

	sprintf(location, "networks/%s.nnd", networkName);

	FILE* file = fopen(location, "rb");
	size_t bitsWritten = 0;

	// get layerCount from .nnd
	int layerCount = 0;
	fread(&layerCount, sizeof(layerCount), 1, file);

	// get architecture from .nnd
	int* architecture = malloc((layerCount + 1) * sizeof(int));
	fread(architecture, sizeof(architecture[0]), layerCount + 1, file);


	Neurals network = allocateNeurals(architecture, layerCount + 1, 0, 0);

	for (int i = 0; i < network.layerCount; i++) {
		fread(
			network.weights[i].contents, 
			sizeof(network.weights[i].contents[0]),
			network.weights[i].cols * network.weights[i].rows,
			file
		);
	}

	for (int i = 0; i < network.layerCount; i++) {
		fread(
			network.biases[i].contents,
			sizeof(network.biases[i].contents[0]),
			network.biases[i].cols * network.biases[i].rows,
			file
		);
	}

	fclose(file);
	free(architecture);

	return network;
}





#endif