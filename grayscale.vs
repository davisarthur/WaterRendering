#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 transformMatrix;
out vec3 color;
void main()
{
   gl_Position = transformMatrix * vec4(aPos, 1);
   float ypos = 1000 * aPos.y;
   color = vec3(ypos, ypos, ypos);
}