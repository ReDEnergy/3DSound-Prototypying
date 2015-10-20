#version 430
layout(local_size_x = 32, local_size_y = 32) in;

// layout(binding = 0, rgba32f) uniform image2D colorBuffer;
layout(binding = 0, r8) uniform image2D colorBuffer;
layout(binding = 1, rgba32f) uniform image2D worldPosition;

layout(std430, binding = 0) buffer SSBO {
	vec3 distance[];
};

uniform vec3 plane_direction;
uniform vec3 plane_origin;
uniform ivec2 resolution;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	//vec4 pos = imageLoad(worldPosition, pixel);
	// imageStore(colorBuffer, pixel, pos);

	// float dist = abs(dot(position - plane_origin, plane_direction));
	// distance[pixel.y * resolution.x + pixel.x] = pos.xyz;

	// float outColor = 1.0;
	// if (dist < 1000)
		// outColor = 0.1;
	
	imageStore(colorBuffer, pixel, vec4(pixel.x / float(resolution.x), pixel.y / float(resolution.y), 0, 1.0));
}
