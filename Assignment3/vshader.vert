#version 140
#extension GL_ARB_compatibility: enable 

out vec4 color;

uniform mat4 Mv;

void main() 
{
   gl_Position = Mv * gl_Vertex; 
   color = gl_Color;   
}


