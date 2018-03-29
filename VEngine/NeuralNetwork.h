#pragma once
#include <stdio.h>
#include "Layer.h"

class NeuralNetwork
{
public:
	friend Node;

	//extraData must be nullptr
	//all other pointers must be arrays of length layerCount, with each element describing that index's layer
	NeuralNetwork(int layerCount, LayerType* layerTypes, int* nodeCounts, ActivationFunction* layerActivationFunctions, int numInputs, int* extraData = nullptr);

	//note outputValues and numOutputs must be a valid empty array, it will be memcpy'd to.
	void Evaluate(float* inputs, int numInputs, float* outputValues);

	//must be called after evaluate, and re-evaluated before backproping again
	void BackProp(float* actualResult);

	int FinalLayerOutputCount();

	//returns true if file opened succesfully, prints to console error info
	bool InitLogging(const char* stateFilePath, bool clearContents = true);

	//must be called before exit or data is not gaurenteed to be written to file
	void CloseLogging();

	//Initlogging must have been called if toFile == true
	void LogState(int runId, bool toFile, bool toConsole);

	//network constructor must have already been called, and in the same format as the logged network
	bool LoadStateLog(char* stateFilePath);

	//call after the constructor incase the allocations caused node pointers to break
	void ReLinkPointers();

private:
	//c file pointer for csv output
	FILE* stateLog;

	//dynamic array of layer pointers
	int numLayers;
	Layer** layers;
	
	//stored copy of the last input sent to the network, mainly for backpropogation final layer
	float* lastInput;

	//for evaluation calculations and returns
	int _numOutputs[2];
	float* _results[2];

};