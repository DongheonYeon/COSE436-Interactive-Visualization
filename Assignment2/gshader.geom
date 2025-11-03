#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vViewPosition[];
in vec3 vViewNormal[];

out vec3 gViewPosition;
out vec3 gViewNormal;

uniform int UseFlatShading;

void main()
{
    vec3 faceNormal = vec3(0.0);
    if (UseFlatShading != 0) {
        vec3 edge1 = vViewPosition[1] - vViewPosition[0];
        vec3 edge2 = vViewPosition[2] - vViewPosition[0];
        faceNormal = normalize(cross(edge1, edge2));
    }

    for (int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        gViewPosition = vViewPosition[i];
        gViewNormal = (UseFlatShading != 0) ? faceNormal : normalize(vViewNormal[i]);
        EmitVertex();
    }
    EndPrimitive();
}
