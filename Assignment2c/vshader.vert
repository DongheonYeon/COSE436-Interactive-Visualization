#version 150 compatibility

// Geometry shader로 넘길 뷰 공간 좌표 및 노멀
out vec3 vViewPosition;
out vec3 vViewNormal;

void main()
{
    // 고전 파이프라인으로부터 정점/노멀을 받아 뷰 공간으로 변환
    vec4 viewPosition = gl_ModelViewMatrix * gl_Vertex;
    vViewPosition = viewPosition.xyz;
    vViewNormal = normalize(gl_NormalMatrix * gl_Normal);

    gl_Position = gl_ProjectionMatrix * viewPosition;
}
