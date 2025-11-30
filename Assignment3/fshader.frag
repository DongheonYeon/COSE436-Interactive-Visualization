#version 140
#extension GL_ARB_compatibility: enable

in vec2 vUV;
uniform sampler2D u_texture;    // texel읽어옴
out vec4 fColor;

void main()
{
    fColor = texture(u_texture, vUV);   // 최종 pixel을 texel 색으로
}
