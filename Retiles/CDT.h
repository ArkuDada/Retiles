

#ifndef CDT_LIB
#define CDT_LIB

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

#include "shader.hpp"
#include "SOIL.h"

#define PI 3.1415926

// -------------------------------------------
// CDT data type
// -------------------------------------------

struct CDTVertex
{
	float x, y, z;
	float r, g, b;
	float u, v;
};

struct CDTMesh
{
	GLuint		vaoHandle;
	GLuint		vertexBuffer;
	std::vector<CDTVertex> vertex;
};


typedef GLuint CDTTex;

#define CDT_COLOR 0
#define CDT_TEXTURE 1
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// -------------------------------------------
// Init & Shutdown
// -------------------------------------------

void CDTInit(int width, int height);
void CDTShutdown();
int  GetWindowWidth();
int  GetWindowHeight();

// -------------------------------------------
// CDT Mesh functions
// -------------------------------------------

CDTMesh CreateMesh(std::vector<CDTVertex> in_vertex);
void DrawMesh(CDTMesh &mesh);
void UnloadMesh(CDTMesh &mesh);

// -------------------------------------------
// CDT Texture functions
// -------------------------------------------

CDTTex TextureLoad(const char* filename);
void TextureUnload(CDTTex &tex);

// -------------------------------------------
// CDT Camera function
// -------------------------------------------

void MoveCam(float dx, float dy);
void ZoomIn(float step);
void ZoomOut(float step);
void RotateCam(float degree);
void SetCamPosition(float xpos, float ypos);
void SetCamZoom(float zoom);
void SetCamRotation(float degree);
void ResetCam();

// -------------------------------------------
// CDT Renderer function
// -------------------------------------------

void SetRenderMode(int mode, float alpha);
void SetTexture(CDTTex tex, float offsetX, float offsetY);
void SetTransform(const glm::mat4 &modelMat);



#endif 