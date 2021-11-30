#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 slopeVec;

uniform mat4 transformMatrix;

out vec3 vertexPosition;
out vec3 normal;

void main() {
   gl_Position = transformMatrix * vec4(aPos.x, aPos.y, aPos.z, 1);
   vertexPosition = vec3(aPos);
   normal = normalize(vec3(-slopeVec.x, 1.0, -slopeVec.z));
}