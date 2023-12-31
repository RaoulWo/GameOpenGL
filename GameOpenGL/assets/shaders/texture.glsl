#type vertex
#version 330 core
			    
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);	
}

#type fragment
#version 330 core
			    
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform float u_TilingFactor;

void main()
{
	color = texture(u_Texture, v_TexCoord * u_TilingFactor) * u_Color;
}