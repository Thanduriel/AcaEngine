#version 450

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_world_position;

layout(binding = 0) uniform sampler2D tx_diffuse;

#define MAX_LIGHTS 8
const float e = 0.001;
const int SPECULAR = 15;

layout(location = 3) uniform vec3 camera_pos;
layout(location = 4) uniform vec3 light_color[MAX_LIGHTS];
layout(location = 4+MAX_LIGHTS) uniform vec3 light_position[MAX_LIGHTS];
layout(location = 4+MAX_LIGHTS * 2) uniform float light_intensity[MAX_LIGHTS];
layout(location = 4+MAX_LIGHTS * 3) uniform int n_ligths;

layout(location = 0) out vec4 out_color;

vec3 brdf(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 lColor, float d, float intensity)
{
	vec3 reflectDir = reflect(-lightDir, normal);
	float diffF = dot(lightDir, normal);
	if (diffF < 0) { return vec3(0,0,0);}
	float specF = max(dot(reflectDir, viewDir), 0.f);
	float i = 80.f / (1 + d*d);
	return i * (diffF * lColor * 0.5 + 0.5 * pow(specF, SPECULAR)*vec3(1,1,1));
}

void main()
{
	// vec3 tColor = texture(tx_diffuse, in_texCoord).rgb;
	vec3 tColor = vec3(1,1,1);
	vec3 lColor = vec3(0);
	vec3 viewDir = normalize(camera_pos - in_world_position);
	for ( int i = 0; i < MAX_LIGHTS; ++i) {
		if (i < n_ligths) {
			vec3 lightPos = light_position[i] - vec3(0,0,20);
			float d = length(in_world_position - lightPos);
			vec3 lightDir = normalize(lightPos - in_world_position);
			lColor += brdf(lightDir, viewDir, in_normal.xyz, light_color[i], d, light_intensity[i]);
		}
	}
	out_color = vec4(lColor*tColor * viewDir.z, 1);
}
