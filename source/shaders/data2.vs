#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 slopeVec;

uniform mat4 proj;
uniform float miny;
uniform float maxy;
uniform float minSlopeX;
uniform float maxSlopeX;
uniform float minSlopeZ;
uniform float maxSlopeZ;

out vec3 color;

void main() {
   float height = abs(aPos.x + aPos.z);
   gl_Position = proj * vec4(aPos.x, aPos.z, 0, 1);
   color = vec3(height, height, height);
}