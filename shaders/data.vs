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
   gl_Position = proj * vec4(aPos.x, aPos.z, aPos.y, 1);
   float height = (aPos.y - miny) / (maxy - miny);
   float slopeX = (slopeVec.x - minSlopeX) / (maxSlopeX - minSlopeX);
   float slopeZ = (slopeVec.z - minSlopeZ) / (maxSlopeZ - minSlopeZ);
   color = vec3(height, slopeX, slopeZ);
}