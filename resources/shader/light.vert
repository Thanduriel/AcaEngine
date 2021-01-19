#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoord;

layout(location = 0) out vec4 out_normal;
layout(location = 1) out vec2 out_texCoords;
layout(location = 2) out vec4 out_world_position;

layout(location = 0) uniform mat4 c_modelViewProjection;
layout(location = 1) uniform mat4 c_normalModelView;
layout(location = 2) uniform mat4 c_worldProjection;

void main()
{
	gl_Position = c_modelViewProjection * vec4(in_position, 1.0);
	out_normal = c_normalModelView * vec4(in_normal, 0.0);
	out_texCoords = in_texCoord;
	out_world_position = c_worldProjection * vec4(in_position, 1.0);
}
