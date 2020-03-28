#version 450

//layout(location = 0) in vec4 in_position;
layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_texCoord;

layout(location = 0) out vec4 out_color;

void main()
{
	float cosTheta = clamp( dot( in_normal, vec4(0.57737,0.57737,0.57737,0) ), 0.1,0.7 );
	
	out_color = vec4(cosTheta, cosTheta, cosTheta, 1.0);
}