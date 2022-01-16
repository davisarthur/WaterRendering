#version 330 core
out vec4 FragColor;
  
in vec3 vertexPosition;
in vec3 normal;
in vec4 color;

uniform vec3 eye;

void main() { 
    FragColor = color;
}