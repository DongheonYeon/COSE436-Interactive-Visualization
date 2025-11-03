#version 140
#extension GL_ARB_compatibility: enable

// Geometry shader로 넘길 뷰포지션, 뷰노말
out vec3 vViewPosition;
out vec3 vViewNormal;

void main()
{
    // 버텍스, 노말을 받아 뷰 공간으로 변환
    vec4 viewPosition = gl_ModelViewMatrix * gl_Vertex;
    vViewPosition = viewPosition.xyz;
    vViewNormal = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ProjectionMatrix * viewPosition;
}
