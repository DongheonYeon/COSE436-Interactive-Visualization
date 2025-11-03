/*
 * Skeleton code for COSE436 Fall 2025 Assignment 2
 *
 * Won-Ki Jeong, wkjeong@korea.ac.kr
 */

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <assert.h>
#include "textfile.h"
#include "Angel.h"

#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

// Shader programs
GLuint p[3];
GLint ambientProductsLoc = -1;
GLint diffuseProductsLoc = -1;
GLint specularProductsLoc = -1;
GLint lightPositionsLoc = -1;
GLint numLightsLoc = -1;
GLint useFlatShadingLoc = -1;
GLint shininessLoc = -1;
struct LightSource
{
    GLfloat position[3];
    GLfloat ambient[3];
    GLfloat diffuse[3];
    GLfloat specular[3];
};

const int gLightsNum = 3;
// const LightSource gLights[] = {
//     // Position, La, Ld, Ls
//     { {-0.5f, 0.0f, 0.5f}, {1.00f, 0.00f, 0.00f}, {1.00f, 0.00f, 0.00f}, {1.00f, 0.00f, 0.00f} },   // Left, Red
//     { {0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },    // Right, Green
//     { {0.5f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }    // Top, Blue
// };
const LightSource gLights[] = {
    // Position, La, Ld, Ls
    { { 0.0f, 0.5f, 2.0f }, { 0.30f, 0.15f, 0.15f }, { 0.85f, 0.20f, 0.20f }, { 1.00f, 0.60f, 0.60f } },
    { { -1.5f, 1.5f, 1.5f }, { 0.10f, 0.10f, 0.30f }, { 0.20f, 0.20f, 0.80f }, { 0.60f, 0.60f, 1.00f } },
    { { 1.5f, 1.2f, -1.2f }, { 0.10f, 0.25f, 0.10f }, { 0.20f, 0.80f, 0.20f }, { 0.60f, 1.00f, 0.60f } }
};

GLfloat gMaterialBaseColor[3] = { 1.0f, 1.0f, 1.0f }; // white
const GLfloat gSpecularBaseColor[3] = { 1.0f, 1.0f, 1.0f }; // white
float gAmbientCoeff = 0.5f;
float gDiffuseCoeff = 0.5f;
float gSpecularCoeff = 0.5f;
float gShininess = 125.0f;

GLfloat gAmbientProducts[gLightsNum][3];
GLfloat gDiffuseProducts[gLightsNum][3];
GLfloat gSpecularProducts[gLightsNum][3];

struct MeshData
{
    std::vector<Angel::vec3> positions;
    std::vector<Angel::vec3> normals;
    std::vector<unsigned int> indices;
    Angel::vec3 center = Angel::vec3(0.0f, 0.0f, 0.0f);
    float scale = 1.0f;
};

std::vector<std::string> gMeshPaths = {
    "../mesh-data/bunny.off",
    "../mesh-data/dragon-full.off",
    "../mesh-data/fandisk.off"
};
int gCurrentMeshIndex = 0;

bool isMeshLoaded = false;
GLuint gVertexBuffer = 0;
GLuint gNormalBuffer = 0;
GLuint gIndexBuffer = 0;
GLsizei gIndexCount = 0;
Angel::vec3 gMeshCenter(0.0f, 0.0f, 0.0f);
float gMeshScale = 1.0f;
size_t gMeshVertexCount = 0;
size_t gMeshFaceCount = 0;
std::string gActiveMeshLabel;

bool LoadOFFMesh(const std::string& path, MeshData& outMesh);
bool LoadMeshByPath(const std::string& path);
void ReleaseMeshBuffers();
bool UploadMeshToGPU(const MeshData& data);
void DrawMesh();
void SelectMesh(int delta);

enum ShadingMode
{
    ShadingSmooth = 0,
    ShadingFlat = 1
};
int gShadingMode = ShadingSmooth;

float gModelRotation = 0.0f;
bool isRotating = false;

