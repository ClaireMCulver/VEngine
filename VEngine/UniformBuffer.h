#pragma once
#include "GPUBuffer.h"
#include "VEngineDefs.h"

class UniformBuffer :
	public GPUBuffer
{
public:
	UniformBuffer();
	~UniformBuffer();

public:
	bool SetBufferData(void* data, size_t dataSize, int offset);

private:
	void* ramBuffer;
};

