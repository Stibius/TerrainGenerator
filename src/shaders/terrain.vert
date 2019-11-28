#version 430

in vec3 position;
in vec3 normal;
in vec3 texCoords;

uniform mat4 cameraVP;
uniform mat4 lightVP;
uniform vec3 cameraPos;

out vec3 TexCoords;
out vec3 Normal;
out vec3 WorldPos;
out vec3 CameraPos;
out vec3 LightSpacePos;

void main()
{
    gl_Position = cameraVP * vec4(position, 1);
    WorldPos = position;
    CameraPos = cameraPos;
    TexCoords = texCoords;
    Normal = normal;
    LightSpacePos = vec3(lightVP * vec4(position, 1));
}
