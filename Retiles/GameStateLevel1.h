

#ifndef GAME_STATE_LEVEL1
#define GAME_STATE_LEVEL1

#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

//IrrKlang
#include <irrKlang.h>
using namespace irrklang;

// ---------------------------------------------------------------------------

void GameStateLevel1Load(void);
void GameStateLevel1Init(void);
void GameStateLevel1Update(double dt, long frame, int &state);
void GameStateLevel1Draw(void);
void GameStateLevel1Free(void);
void GameStateLevel1Unload(void);

// ---------------------------------------------------------------------------

#endif // GAME_STATE_LEVEL1