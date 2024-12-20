#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 homo = vec4(aPos.xyz, 1.0);
    vec4 fPos = projection * view * model * homo; 
    gl_Position = fPos;
    vertexColor = vec4(color, 1.0f);
}