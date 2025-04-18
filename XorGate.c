#include "Main.h"

#ifndef XORGATEC_DEF
#define XORGATEC_DEF

inline int XOR_main() {

	srand(getTimeInMilliseconds());

	// create the network
	int architecture[] = {
		2, 3, 3, 1
	};
	Neurals network = allocateNeurals(architecture, arrayCount(architecture), 0, 0, 0);


	randomizeNeurals(network, 0.f, 1.f);

	// create the data to learn from
	float xorTestData[] = {
		0, 0, 0,
		0, 1, 1,
		1, 0, 1,
		1, 1, 0,
	};
	Matrix xorTestMatrix = createMatrix(arrayCount(xorTestData) / 3, 3, xorTestData, sizeof(xorTestData));
	Matrix xorTrainingInput = subMatrix(xorTestMatrix, 0, 0, xorTestMatrix.rows, 2);
	Matrix xorTrainingOutput = subMatrix(xorTestMatrix, 2, 0, xorTestMatrix.rows, 1);

	// train the network
	long long int timeBefore = getTimeInMilliseconds();
	trainNeurals(network, xorTrainingInput, xorTrainingOutput, 1e-3, 10, 1e-3, 100, 1000);
	printf("Total time: %llu ms\n\nResults:\n", getTimeInMilliseconds() - timeBefore);

	//printNeurals(network);

	//// save the network to disk
	//saveNeurals(network, "xorGate");

	////Neurals network = loadNeurals("xorGate");

	// print the result
	for (int i = 0; i < xorTrainingInput.rows; i++) {

		Matrix inputs = rowMatrix(xorTrainingInput, i);
		Matrix outputs = inputNeurals(network, inputs);

		printf("%.1f ^ %.1f = %f\n", inputs.contents[0], inputs.contents[1], outputs.contents[0]);
	}

	//freeMatrix(xorTrainingInput);
	//freeMatrix(xorTrainingOutput);
	//freeNeurals(network);
	return 0;
};

#endif