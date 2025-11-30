#version 140
#extension GL_ARB_compatibility: enable

uniform mat4 Mv;
out vec2 vUV;   // vertex UV

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vUV = gl_MultiTexCoord0.st;
}


