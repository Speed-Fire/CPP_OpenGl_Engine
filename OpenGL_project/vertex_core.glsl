#version 440

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texcoord;
layout (location = 3) in vec3 vertex_normal;

out vec3 vs_position;
out vec3 vs_color;
out vec2 vs_texcoord;
out vec3 vs_normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
//	if(ModelMatrix != 0)
//	{
//		vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
//	}
//	else
//	{
//		vs_position = vertex_position;
//	}
vs_position = vec4(ModelMatrix * vec4(vertex_position, 1.f)).xyz;
	
	vs_color = vertex_color;
	vs_texcoord = vec2(vertex_texcoord.x, vertex_texcoord.y * -1.f);

	vs_normal = mat3(ModelMatrix) * vertex_normal;

//	if(ModelMatrix != 0)
//	{
//		gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
//	}
//	else
//	{
//		gl_Position = ProjectionMatrix * ViewMatrix * vec4(vertex_position, 1.f);
//	}
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(vertex_position, 1.f);
}