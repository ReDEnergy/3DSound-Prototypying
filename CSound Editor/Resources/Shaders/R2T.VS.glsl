#version 410

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;
layout(location = 2) in vec3 v_normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec4 world_position;
layout(location = 2) out vec4 world_normal;
layout(location = 3) out vec4 view_position;
layout(location = 4) out vec4 view_normal;
layout(location = 5) out vec4 screen_position;

void main() {
	texture_coord = v_texture_coord;

	world_position = Model * vec4(v_position, 1.0);
	world_normal = Model * vec4(v_normal, 1.0);

	view_position = View * world_position;
	view_normal = View * world_normal;
	
	gl_Position = Projection * view_position;
	screen_position = gl_Position;
}
