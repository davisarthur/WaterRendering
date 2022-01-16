#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform float Lx;
uniform float Lz;
uniform float maxY;
uniform float minY;
uniform float maxSlopeX;
uniform float minSlopeX;
uniform float maxSlopeZ;
uniform float minSlopeZ;
uniform sampler2D waterTexture;

out vec3 vertexPosition;
out vec3 normal;
out vec4 color;

void main() {
    vec2 TexCoords = vec2(aPos.x, aPos.z);
    vec4 data = texture(waterTexture, TexCoords);
    vertexPosition = vec3(aPos.x, data.x * 10.0, aPos.z);
    gl_Position = transform * vec4(vertexPosition, 1);
    color = data;
}  