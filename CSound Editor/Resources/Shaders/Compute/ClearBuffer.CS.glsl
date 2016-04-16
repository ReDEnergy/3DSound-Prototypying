#version 430
layout(local_size_x = 32) in;

layout(std430, binding = 0) buffer SSBO {
	uint counter[];
};

void main()
{
	counter[gl_GlobalInvocationID.x] = 0;
}
