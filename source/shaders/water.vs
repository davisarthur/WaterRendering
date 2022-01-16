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
    vec2 TexCoords = vec2(aPos.x / Lx + Lx / 2.0, aPos.z / Lz + Lz / 2.0);
    vec4 data = texture(waterTexture, TexCoords);
    float height = minY + data.x * (maxY - minY);
    float slopeX = minSlopeX + data.y * (maxSlopeX - minSlopeX);
    float slopeZ = minSlopeZ + data.z * (maxSlopeZ - minSlopeZ);
    vertexPosition = vec3(aPos.x, height, aPos.z);
    normal = normalize(vec3(-slopeX, 1, -slopeZ));
    gl_Position = transform * vec4(vertexPosition, 1);
}  