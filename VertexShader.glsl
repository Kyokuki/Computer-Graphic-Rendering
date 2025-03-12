#version 330 core

layout (location = 0) in vec3 Vertex;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 Normal;

out vec3 ObjColor;
out vec3 FragNorm;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    FragPos = Vertex;
    ObjColor = Color;
    FragNorm = Normal;
    gl_Position = proj * view * vec4(Vertex, 1.0);
}
