#pragma once
#include <stdio.h>
#include "Node.h"

enum LayerType
{
	CONV,
	FCL,
	POOL
};

class NeuralNetwork;

class Layer
{
public:
	friend Node;

	//constructor, never actually called manually, instead called by NeuralNetwork's constructor
	Layer(int nodeCount, int numInputs,LayerType type, ActivationFunction activationFunction, int id, int* extraData);

	//output values and numoutputs must be valid allocated memory, it will be memcpy'd to
	void Evaluate(float* inputs, int numInputs, float* outputValues, int* numOutputs);

	//performs backpropagation on this layer, *target contains the target values for the final layer, or nullptr
	void Backpropagate(float* target = nullptr);

	int GetNumOutputs();
	LayerType GetType();

	//log sate to file
	void LogState(int runId, bool toFile, bool toConsole, FILE* file);

	//sets each node's parent pointer to this
	void ReLink();

	//sets the weights and floats for node id
	void SetNode(int id, std::vector<float> &weights, float bias);

	//doublly linked list
	Layer* prev;
	Layer* next;
	//parent network
	NeuralNetwork* nn;

private:
	//id of this layer, aka layerId
	int id;

	//dynamic allocation node array
	int numNodes;
	Node* nodes;

	LayerType layerType;
};