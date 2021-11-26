#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 transformMatrix;
uniform float miny;
uniform float maxy;
out vec3 color;
void main()
{
   gl_Position = transformMatrix * vec4(aPos, 1);
   float ypos = (aPos.y - miny) / (maxy - miny);
   color = vec3(ypos, ypos, ypos);
}