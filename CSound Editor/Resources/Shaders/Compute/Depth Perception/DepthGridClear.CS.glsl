#version 430
layout(local_size_x = 32) in;

layout(std430, binding = 0) writeonly buffer ssbo {
	uvec2 cell[];
};

layout(std430, binding = 1) writeonly buffer ssbo2 {
	ivec4 position[];
};

void main()
{
	cell[gl_GlobalInvocationID.x].x = 0xFFFF;
	cell[gl_GlobalInvocationID.x].y = 0;
	position[gl_GlobalInvocationID.x] = ivec4(0);
}
