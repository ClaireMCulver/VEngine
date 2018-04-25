#pragma once
#include <stdio.h>
#include <vector>

class Layer;
enum LayerType;

enum ActivationFunction
{
	SIGMOID,
	TANH,
	RELU
};

class Node
{
public:
	friend Layer;
	//constructor called only by layer's constructor
	Node(int weightCounts, Layer* parentLayer, ActivationFunction activationFunction, int id, int* extraData);

	//evaluates the node using the given inputs and returns the result of the activation function on the weighted total plus bias
	float Evaluate(float* inputs, int numInputs);

	//backpropagates this node to the target value, target value only needed for final layer
	float Backpropagate(float target = 0);

	//log the other half of the nodes line
	void LogState(int runId, bool toFile, bool toConsole, FILE* file);

private:
	//calculates the result of the activation function on x
	float Activation(float x);

	//calculate the result of the derivative activation function on x
	float DerivativeActivation(float x);

	//this nodes id, only unique inside layer, aka neuronId
	int id;

	float bias = 0.f;

	//dynamic weight array
	int numWeights;
	float* weights;

	//weighted sum from last evaluation
	float lastSum;

	//result of the activation from last evaluation
	float lastEval = -123.f;

	//backpropagation error of this node
	float error;

	Layer* parent;
	LayerType layerType;
	ActivationFunction actFunct;
};