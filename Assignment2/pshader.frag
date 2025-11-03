#version 140
#extension GL_ARB_compatibility: enable

in vec3 gViewPosition;
in vec3 gViewNormal;

out vec4 fColor;

const int MAX_LIGHTS = 4;

uniform int NumLights;
uniform vec3 LightPositions[MAX_LIGHTS];
uniform vec3 AmbientProducts[MAX_LIGHTS];
uniform vec3 DiffuseProducts[MAX_LIGHTS];
uniform vec3 SpecularProducts[MAX_LIGHTS];
uniform float Shininess;

void main()
{
    vec3 N = normalize(gViewNormal);
    vec3 V = normalize(-gViewPosition);

    vec3 accumulatedAmbient = vec3(0.0);
    vec3 accumulatedDiffuse = vec3(0.0);
    vec3 accumulatedSpecular = vec3(0.0);

    for (int i = 0; i < NumLights; ++i) {
        vec3 L = normalize(LightPositions[i] - gViewPosition);
        vec3 R = reflect(-L, N);

        float diff = max(dot(N, L), 0.0);
        float spec = 0.0;
        if (diff > 0.0) {
            spec = pow(max(dot(R, V), 0.0), Shininess);
        }

        accumulatedAmbient += AmbientProducts[i];
        accumulatedDiffuse += DiffuseProducts[i] * diff;
        accumulatedSpecular += SpecularProducts[i] * spec;
    }

    vec3 finalColor = accumulatedAmbient + accumulatedDiffuse + accumulatedSpecular; // no attenuation factor
    fColor = vec4(finalColor, 1.0);
}