void uploadLightingUniforms() {
    glUseProgram(p[0]);

    GLfloat materialAmbient[3];
    GLfloat materialDiffuse[3];
    GLfloat materialSpecular[3];
    for (int i = 0; i < 3; ++i) {
        materialAmbient[i] = gMaterialBaseColor[i] * gAmbientCoeff;
        materialDiffuse[i] = gMaterialBaseColor[i] * gDiffuseCoeff;
        materialSpecular[i] = gSpecularBaseColor[i] * gSpecularCoeff;
    }

    for (int lightIdx = 0; lightIdx < gLightsNum; ++lightIdx) {
        for (int channel = 0; channel < 3; ++channel) {
            gAmbientProducts[lightIdx][channel] = materialAmbient[channel] * gLights[lightIdx].ambient[channel];
            gDiffuseProducts[lightIdx][channel] = materialDiffuse[channel] * gLights[lightIdx].diffuse[channel];
            gSpecularProducts[lightIdx][channel] = materialSpecular[channel] * gLights[lightIdx].specular[channel];
        }
    }

    GLfloat lightPositions[gLightsNum * 3] = { 0.0f };
    for (int lightIdx = 0; lightIdx < gLightsNum; ++lightIdx) {
        for (int channel = 0; channel < 3; ++channel) {
            lightPositions[lightIdx * 3 + channel] = gLights[lightIdx].position[channel];
        }
    }

    if (numLightsLoc >= 0) {
        glUniform1i(numLightsLoc, gLightsNum);
    }
    if (lightPositionsLoc >= 0) {
        glUniform3fv(lightPositionsLoc, gLightsNum, lightPositions);
    }
    if (ambientProductsLoc >= 0) {
        glUniform3fv(ambientProductsLoc, gLightsNum, &gAmbientProducts[0][0]);
    }
    if (diffuseProductsLoc >= 0) {
        glUniform3fv(diffuseProductsLoc, gLightsNum, &gDiffuseProducts[0][0]);
    }
    if (specularProductsLoc >= 0) {
        glUniform3fv(specularProductsLoc, gLightsNum, &gSpecularProducts[0][0]);
    }
    if (useFlatShadingLoc >= 0) {
        glUniform1i(useFlatShadingLoc, gShadingMode == ShadingFlat);
    }
    if (shininessLoc >= 0) {
        glUniform1f(shininessLoc, gShininess);
    }
}

void ReleaseMeshBuffers() {
    if (gVertexBuffer != 0) {
        glDeleteBuffers(1, &gVertexBuffer);
        gVertexBuffer = 0;
    }
    if (gNormalBuffer != 0) {
        glDeleteBuffers(1, &gNormalBuffer);
        gNormalBuffer = 0;
    }
    if (gIndexBuffer != 0) {
        glDeleteBuffers(1, &gIndexBuffer);
        gIndexBuffer = 0;
    }
    gIndexCount = 0;
    isMeshLoaded = false;
}

