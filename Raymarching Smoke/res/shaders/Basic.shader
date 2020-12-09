#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec3 v_Normal;
out vec4 v_Position;

uniform mat4 u_MVP;
uniform mat3 u_Light;

void main()
{
	gl_Position = u_MVP * position;
	v_Normal = normal;
	v_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec4 v_Position;

void main()
{
	//vec4 texColor = texture(u_Texture, v_TexCoord);
	//vec3 normalLight = normalize(light);
	vec3 normalized = normalize(v_Normal);
	color = vec4(normalized.xyz, 1);
};