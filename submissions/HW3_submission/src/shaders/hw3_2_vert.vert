#version 330 core
layout (location = 0) in vec3 aPos;

uniform float angle;

void main()
{
    mat3 rotMat;
    rotMat[0] = vec3(cos(radians(angle)), -sin(radians(angle)), 0);
    rotMat[1] = vec3(sin(radians(angle)), cos(radians(angle)), 0);
    rotMat[2] = vec3(0, 0, 1);
    vec3 rotpos = rotMat * aPos;
    gl_Position = vec4(rotpos.x, rotpos.y, rotpos.z, 1.0);
}