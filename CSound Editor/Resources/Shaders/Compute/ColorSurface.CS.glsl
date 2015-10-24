#version 430

layout(std430, binding = 0) buffer SSBO {
	uint counter[];
};

layout(binding = 0, rgba32f) uniform readonly image2D colorMap;
layout(local_size_x = 32, local_size_y = 32) in;

// TODO - use this for correct color offset during debuggin
uniform vec3 colorID_offset;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = imageLoad(colorMap, pixel);

	uint index = uint(color.z * 5) * 256 + uint(color.y * 15) * 16 + uint(color.x * 15);
	atomicAdd(counter[index], 1);
}
