

#include "CDT.h"

// -------------------------------------------
// CDT global variables
// -------------------------------------------

// Camera variables
glm::vec3	cdt_campos;
glm::vec3	cdt_camdir;
glm::vec3	cdt_camup;
float		cdt_camzoom;
float		cdt_camdegree;
glm::mat4	cdt_ViewMatrix;
glm::mat4	cdt_ProjectionMatrix;

// Render 
int			cdt_width;
int			cdt_height;
GLuint		cdt_programID;
int			cdt_rendermode;
float		cdt_tranparency;
glm::mat4	cdt_MVP;
CDTTex		cdt_blanktex;


// -------------------------------------------
// Init & Shutdown
// -------------------------------------------

void CDTInit(int width, int height)
{
	srand(time(NULL));

	cdt_width = width;
	cdt_height = height;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cdt_programID = LoadShaders("color_tex_transparency.vert", "color_tex_transparency.frag");
	cdt_blanktex = TextureLoad("blank.png");
	cdt_tranparency = 1.0f;

	// set cam, model view proj matrix
	cdt_campos = glm::vec3(0.0f, 0.0f, 0.0f);
	cdt_camdir = glm::vec3(0.0f, 0.0f, -1.0f);
	cdt_camup = glm::vec3(0.0f, 1.0f, 0.0f);
	cdt_camzoom = 1.0f;
	cdt_camdegree = 0.0f;
	cdt_ProjectionMatrix = glm::ortho(-(cdt_width/2)*cdt_camzoom, (cdt_width/2)*cdt_camzoom, -(cdt_height/2)*cdt_camzoom, (cdt_height/2)*cdt_camzoom, -10.0f, 10.0f);
	cdt_ViewMatrix = glm::lookAt(cdt_campos, cdt_campos + cdt_camdir, cdt_camup);

}

void CDTShutdown()
{
	glDeleteProgram(cdt_programID);
	TextureUnload(cdt_blanktex);
}

int  GetWindowWidth()
{
	return cdt_width;
}

int  GetWindowHeight()
{
	return cdt_height;
}

// -------------------------------------------
// CDT Mesh functions
// -------------------------------------------

CDTMesh CreateMesh(std::vector<CDTVertex> in_vertex)
{
	CDTMesh aMesh;
	aMesh.vertex = in_vertex;

	glGenBuffers(1, &aMesh.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, aMesh.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, aMesh.vertex.size() * sizeof(CDTVertex), &aMesh.vertex[0].x, GL_STATIC_DRAW);

	glGenVertexArrays(1, &aMesh.vaoHandle);
	glBindVertexArray(aMesh.vaoHandle);
	glBindBuffer(GL_ARRAY_BUFFER, aMesh.vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CDTVertex), BUFFER_OFFSET(0));		//The starting point of the VBO, for the vertices
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CDTVertex), BUFFER_OFFSET(12));     //The starting point of color, 12 bytes away
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CDTVertex), BUFFER_OFFSET(24));
	
	glBindVertexArray(0);
	
	return aMesh;
}

void DrawMesh(CDTMesh &mesh)
{
	glBindVertexArray(mesh.vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertex.size());

	glBindVertexArray(0);
}

void UnloadMesh(CDTMesh &mesh)
{
	glDeleteBuffers(1, &mesh.vertexBuffer);
	glDeleteVertexArrays(1, &mesh.vaoHandle);

	mesh.vertex.clear();
}

// -------------------------------------------
// CDT Texture functions
// -------------------------------------------

CDTTex TextureLoad(const char* filename)
{
	CDTTex aTex;

	GLubyte*	pData;
	int			texWidth, texHeight, channels;

	pData = SOIL_load_image(filename, &texWidth, &texHeight, &channels, SOIL_LOAD_AUTO);

	glGenTextures(1, &aTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLint format = (channels == 4) ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, pData);

	SOIL_free_image_data(pData);

	return aTex;
}


