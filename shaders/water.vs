#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform float Lx;
uniform float Lz;

out vec2 TexCoords;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoords = vec2(aPos.x / Lx, aPos.z / Lz);
}  