#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 proj;
uniform float miny;
uniform float maxy;

out vec3 color;

void main() {
   float ypos = (aPos.y - miny) / (maxy - miny);
   gl_Position = proj * vec4(aPos.x, aPos.z, ypos, 1);
   color = vec3(ypos, ypos, ypos);
}