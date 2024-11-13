#version 330 core
in vec4 vertexColor;
in vec3 v2fNormal;
in vec3 worldPos;
out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 cameraPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(v2fNormal);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * vertexColor.xyz;
    FragColor = vec4(result, 1.0);
} 