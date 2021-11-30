#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 slopeVec;
uniform mat4 transformMatrix;
uniform float miny;
uniform float maxy;
out vec3 color;
void main() {
   gl_Position = transformMatrix * vec4(aPos.x, aPos.z, aPos.y, 1);
   float height = (aPos.y - miny) / (maxy - miny);
   color = vec3(height, slopeVec.x, slopeVec.z);
}