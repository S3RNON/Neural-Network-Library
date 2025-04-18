# Neural Network Library

This is a library to create and train Neural Networks. Right now, it only supports traditional Neural Networks, with Input layers, Hidden layers and Output layers. In the near future, convolutional layers are planned. The libraries themselves are found in the header files, and additional headers like pngn.h and timefuncs.h are created to serve my needs, but are not restrictly necessary to create your own neural network. However, I've found these libraries, especially pngn.h to be too useful to not also commit to github. Pngn.h is a library that reads PNG files and decrypts them.

This was all built in C, mostly to be a technically challenging exercise for myself but also to take advantage of the high speed that C is known for and the data manipulation. In the future, I will be porting this project in C++ to take advantage of the Object Oriented Programming.

# Notable features:
1. Easy to use header files which are modular. Say you want to build your own Matrix functionality, you can do that by defining ```_NEURAL_MATRIX``` yourself with `#define _NEURAL_MATRIX` at the top of your main file. Then, you would need to look at Neurals.h to see what functionality your custom Matrices need to have in order to function.
2. Custom architecture of Input, Hidden and Output layers. Neural Networks can vary vastly in use, and custom architecture is an absolute must. 
3. Implementation of Strassen's algorithm for very fast Matrix multiplications, which is necessary when dealing with large data being trained on.