#version 450

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_texCoord;

layout(binding = 0) uniform sampler2D tx_diffuse;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 diffuse = texture(tx_diffuse, in_texCoord).rgb;
	float cosTheta = clamp( dot( in_normal, vec4(0.57737,0.57737,0.57737,0) ), 0.3,1.0 );
	
	out_color = vec4(diffuse * cosTheta, 1.0);
}