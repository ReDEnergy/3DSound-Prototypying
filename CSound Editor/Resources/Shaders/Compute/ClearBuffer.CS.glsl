#version 430

layout(std430, binding = 0) buffer SSBO {
	uint counter[];
};

layout(local_size_x = 16) in;

void main()
{
	counter[gl_GlobalInvocationID.x] = 0;
}
