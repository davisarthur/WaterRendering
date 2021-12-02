#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 proj;
uniform float miny;
uniform float maxy;

out vec3 color;

void main() {
   gl_Position = proj * vec4(aPos.x, aPos.z, 0, 1);
   float ypos = (aPos.y - miny) / (maxy - miny);
   color = vec3(ypos, ypos, ypos);
}