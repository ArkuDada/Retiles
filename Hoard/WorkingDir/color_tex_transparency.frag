#version 330 core

in vec3 Color;
in vec2 TexCoord;

uniform sampler2D tex1;
uniform float alpha;
uniform int mode;			//0 - color, 1 - texture

out vec4 Color0;

void main( void )
{
	vec4 texColor = texture( tex1, TexCoord);
	vec4 finalColor = vec4(0.0f,0.0f,0.0f,1.0f);

	if (mode == 0){
		finalColor = vec4(Color,alpha);
	}else{
		texColor.rgb *= alpha;
		finalColor = texColor;
	}

	Color0 = finalColor;
}