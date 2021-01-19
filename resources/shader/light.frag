#version 450

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_world_position;

layout(binding = 0) uniform sampler2D tx_diffuse;

#define MAX_LIGTHS 8
#define e 0.001f
#define SPECULAR 8
layout(location = 3) uniform vec3 light_color[MAX_LIGTHS];
layout(location = 3+MAX_LIGTHS) uniform vec3 light_position[MAX_LIGTHS];
layout(location = 3+MAX_LIGTHS * 2) uniform float light_intensity[MAX_LIGTHS];
layout(location = 3+MAX_LIGTHS * 3) uniform int n_ligths;

layout(location = 0) out vec4 out_color;

vec3 brdf(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 lColor, float d, float intensity)
{
	vec3 reflectDir = reflect(-lightDir, normal);
	float specF = max(dot(reflectDir, viewDir), 0.f);
	float i = intensity / (1 + d*d);
	return i * (dot(lightDir, normal) * lColor * 0.5 + 0.5 * pow(specF, SPECULAR)*vec3(1.f));
}

void main()
{
	vec3 tColor = texture(tx_diffuse, in_texCoord).rgb;
	vec3 lColor = vec3(0);
	vec3 viewDir = normalize(in_world_position.xyz);
	for ( int i = 0; i < MAX_LIGTHS; ++i) {
		if (i < n_ligths) {
			float d = length(in_world_position - light_position[i]);
			vec3 lightDir = normalize(light_position[i]);
			lColor += brdf(lightDir, viewDir, in_normal.xyz, light_color[i], d, light_intensity[i]);
		}
	}
	out_color = vec4(lColor*tColor, 1);
}
