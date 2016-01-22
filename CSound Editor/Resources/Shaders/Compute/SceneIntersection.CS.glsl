#version 430
layout(local_size_x = 32, local_size_y = 32) in;

layout(binding = 0, r8) uniform image2D colorBuffer;
layout(binding = 1, rgba32f) uniform readonly image2D viewPosition;
layout(binding = 2, rgba32f) uniform readonly image2D colorMap;
layout(binding = 3, rgba32f) uniform image2D diffuseMap;

layout(std430, binding = 0) buffer ssbo {
	ivec4 center[];
};

uniform vec3 plane_direction;
uniform float sphere_size;
uniform vec3 eye_position;
uniform ivec3 colorID_step;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec3 pos = imageLoad(viewPosition, pixel).xyz;

	float outColor = 0;

	// Plane Intersection
	// float dist = abs(dot(vec3(pos.xy, -pos.z), plane_direction));
	// if (abs(dist - sphere_size) < 0.01 )
		// outColor = 1.0;

	// Spehere intersection
	// TODO ?! Why not use depth ? Because it's not linear.... View Space is linear
	// float dist = length(pos);
	float dist = length(pos);
	if (abs(dist - sphere_size) < 0.05 )
		outColor = 1.0;

	// Save visual information
	imageStore(colorBuffer, pixel, vec4(outColor, 0, 0, 1.0));

	// Bake into the scene diffuse map
	vec4 diffuse = imageLoad(diffuseMap, pixel);
	imageStore(diffuseMap, pixel, diffuse + vec4(outColor, 0, 0, 0));

	// Save position information
	if (outColor != 0)
	{
		vec4 color = imageLoad(colorMap, pixel);
		uint index =  uint(color.x * colorID_step.x) + uint(color.y * colorID_step.y) * 16 + uint(color.z * colorID_step.z) * 256;
		atomicAdd(center[index].x, int(pos.x * 1000));
		atomicAdd(center[index].y, int(pos.y * 1000));
		atomicAdd(center[index].z, int(pos.z * 1000));
		atomicAdd(center[index].w, 1);
	}
}
