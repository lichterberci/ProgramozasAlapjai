# Technical documentation

## Reading the datasets

A dataset is read from 2 files: one contains the pixel-data, the other one contains the labels. They are read simultaneously and stored in RAM in a Dataset struct.

### Format of the image files

- 4B magic number
- 4B # images
- 4B # cols
- 4B # rows
- images --> 1B per pixel

### Format of the label files

- 4B magic number
- 4B # labels
- labels --> 1B per label

## Model breakdown

We use a fully-connected feed-forward deep neural network. A general case can be found in the image below.

![Image ofa neural network](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQ6BF6WZQ1Dc27ofamPVwhjFRMGuvdkyZXmJQ&usqp=CAU)

### Input layer

A flattened vector of the input pixels normalized to [0:1] interval.

### Hidden layers

These are defined by the user. Their size and activation function can be customized.

### Output layer

As this problem is a classification problem, the output layer's activation function is softmax. The output layer has 10 neurons, as we have 10 classes.

## Implementing the feed-forward algorithm 

For every layer, we compute __f(W*x+b)__, where f is the activation function, W is the weight matrix, x is the previous layer's value-vector and b is the bias vector.

![Image of the feed-forward algorithm](https://miro.medium.com/max/1400/1*upfpVueoUuKPkyX3PR3KBg.png)

## Implementing backpropagation

We use cross-entropy for the loss-function, as it is great for classification problems.

Articles for better understanding:

- [Backpropagation](https://towardsdatascience.com/understanding-backpropagation-algorithm-7bb3aa2f95fd)
- [Softmax and cross-entropy loss](https://towardsdatascience.com/derivative-of-the-softmax-function-and-the-categorical-cross-entropy-loss-ffceefc081d1)

We first compute the loss, then we compute the derivative of the loss with respect to the neurons in each layer. We do this first, so we can use the chain-rule later with ease.

Then we adjust the weights and biases using the chain rule. This is easy, as we just use the previously computed derivatives.

## Overview of the program's structure

The program reads the arguments, determines the actions, it would have to take (training, testing of accuracy, showing images), and does them sequentially. The other parameters just customize this basic flow (eg.: by setting the dataset's folder).
## Saving / loading models

Each model can be saved/loaded to/from a file. Usually we use the .model extension, but it can differ.

### The file-structure of the .model extension

- 1B # layers
- for each layer:
  - 4B inputDim
  - 4B outputDim
  - inputDim \* outputDim \* 8B weights
  - outputDIm \* 8B biases

## Important types

### Model *(struct)*

Defines a model, holds its layers in a heap-allocated array.

### Layer *(struct)*

Defines a layer, holds its dimensions, and its weights and biases in heap-allocated arrays.

### Dataset *(struct)*

Defines a dataset, conatins its images and labels in a heap-allocated LabeledImage array.

### LabeledImage *(struct)*

Defines an image with its label. Contains its pixels in a stack-allocated array. Should not be copied much!

### Result *(struct)*

Defines a prediction-result. Holds 10 stack-allocated probabilities.

