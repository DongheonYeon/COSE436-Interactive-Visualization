#version 140
#extension GL_ARB_compatibility: enable

in vec3 pixelPosition;
uniform vec3 eyePosition;
uniform vec3 objectMin;
uniform vec3 objectMax;
uniform vec3 up;
uniform sampler3D tex;


void main()
{
	vec4 composedColor=vec4(0,0,0,0);

	// .. ToDo

	
    gl_FragColor = composedColor;
}