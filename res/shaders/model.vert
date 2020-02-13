#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	FragPos = vec3(u_Model * vec4(aPos, 1.0));
	Normal = mat3(u_Model) * aNormal;
	
	gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}
