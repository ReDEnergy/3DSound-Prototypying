#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;

layout(location = 0) out vec2 text_coord;

vec3 wup = vec3(0, 1.0, 0);
vec3 forward = normalize(eye_position - gl_in[0].gl_Position.xyz);
vec3 right = normalize(cross(forward, wup));
vec3 up = normalize(cross(forward, right));

vec4 GetPosition(vec3 offset) {
	vec3 pos = right * offset.x + up * offset.y + gl_in[0].gl_Position.xyz;
	return Projection * View * vec4(pos, 1.0);
}

void EmitPoint(vec3 offset) {
	gl_Position = GetPosition(offset);
	EmitVertex();
}

void main() {
	
	float ds = 0.25 ;
	
	// Point 0
	text_coord = vec2(0, 0);
	EmitPoint(vec3(-ds, -ds, 0));
	
	// Point 1
	text_coord = vec2(1, 0);
	EmitPoint(vec3( ds, -ds, 0));
	
	// Point 3
	text_coord = vec2(0, 1);
	EmitPoint(vec3(-ds,  ds, 0));

	// Point 2
	text_coord = vec2(1, 1);
	EmitPoint(vec3( ds,  ds, 0));
}
