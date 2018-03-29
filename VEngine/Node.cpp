#include <math.h>
#include <random>
#include "NeuralNetwork.h"

#define LEARNING_RATE 0.005f

Node::Node(int weightCounts, Layer* parentLayer, ActivationFunction activationFunction, int idnum, int* extraData)
{
	id = idnum;
	parent = parentLayer;
	layerType = parentLayer->GetType();
	actFunct = activationFunction;

	//allocate weights array
	numWeights = weightCounts;
	weights = new float[weightCounts];
	
	for (int i = 0; i < numWeights; i++)
	{
		//set weight to a random value between -1 and 1
		weights[i] = ((float)rand() / RAND_MAX) * 2.f - 1.f;
	}

	//set bias value between -1 and 1
	bias = ((float)rand() / RAND_MAX) * 2.f - 1.f;
}


float Node::Evaluate(float* inputs, int numInputs)
{
	//calculate sum of weights*inputs
	float sum = 0;
	for (int i = 0; i < numInputs; i++)
		sum += inputs[i] * weights[i];
	lastSum = sum + bias;
	//run the sum through the activation function
	return lastEval = Activation(lastSum);
}


float Node::Activation(float x)
{
	switch (actFunct)
	{
	case SIGMOID:
		return x / (1.f + abs(x));
	case TANH:
		return tanh(x);
	case RELU:
		return (float)fmax(0,x);
	default:
		return 0.f;
	}
}

float Node::DerivativeActivation(float x)
{
	switch (actFunct)
	{
	case SIGMOID: {
		float fx = x / (1.f + abs(x));
		return fx * (1 - fx); }
	case TANH:
		return 1 - pow(tanh(x), 2);
	case RELU:
		return x >= 0.f ? 1.f : 0.f;
		return 0.f;
	default:
		return 0.f;
	}
}

float Node::Backpropagate(float target)
{
	//fix the lock-out issue explained in slides
	if (lastSum > 2.f)
		lastSum = 2.f;
	else if (lastSum < -2.f)
		lastSum = -2.f;

	//check if we are last layer
	if (parent->next == NULL)
	{
		//caluclate error, according to delta rule this is (t - j)
		float targetwrtout = -(target - lastEval);
		//calculate the derivative of the activator, according to delta rule this is g'(h)
		float outwrtnet = DerivativeActivation(lastSum);
		//calculate the node delta, just so we dont do it per neuron, this value is also stored as error
		float nodeDelta = targetwrtout * outwrtnet;

		for (int i = 0; i < numWeights; i++)
		{
			//calculate the weights error, with the delta rule this is everything but the learning consant
			float errorwrtweight = nodeDelta * parent->prev->nodes[i].lastEval;
			//modify weights by the rest of the delta rule, aka the learning rate * weights error
			weights[i] = weights[i] - LEARNING_RATE * errorwrtweight;
		}

		//same thing for bias as with weights, but ignore the weights error
		bias = bias - LEARNING_RATE * nodeDelta;

		//store the error for previous layers
		error = nodeDelta;

		return error;
	}
	else
	{
		//error calculation, the summation of the prior layers per neuron error multiplied by the inlfuence that neuron has on this neuron
		float targetwrtout = 0;
		for (int i = 0; i < parent->next->numNodes; i++)
			targetwrtout += parent->next->nodes[i].error * parent->next->nodes[i].weights[id];

		//the rest is the same as the input layer
		float outwrtnet = DerivativeActivation(lastSum);
		float nodeDelta = targetwrtout * outwrtnet;

		for (int i = 0; i < numWeights; i++)
		{
			//except here, here if our neruons input was the actual input, aka layer 0, we use the actual input
			float errorweight = nodeDelta * (parent->prev != NULL ? parent->prev->nodes[i].lastEval : parent->nn->lastInput[i]);
			weights[i] = weights[i] - LEARNING_RATE * errorweight;
		}


		//do it for bias
		bias = bias - LEARNING_RATE * nodeDelta;

		error = nodeDelta;
		return error;
	}
}

char* ActivationFunctionString(ActivationFunction a)
{
	switch (a)
	{
	case SIGMOID: return "SoftStep";
	case TANH: return "tanh";
	case RELU:	return "ReLu";
	default: return "ERR";
	}
}

void Node::LogState(int runId, bool toFile, bool toConsole, FILE* file)
{
	//CSV Format is
	//RunID, LayerID, LayerType, NueronID, ActivationFunction, LastValue, [Weights], Bias
	//At this point in time, The current line in the file should be "RunID, LayerID, LayerType,"
	if (toFile)
	{
		fprintf(file, "%i,%s,%g,\"[", id, ActivationFunctionString(actFunct),lastEval);
		for (int i = 0; i < numWeights - 1; i++)
			fprintf(file, "%g,", weights[i]);
		fprintf(file, "%g]\",%g\n", weights[numWeights - 1], bias);
	}
	if (toConsole)
	{
		printf("%i,%s,%g,\"[", id, ActivationFunctionString(actFunct),lastEval);
		for (int i = 0; i < numWeights - 1; i++)
			printf("%g,", weights[i]);
		printf("%g]\",%g\n", weights[numWeights - 1], bias);
	}
}