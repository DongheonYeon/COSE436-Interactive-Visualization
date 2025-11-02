#version 140
#extension GL_ARB_compatibility: enable

out vec4 color;

uniform mat4 Mvp;

void main() 
{
   gl_Position = Mvp * gl_Vertex; 
   color = gl_Color;   
}


