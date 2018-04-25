#include "Layer.h"

Layer::Layer(int nodeCount, int numInputs, LayerType type, ActivationFunction activationFunction, int idnum, int* extraData)
{
	id = idnum;
	numNodes = nodeCount;
	layerType = type;
	
	//allocate node array as we did with layer, ignoring constructor
	nodes = (Node*) new char[nodeCount * sizeof(Node)];

	for (int i = 0; i < numNodes; i++)
	{
		//set each node to a new node via constructor
		nodes[i] = Node(numInputs, this, activationFunction, i, extraData);
	}
}

int Layer::GetNumOutputs()
{
	return numNodes;
}

LayerType Layer::GetType()
{
	return layerType;
}

void Layer::Evaluate(float* inputs, int numInputs, float* outputValues, int* numOutputs)
{
	//simply set the output value at the nodeId's index to the nodes result
	for (int i = 0; i < numNodes; i++)
	{
		outputValues[i] = nodes[i].Evaluate(inputs, numInputs);
	}
	//the numoutput return pointers value
	*numOutputs = numNodes;
}

void Layer::Backpropagate(float* target)
{
	//simply call backpropagate on each node
	for (int i = 0; i < numNodes; i++)
	{
		nodes[i].Backpropagate(target == nullptr ? 0 : target[i]);
	}
}

void Layer::ReLink()
{
	for (int i = 0; i < numNodes; i++)
		nodes[i].parent = this;
}

void Layer::SetNode(int id, std::vector<float> &weights, float bias)
{
	//set each nodes values
	nodes[id].bias = bias;
	for (int i = 0, s = weights.size(); i < s; i++)
	{
		nodes[id].weights[i] = weights[i];
	}
}

char* LayerTypeString(LayerType t)
{
	switch(t)
	{
	case CONV: return "CONV";
	case FCL: return "FCL";
	default: return "ERR";
	}
}

void Layer::LogState(int runId, bool toFile, bool toConsole, FILE* file)
{
	//CSV Format is
	//RunID, LayerID, LayerType, NueronID, ActivationFunction, LastValue, [Weights], Bias
	//At this point in time, the function must set the line to "RunID,LayerID,LayerType," then call the nodes log for each node

	for (int i = 0; i < numNodes; i++)
	{
		if (toFile) fprintf(file, "%i,%i,%s,", runId, id, LayerTypeString(layerType));
		if (toConsole) printf("%i,%i,%s,", runId, id, LayerTypeString(layerType));
		nodes[i].LogState(runId, toFile, toConsole, file);
	}
}