bool UploadMeshToGPU(const MeshData& data) {
    if (data.positions.empty() || data.indices.empty()) {
        return false;
    }

    ReleaseMeshBuffers();

    glGenBuffers(1, &gVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(data.positions.size() * sizeof(Angel::vec3)),
                 data.positions.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &gNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(data.normals.size() * sizeof(Angel::vec3)),
                 data.normals.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &gIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(data.indices.size() * sizeof(unsigned int)),
                 data.indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    gIndexCount = static_cast<GLsizei>(data.indices.size());
    gMeshCenter = data.center;
    gMeshScale = data.scale;
    gMeshVertexCount = data.positions.size();
    gMeshFaceCount = data.indices.size() / 3;
    isMeshLoaded = true;

    return true;
}

bool LoadOFFMesh(const std::string& path, MeshData& outMesh) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open OFF file: " << path << std::endl;
        return false;
    }

    auto readDataLine = [&file](std::string& line) -> bool {
        while (std::getline(file, line)) {
            std::size_t comment = line.find('#');
            if (comment != std::string::npos) {
                line = line.substr(0, comment);
            }
            if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
                continue;
            }
            return true;
        }
        return false;
    };

    std::string OFF_header;
    std::string line;
    readDataLine(OFF_header);
    readDataLine(line);

    std::istringstream countsStream(line);
    std::size_t vertexCount = 0;
    std::size_t faceCount = 0;
    std::size_t edgeCount = 0;
    countsStream >> vertexCount >> faceCount >> edgeCount;
    static_cast<void>(edgeCount);

    outMesh.positions.resize(vertexCount);
    outMesh.normals.assign(vertexCount, Angel::vec3(0.0f, 0.0f, 0.0f));
    outMesh.indices.clear();
    outMesh.indices.reserve(faceCount * 3);

    Angel::vec3 minBounds(std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::max(),
                          std::numeric_limits<float>::max());
    Angel::vec3 maxBounds(-std::numeric_limits<float>::max(),
                          -std::numeric_limits<float>::max(),
                          -std::numeric_limits<float>::max());

    for (std::size_t v = 0; v < vertexCount; ) {
        readDataLine(line);

        std::istringstream vertexStream(line);
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        if (!(vertexStream >> x >> y >> z)) {
            continue;
        }

        outMesh.positions[v] = Angel::vec3(x, y, z);
        minBounds.x = std::min(minBounds.x, x);
        minBounds.y = std::min(minBounds.y, y);
        minBounds.z = std::min(minBounds.z, z);
        maxBounds.x = std::max(maxBounds.x, x);
        maxBounds.y = std::max(maxBounds.y, y);
        maxBounds.z = std::max(maxBounds.z, z);
        ++v;
    }

    for (std::size_t f = 0; f < faceCount; ) {
        readDataLine(line);

        std::istringstream faceStream(line);
        unsigned int verticesInFace = 0;
        if (!(faceStream >> verticesInFace)) {
            continue;
        }

        if (verticesInFace < 3) {
            // Skip malformed face definitions
            continue;
        }

        std::vector<unsigned int> faceIndices(verticesInFace);
        for (unsigned int k = 0; k < verticesInFace; ++k) {
            faceStream >> faceIndices[k];
        }

        for (unsigned int k = 1; k + 1 < verticesInFace; ++k) {
            unsigned int i0 = faceIndices[0];
            unsigned int i1 = faceIndices[k];
            unsigned int i2 = faceIndices[k + 1];

            if (i0 >= vertexCount || i1 >= vertexCount || i2 >= vertexCount) {
                continue;
            }

            outMesh.indices.push_back(i0);
            outMesh.indices.push_back(i1);
            outMesh.indices.push_back(i2);

            Angel::vec3 edge1 = outMesh.positions[i1] - outMesh.positions[i0];
            Angel::vec3 edge2 = outMesh.positions[i2] - outMesh.positions[i0];
            Angel::vec3 normal = Angel::cross(edge1, edge2);

            // Angel::vec3 normal = Angel::cross(outMesh.positions[i1] - outMesh.positions[i0], 
            //                                     outMesh.positions[i2] - outMesh.positions[i0]);

            outMesh.normals[i0] += normal;
            outMesh.normals[i1] += normal;
            outMesh.normals[i2] += normal;
        }

        ++f;
    }

    const float epsilon = 1e-10f;
    for (auto& normal : outMesh.normals) {
        float len = Angel::length(normal);
        if (len > epsilon) {
            normal /= len;
        } else {
            normal = Angel::vec3(0.0f, 1.0f, 0.0f);
        }
    }

    outMesh.center = (minBounds + maxBounds) * 0.5f;
    float maxRadius = 0.0f;
    for (const auto& position : outMesh.positions) {
        float dist = Angel::length(position - outMesh.center);
        maxRadius = std::max(maxRadius, dist);
    }

    outMesh.scale = 1.0f / maxRadius;
    return true;
}

bool LoadMeshByPath(const std::string& path) {
    MeshData data;
    if (!LoadOFFMesh(path, data)) {
        std::cerr << "LoadOFFMesh Fail: " << path << std::endl;
        return false;
    }
    if (!UploadMeshToGPU(data)) {
        std::cerr << "UploadMeshToGPU Fail: " << path << std::endl;
        return false;
    }

    gActiveMeshLabel = path;
    gModelRotation = 0.0f;
    uploadLightingUniforms();

    std::cout
        << "Loaded mesh: " << path
        << " (" << gMeshVertexCount << " vertices, "  << gMeshFaceCount << " triangles)"
    << std::endl;

    glutPostRedisplay();
    return true;
}

