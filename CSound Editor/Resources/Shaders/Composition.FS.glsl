#version 410

#define Diffuse	u_texture_0

uniform sampler2D u_texture_0;	// Diffuse texture
uniform sampler2D u_texture_1;	// Deferred light
uniform sampler2D u_texture_2;	// Shadows
uniform sampler2D u_texture_3;	// Ambient occlusion
uniform sampler2D u_texture_4;	// Depth Scene
uniform sampler2D u_texture_5;	// Debug View
uniform sampler2D u_texture_6;	// Debug Debug
uniform sampler2D u_texture_7;	// Gizmo
uniform sampler2D u_texture_8;	// 
uniform sampler2D u_texture_9;	// 
uniform sampler2D u_texture_10;	// Skeletal System View
uniform sampler2D u_texture_11;	// Skeletal System View

uniform vec2 resolution;
uniform int active_ssao;
uniform int active_deferred;
uniform bool active_selection;
uniform int debug_view;
uniform float zFar;
uniform float zNear;

layout(location = 0) out vec4 out_color;

///////////////////////////////////////////////////////////////////////////////
// Utils

float linearDepth(sampler2D depthTexture, vec2 coord);
vec4 viewDepth(sampler2D depthTexture, vec2 coord);
vec4 component(sampler2D textureID, vec2 coord, int channel);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO - make Alpha blending if alpha channel different than 1

vec4 DebugView(vec2 text_coord) {
	float ds = texture(u_texture_4, text_coord).r;
	float dd = texture(u_texture_6, text_coord).r;
	vec4 color = texture(u_texture_5, text_coord);
	if (dd < ds || (dd == 1.0 && color != vec4(0)))
		return texture(u_texture_5, text_coord);
	return vec4(0);
}

vec4 KinectView(vec2 text_coord) {
	float ds = texture(u_texture_4, text_coord).r;
	float dd = texture(u_texture_8, text_coord).r;
	if (dd < ds)
		return texture(u_texture_9, text_coord);
	return vec4(0);
}

vec4 SkeletalView(vec2 text_coord) {
	float ds = texture(u_texture_4, text_coord).r;
	float dd = texture(u_texture_10, text_coord).r;
	if (dd < ds)
		return texture(u_texture_11, text_coord);
	return vec4(0);
}

float SoftShadow(vec2 text_coord) {
	float depth = linearDepth(u_texture_4, text_coord);
	if (depth > 40)
		return texture(u_texture_0, text_coord).a;

	float sum = 0;
	float kernel = 5;
	if (depth > 16)	kernel = 3;
	
	int size = int(kernel / 2);
	for (int i=-size; i<=size; i++) {
		for (int j=-size; j<=size; j++) {
			float shadow = texture(u_texture_0, text_coord + ivec2(i, j) / resolution).a;
			sum += shadow;
		}
	}
	
	return sum/(kernel * kernel);
}

void main() {
	vec2 text_coord = gl_FragCoord.xy / resolution;

	vec4 diffuse = texture(u_texture_0, text_coord);
	// diffuse.rgb *= SoftShadow(text_coord);

	///////////////////////////////////////////////////////////////////////////
	// Bake Shadows
	// vec4 shadow = texture(u_texture_2, text_coord);
	// diffuse.rgb *= shadow.r;
	// diffuse.rgb *= shadow.g;
	// diffuse.rgb *= shadow.b;
	// diffuse.rgb *= shadow.a;
	
	///////////////////////////////////////////////////////////////////////////
	// Bake Ambient Occlusion
	if (active_ssao == 1) {
		float ao = texture(u_texture_3, text_coord).x;
		vec4 ssao = vec4(1.0) - vec4(ao, ao, ao, 1.0);
		out_color = clamp((diffuse - ssao), 0.0, 1.0);
	} else {
		out_color = diffuse;
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Bake Light
	vec4 light = texture(u_texture_1, text_coord);
	vec4 ambient = vec4(0.8);
	out_color *= (light + ambient);
	
	///////////////////////////////////////////////////////////////////////////
	// Creates a fake alpha blending
	if (debug_view == 1)
		out_color += DebugView(text_coord);
		
	out_color += SkeletalView(text_coord);

	if (active_selection)
	{
		vec4 gizmo_add = texture(u_texture_7, text_coord);
		if (gizmo_add != vec4(0, 0, 0, 1))
			out_color = gizmo_add + out_color * 0.5;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Utils

float linearDepth(sampler2D depthTexture, vec2 coord) {
	float zt = texture(depthTexture, coord).x;
	float zn = 2.0 * zt - 1.0;
	float z = (2.0 * zNear * zFar) / (zFar + zNear - zn * (zFar - zNear));
	return z;
}

vec4 viewDepth(sampler2D depthTexture, vec2 coord) {
	float dist = texture(depthTexture, coord).x;
	float d = pow(dist , 128);
	return vec4(d, d, d, 1.0);
}

vec4 component(sampler2D textureID, vec2 coord, int channel) {
	float color = texture(textureID, coord)[channel];
	return vec4(vec3(color), 1.0);
}