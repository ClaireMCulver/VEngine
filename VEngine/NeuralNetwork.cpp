#include "NeuralNetwork.h"

#include <memory> //memcpy

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

NeuralNetwork::NeuralNetwork(int layerCount, LayerType* layerTypes, int* nodeCounts, ActivationFunction* layerActivationFunctions, int numInputs, int* extraData)
{
	numLayers = layerCount;
	//allocate memory, avoid use of Layer constructor
	layers = (Layer**) new char[numLayers * sizeof(Layer*)];

	for (int i = 0; i < numLayers; i++)
	{
		//generate a layer and store its pointer
		layers[i] = new Layer(nodeCounts[i], i == 0 ? numInputs : layers[i - 1]->GetNumOutputs(), layerTypes[i], layerActivationFunctions[i], i, extraData);
		layers[i]->nn = this;
	}
	
	for (int i = 0; i < numLayers; i++)
	{
		//link layers
		layers[i]->prev = i - 1 < 0 ? NULL : layers[i - 1];
		layers[i]->next = i + 1 >= numLayers ? NULL : layers[i + 1];
	}

	//allocate lastinput space
	lastInput = new float[numInputs];
}

int NeuralNetwork::FinalLayerOutputCount()
{
	return layers[numLayers - 1]->GetNumOutputs();
}

void NeuralNetwork::Evaluate(float* inputs, int numInputs, float* outputValues)
{
	//store lastinput
	memcpy_s(lastInput, sizeof(float) * numInputs, inputs, sizeof(float) * numInputs);

	//prep our memory
	_numOutputs[0] = layers[0]->GetNumOutputs();
	_results[0] = new float[_numOutputs[0]];
	_results[1] = new float[1];

	//first evaluation goes into index 0
	layers[0]->Evaluate(inputs, numInputs, _results[0], &_numOutputs[0]);
	

	int index = 0;
	int lastindex = 0;
	for (int i = 1; i < numLayers; i++)
	{
		//calculate which index we are going to use
		index = i % 2;
		//delete the one that was not done last, the one were about to use
		delete _results[index];
		//reallocate the memory to the size we need
		_numOutputs[index] = layers[i]->GetNumOutputs();
		_results[index] = new float[_numOutputs[index]];
		//run the layer and store into our index
		layers[i]->Evaluate(_results[lastindex], _numOutputs[lastindex], _results[index], &_numOutputs[index]);
		//store last index
		lastindex = index;
	}

	//send return values via memcpy into the output pointer
	memcpy_s(outputValues, _numOutputs[lastindex] * sizeof(float), _results[lastindex], _numOutputs[lastindex] * sizeof(float));

	//cleanup
	delete _results[0];
	delete _results[1];
}

void NeuralNetwork::BackProp(float* actualProp)
{
	//call backprop on layers in last to first order
	layers[numLayers - 1]->Backpropagate(actualProp);
	for (int i = numLayers - 2; i >= 0; i--)
	{
		layers[i]->Backpropagate();
	}
}

void NeuralNetwork::ReLinkPointers()
{
	//incase our layers or nodes lose the pointers
	for (int i = 0; i < numLayers; i++)
	{
		layers[i]->nn = this;
		layers[i]->prev = i - 1 < 0 ? NULL : layers[i - 1];
		layers[i]->next = i + 1 >= numLayers ? NULL : layers[i + 1];
		//re-link the nodes parent pointer
		layers[i]->ReLink();
	}
}

bool NeuralNetwork::InitLogging(const char* stateFilePath, bool clearContents)
{
	//open the log file, and print the csv header if clear mode is on
	fopen_s(&stateLog, stateFilePath, clearContents ? "w" : "a");
	if (stateLog == NULL)
	{
		printf("Unable to open state file \"%s\"", stateFilePath);
		return false;
	}
	if (clearContents) fprintf(stateLog, "logID,layerID,layerType,neuronID,activationFunction,lastValue,weights,bias\n");
	return true;
}

void NeuralNetwork::CloseLogging()
{
	fclose(stateLog);
}

void NeuralNetwork::LogState(int runId, bool toFile, bool toConsole)
{
	for (int i = 0; i < numLayers; i++)
		layers[i]->LogState(runId, toFile, toConsole, stateLog);
}

bool NeuralNetwork::LoadStateLog(char* stateFilePath)
{
	std::ifstream csv(stateFilePath);
	std::string line;
	
	//get and ignore the csv header
	std::getline(csv, line, '\n');

	//make sure we arnt at eof
	while (csv.good())
	{
		std::stringstream ss(line);
		
		//logId - ignore
		std::getline(csv, line, ',');

		//layerId - store
		std::getline(csv, line, ',');
		int layerId = 0;
		ss = std::stringstream(line);
		ss >> layerId;

		//layerType - ignore
		std::getline(csv, line, ',');

		//neuronId - store
		std::getline(csv, line, ',');
		int neuronId = 0;
		ss = std::stringstream(line);
		ss >> neuronId;

		//activation - ignore
		std::getline(csv, line, ',');

		//lastValue - ignore
		std::getline(csv, line, ',');
		
		//weight array - store
		//this line is stored in the csv as "[w1,w2,w3,...wx]"
		//skip the ""[" part
		std::getline(csv, line, '[');
		//fetch till the ending ]
		std::getline(csv, line, ']');
		//line is currently a list of csv float values for weights
		//example: 0.1,0.2,0.3,0.4
		ss = std::stringstream(line);
		std::vector<float> weights;
		float val;
		//attempt to convert
		while (ss >> val)
		{
			//if successful add it
			weights.push_back(val);
			//peak to skip the comma
			if (ss.peek() == ',')
				ss.ignore();
		}
		//fetch and ignore the trailing comma to reset file position to our next value
		std::getline(csv, line, ',');

		//bias - store
		std::getline(csv, line, '\n');
		float bias = 0;
		ss = std::stringstream(line);
		ss >> bias;

		//apply node
		layers[layerId]->SetNode(neuronId, weights, bias);
	}

	return true;
}