void TextureUnload(CDTTex &tex)
{
	glDeleteTextures(1, &tex);
}

// -------------------------------------------
// CDT Camera function
// -------------------------------------------

void MoveCam(float dx, float dy)
{
	cdt_campos.x += dx;
	cdt_campos.y += dy;
	SetCamPosition(cdt_campos.x, cdt_campos.y);
}

void ZoomIn(float step)
{
	cdt_camzoom += step;
	SetCamZoom(cdt_camzoom);
}

void ZoomOut(float step)
{
	ZoomIn(-step);
}

void RotateCam(float degree)
{
	cdt_camdegree += degree;
	SetCamRotation(cdt_camdegree);
}

void SetCamPosition(float xpos, float ypos)
{
	cdt_campos.x = xpos;
	cdt_campos.y = ypos;
	cdt_ViewMatrix = glm::lookAt(cdt_campos, cdt_campos + cdt_camdir, cdt_camup);
}

void SetCamZoom(float zoom)
{
	cdt_camzoom = zoom;
	if (cdt_camzoom < 0.1f){ cdt_camzoom = 0.1f; }
	cdt_ProjectionMatrix = glm::ortho(-(cdt_width / 2)*cdt_camzoom, (cdt_width / 2)*cdt_camzoom, -(cdt_height / 2)*cdt_camzoom, (cdt_height / 2)*cdt_camzoom, -10.0f, 10.0f);
	cdt_ViewMatrix = glm::lookAt(cdt_campos, cdt_campos + cdt_camdir, cdt_camup);
}

void SetCamRotation(float degree)
{
	cdt_camdegree = degree;

	glm::vec3 newUp;
	newUp.x = - 1 * glm::sin(glm::radians(cdt_camdegree));
	newUp.y =  glm::cos(glm::radians(cdt_camdegree));
	newUp.z = cdt_camup.z;

	cdt_camup = newUp;

	cdt_ViewMatrix = glm::lookAt(cdt_campos, cdt_campos + cdt_camdir, cdt_camup);
}

void ResetCam()
{
	cdt_campos = glm::vec3(0.0f, 0.0f, 0.0f);
	cdt_camdir = glm::vec3(0.0f, 0.0f, -1.0f);
	cdt_camup = glm::vec3(0.0f, 1.0f, 0.0f);
	cdt_camzoom = 1.0f;
	cdt_camdegree = 0.0f;
	cdt_ProjectionMatrix = glm::ortho(-(cdt_width / 2)*cdt_camzoom, (cdt_width / 2)*cdt_camzoom, -(cdt_height / 2)*cdt_camzoom, (cdt_height / 2)*cdt_camzoom, -10.0f, 10.0f);
	cdt_ViewMatrix = glm::lookAt(cdt_campos, cdt_campos + cdt_camdir, cdt_camup);
}

// -------------------------------------------
// CDT Renderer function
// -------------------------------------------

void SetRenderMode(int mode, float alpha)
{
	glViewport(0, 0, cdt_width, cdt_height);
	glUseProgram(cdt_programID);

	glUniform1i(glGetUniformLocation(cdt_programID, "mode"), mode);
	glUniform1f(glGetUniformLocation(cdt_programID, "alpha"), alpha);

	// default setting
	SetTexture(cdt_blanktex, 0.0f, 0.0f);
	SetTransform(glm::mat4(1.0f));

}

void SetTexture(CDTTex tex, float offsetX, float offsetY)
{
	glUniform1f(glGetUniformLocation(cdt_programID, "offsetX"), offsetX);
	glUniform1f(glGetUniformLocation(cdt_programID, "offsetY"), offsetY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(cdt_programID, "tex1"), 0);
}

void SetTransform(const glm::mat4 &modelMat)
{
	cdt_MVP = cdt_ProjectionMatrix * cdt_ViewMatrix * modelMat;
	glUniformMatrix4fv(glGetUniformLocation(cdt_programID, "MVP"), 1, GL_FALSE, &cdt_MVP[0][0]);
}
