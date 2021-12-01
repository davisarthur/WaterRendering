#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform float Lx;
uniform float Lz;

out vec2 TexCoords;
out vec2 xzPos;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoords = vec2(aPos.x / Lx + Lx / 4.0, aPos.z / Lz + Lz / 4.0);
    xzPos = vec2(aPos.x, aPos.z);
}  