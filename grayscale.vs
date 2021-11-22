#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 color;
void main()
{
   gl_Position = vec4(aPos, 1.0);
   zpos = normalize(aPos).z;
   color = vec4(zpos, zpos, zpos, 1);
}