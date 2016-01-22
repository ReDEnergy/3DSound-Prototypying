#version 430
layout(local_size_x = 32, local_size_y = 32) in;

layout(std430, binding = 0) buffer SSBO {
	uint counter[];
};

layout(binding = 0, rgba32f) uniform readonly image2D colorMap;

// TODO - use this for correct color offset during debuggin
uniform ivec3 colorID_step;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = imageLoad(colorMap, pixel);

	uint index =  uint(color.x * colorID_step.x) + uint(color.y * colorID_step.y) * 16 + uint(color.z * colorID_step.z) * 256;
	atomicAdd(counter[index], 1);
}
