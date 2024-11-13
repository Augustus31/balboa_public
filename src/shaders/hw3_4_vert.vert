#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec4 vertexColor;
out vec3 v2fNormal;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 homo = vec4(aPos.xyz, 1.0);
    vec4 fPos = projection * view * model * homo; 
    gl_Position = fPos;
    vertexColor = vec4(color, 1.0f);
    v2fNormal = mat3(transpose(inverse(model))) * normal;
    vec4 worldPosHomo = model * homo;
    worldPos = worldPosHomo.xyz/worldPosHomo.w;
}