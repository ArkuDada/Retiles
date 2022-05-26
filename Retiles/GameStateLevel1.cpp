#include <iostream>
#include <sstream>
#include <vector>

#include "GameStateLevel1.h"
#include "CDT.h"
#include "GameObject.h"
#include "Player.h"
#include "Level.h"

#define GRAVITY -45.0f

static vector<Chunk*> chunks;
static vector<Level*> levels;
static Level* currLevel;

static vector<Sprite*> sprites;
static vector<GameObject*> gameObjects;
static Player* sPlayer;

class ObjectType
{
public:
	static enum TYPE {
		background,
		player,
		tiles,
		spawn,
		goal
	};
};

static bool gamemode = true; // true = play,false = build
static int stage;
// -------------------------------------------
// Game states function
// -------------------------------------------

void GameStateLevel1Load(void)
{
	//load in chunk
	chunks.push_back(new Chunk("Maps/Chunks/Chunk0.txt"));
	chunks.push_back(new Chunk("Maps/Chunks/Chunk1.txt"));
	
	//load in level
	levels.push_back(new Level("Maps/Levels/Level1.txt"));
	levels.push_back(new Level("Maps/Levels/Level2.txt"));
	levels.push_back(new Level("Maps/Levels/Level3.txt"));
	levels.push_back(new Level("Maps/Levels/Level4.txt"));
	levels.push_back(new Level("Maps/Levels/Level5.txt"));
	levels.push_back(new Level("Maps/Levels/Level6.txt"));

	currLevel =levels[5];
	//currLevel = new Level(levels[5], chunks);
	currLevel->Debug();

	//load in sprite
	sprites.push_back(new Sprite("Sprites/bg.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Player.png",4,2));
	sprites.push_back(new Sprite("Sprites/tiles.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Spawn.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Goal.png", 1, 1));

	printf("Level1: Load\n");
}

void GameStateLevel1Init(void)
{
	gamemode = true;
	SetCamPosition(CELL_SIZE * 5, CELL_SIZE * 5);
	SetCamZoom(1.0f);

	//+ Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	GameObject* temp = new GameObject(true, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800.0f * 5, 800.0f * 5, 1.0f), 0.0f, sprites[ObjectType::background]);
	temp->tag = ObjectType::background;
	gameObjects.push_back(temp);

	sPlayer = new Player(true, glm::vec3(CELL_SIZE + CELL_SIZE/2, CELL_SIZE + CELL_SIZE/2, 0.0f), glm::vec3(CELL_SIZE, CELL_SIZE, 0.0f), 0.0f, sprites[ObjectType::player]);
	sPlayer->tag = ObjectType::player;
	gameObjects.push_back(sPlayer);


	printf("Level1: Init\n");
}



void CheckGameMode()
{
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		gamemode = true; //Play mode
		SetCamPosition(CELL_SIZE*5, CELL_SIZE*5);
		SetCamZoom(1.0f);

		sPlayer->isActive = gamemode;
		Cordinate a = { 1, 1 };
		sPlayer->transform.position = currLevel->mapToWorld(a, gamemode);
		
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		gamemode = false;//Build mode
		SetCamPosition(0.0f, 0.0f);
		SetCamZoom(3.5f);

		sPlayer->isActive = gamemode;
		Cordinate a = { 1, 1 };
		sPlayer->transform.position = currLevel->mapToWorld(a, gamemode);
	}
}

void PlatfromModeUpdate(double dt, long frame, int& state)
{
	int playerMoveInput = 0;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { playerMoveInput |= PLAYER_LEFT; }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		playerMoveInput |= PLAYER_RIGHT;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		playerMoveInput |= PLAYER_JUMP;
	}

	sPlayer->PlayerInput(playerMoveInput);
	if (sPlayer->isJumping)
	{
		sPlayer->body.velocity.y += GRAVITY * dt;
	}

	sPlayer->body.UpdatePosByVel(dt);
	sPlayer->PlayerMapCollisionBehavior(currLevel->checkCollision(sPlayer->transform.position));
	sPlayer->UpdateAnimation();
}

void BuildModeUpdate(double dt, long frame, int& state) {}

void GameStateLevel1Update(double dt, long frame, int& state) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------
	CheckGameMode();

	// Cam zoom UI, for Debugging
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		ZoomIn(0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		ZoomOut(0.1f);
	}

	if (gamemode)
	{
		PlatfromModeUpdate(dt, frame, state);
	}
	else
	{
		BuildModeUpdate(dt, frame, state);
	}

	//---------------------------------------------------------
	// Update all game obj position using velocity 
	//---------------------------------------------------------
	


	//-----------------------------------------
	// Update some game obj behavior
	//	- wrap ship around the screen
	//	- destroy bullet that go out of the screen
	//-----------------------------------------
	

	//-----------------------------------------
	// Check for collsion, O(n^2)
	//-----------------------------------------


	//-----------------------------------------
	// Update modelMatrix of all game obj
	//-----------------------------------------
	for (auto gameObject : gameObjects)
	{
		gameObject->transform.UpdateModelMat();
	}


}

void GameStateLevel1Draw(void) {

	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto gameObject: gameObjects)
	{
		if (!gameObject->isActive) continue;

		gameObject->Draw();
	}

	currLevel->Draw(sprites[ObjectType::tiles], gamemode);
	// Swap the buffer, to present the drawing
	glfwSwapBuffers(window);
}

void GameStateLevel1Free(void)
{
	for (auto gameObject : gameObjects)
	{
		delete gameObject;
	}
	gameObjects.clear();
	// reset camera
	ResetCam();

	printf("Level1: Free\n");
}

void GameStateLevel1Unload(void) {

	for (auto ch : chunks)
	{
		delete ch;
	}
	chunks.clear();
	for (auto lvl : levels)
	{
		delete lvl;
	}
	levels.clear();
	for (auto spr : sprites)
	{
		delete spr;
	}
	sprites.clear();

	printf("Level1: Unload\n");
}