void DrawMesh() {
    if (!isMeshLoaded || gIndexCount == 0) {
        return;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
    glVertexPointer(3, GL_FLOAT, 0, reinterpret_cast<const GLvoid*>(0));

    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, gNormalBuffer);
    glNormalPointer(GL_FLOAT, 0, reinterpret_cast<const GLvoid*>(0));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
    glDrawElements(GL_TRIANGLES, gIndexCount, GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void SelectMesh(int delta) {
    const int meshCount = static_cast<int>(gMeshPaths.size());
    const int nextIndex = (gCurrentMeshIndex + delta + meshCount) % meshCount;


    if (LoadMeshByPath(gMeshPaths[nextIndex])) {
        gCurrentMeshIndex = nextIndex;
    } else {
        std::cerr << "Model Switch Fail" << std::endl;
    }
}

void adjustAmbient(float delta) {
    gAmbientCoeff = std::max(0.0f, std::min(1.0f, gAmbientCoeff + delta));
    printf("Ambient coefficient (k_a): %.2f\n", gAmbientCoeff);
    uploadLightingUniforms();
}

void adjustDiffuse(float delta) {
    gDiffuseCoeff = std::max(0.0f, std::min(1.0f, gDiffuseCoeff + delta));
    printf("Diffuse coefficient (k_d): %.2f\n", gDiffuseCoeff);
    uploadLightingUniforms();
}

void adjustSpecular(float delta) {
    gSpecularCoeff = std::max(0.0f, std::min(1.0f, gSpecularCoeff + delta));
    printf("Specular coefficient (k_s): %.2f\n", gSpecularCoeff);
    uploadLightingUniforms();
}

void adjustShininess(float delta) {
    gShininess = std::max(1.0f, std::min(256.0f, gShininess + delta));
    printf("Shininess (alpha): %.1f\n", gShininess);
    uploadLightingUniforms();
}

void changeSize(int w, int h)
{
    if (h == 0) { h = 1; }

    float ratio = 1.0f * static_cast<float>(w) / static_cast<float>(h);

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 0.1f, 50.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    const float step = 0.05f;
    const float shininessStep = 5.0f;
    switch (key) {
    case '1':
        adjustDiffuse(-step);
        break;
    case '3':
        adjustDiffuse(step);
        break;

    case '4':
        adjustAmbient(-step);
        break;
    case '6':
        adjustAmbient(step);
        break;

    case '7':
        adjustSpecular(-step);
        break;
    case '9':
        adjustSpecular(step);
        break;

    case '-':
    case '_':
        adjustShininess(-shininessStep);
        break;
    case '+':
    case '=':
        adjustShininess(shininessStep);
        break;

    case 's':
    case 'S':
        gShadingMode = ShadingSmooth;
        printf("Shading mode: smooth\n");
        uploadLightingUniforms();
        break;
    case 'p':
    case 'P':
        gShadingMode = ShadingFlat;
        printf("Shading mode: flat\n");
        uploadLightingUniforms();
        break;
    case 'n':
    case 'N':
        SelectMesh(+1);
        break;
    case 'm':
    case 'M':
        SelectMesh(-1);
        break;

    case ' ':
        isRotating = !isRotating;
        printf("Rotation %s\n", isRotating ? "ON" : "OFF");
        break;

    case 27: // ESC key
        printf("Exiting program.\n");
        exit(0);

    default:
        printf("Undefined key: %c\n", key);
        break;
    }

    glutPostRedisplay();
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p[0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.5f, 3.0f,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    // === model rotation ===
    if (isRotating) {
        gModelRotation += 0.2f;
        if (gModelRotation >= 360.0f) {
            gModelRotation -= 360.0f;
        }
    }
    glRotatef(gModelRotation, 0.0f, 1.0f, 0.0f);
    // === model rotation ===

    if (isMeshLoaded) {
        glScalef(gMeshScale, gMeshScale, gMeshScale);
        glTranslatef(-gMeshCenter.x, -gMeshCenter.y, -gMeshCenter.z);
        DrawMesh();
    } else {
        glScalef(1.0f, 1.0f, 1.0f);
        glFrontFace(GL_CW);
        glutSolidTeapot(1.0);
        glFrontFace(GL_CCW);
    }

    glutSwapBuffers();
}

void idle()
{
    glutPostRedisplay();
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Create shader program
    p[0] = createGLSLProgram( "../vshader.vert", "../gshader.geom", "../pshader.frag" );

    glUseProgram(p[0]);

    ambientProductsLoc = glGetUniformLocation(p[0], "AmbientProducts[0]");
    diffuseProductsLoc = glGetUniformLocation(p[0], "DiffuseProducts[0]");
    specularProductsLoc = glGetUniformLocation(p[0], "SpecularProducts[0]");
    lightPositionsLoc = glGetUniformLocation(p[0], "LightPositions[0]");
    numLightsLoc = glGetUniformLocation(p[0], "NumLights");
    useFlatShadingLoc = glGetUniformLocation(p[0], "UseFlatShading");
    shininessLoc = glGetUniformLocation(p[0], "Shininess");

    uploadLightingUniforms();

    if (!LoadMeshByPath(gMeshPaths[gCurrentMeshIndex])) {
        std::cerr << "Model loading failed: " << gMeshPaths[gCurrentMeshIndex] << std::endl;
    }
 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

int main(int argc, char **argv) {
    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("COSE436 - Assignment 2");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

    glewInit();
    if (glewIsSupported("GL_VERSION_3_3"))
        printf("Ready for OpenGL 3.3\n");
    else {
        printf("OpenGL 3.3 is not supported\n");
        exit(1);
    }

    init();

    // enter GLUT event processing cycle
    glutMainLoop();

    return 1;
}
