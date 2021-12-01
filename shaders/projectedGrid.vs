#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
uniform mat4 proj;
uniform mat4 lookAt1;
out vec3 color;
void main() {
    gl_Position = vec4(aPos, 1);
    vec4 actualPos = inverse(proj * lookAt1) * vec4(aPos, 1);
    color = vec3(1.0, 1.0, 1.0);
}