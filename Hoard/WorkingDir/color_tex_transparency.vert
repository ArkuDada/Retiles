#version 330 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColor;
layout(location = 2) in vec2 VertexTexCoord;
		
uniform mat4 MVP;
uniform float offsetX;
uniform float offsetY;

out vec3 Color;
out vec2 TexCoord;

void main( void )
{
	Color = VertexColor;
	TexCoord.x = VertexTexCoord.x + offsetX;
	TexCoord.y = 1.0 - (VertexTexCoord.y + offsetY);

	gl_Position = MVP * vec4(VertexPosition,1.0f);
}
