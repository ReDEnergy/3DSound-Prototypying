#version 410
#define MAX_BONES 100

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in ivec4 v_boneIds;
layout(location = 4) in vec4 v_weights;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Bones[MAX_BONES];

layout(location = 0) out vec2 texture_coord;

void main()
{
	mat4 BoneTransform;
    BoneTransform  = Bones[v_boneIds[0]] * v_weights[0];
    BoneTransform += Bones[v_boneIds[1]] * v_weights[1];
    BoneTransform += Bones[v_boneIds[2]] * v_weights[2];
    BoneTransform += Bones[v_boneIds[3]] * v_weights[3];
	
	texture_coord = v_texture_coord;
	gl_Position = Projection * View * Model * BoneTransform * vec4(v_position, 1.0);
}
