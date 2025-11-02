#version 150 compatibility

out vec3 vViewPosition;
out vec3 vViewNormal;

void main()
{
    vec4 viewPosition = gl_ModelViewMatrix * gl_Vertex;
    vViewPosition = viewPosition.xyz;
    vViewNormal = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ProjectionMatrix * viewPosition;
}
