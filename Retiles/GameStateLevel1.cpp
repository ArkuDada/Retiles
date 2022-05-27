#include <iostream>
#include <sstream>
#include <vector>
#include <irrKlang.h>

#include "GameStateLevel1.h"
#include "CDT.h"
#include "GameObject.h"
#include "Player.h"
#include "Level.h"
#include "Camera.h"

#define STARTLEVEL 0

#define GRAVITY -45.0f

static vector<Chunk*> chunks;
static vector<Level*> levels;
static Level* currLevel;

static vector<Sprite*> sprites;
static vector<GameObject*> gameObjects;
static vector<GameObject*> uiObjects;

static GameObject* bgObject;
static Player* sPlayer;
static GameObject* currSpawn;

static Camera* cam;

static vector<int> selectedChunk;
static bool haveSpawn;
static bool win;

static irrklang::ISoundEngine* audioEngine;
static  irrklang::ISound* playerWalk;

class ObjectType
{
public:
	static enum TYPE {
		background,
		player,
		chunkui,
		tiles,
		spawn,
		goal,
		spike,
		jumppad,
		doublejump,
		win
	};
};

static bool gamemode = true; // true = play,false = build
static int stage;
// -------------------------------------------
// Game states function
// -------------------------------------------

void ResetUI()
{
	for (int i = 0; i < uiObjects.size(); i++)
	{
		uiObjects[i]->isActive = i <= stage;
	}
}

void BuildLevel()
{
	delete currLevel;
	//+ clear gameObject;
	haveSpawn = false;

	for (int i = gameObjects.size() - 1; i > 0; i -= 1)
	{
		GameObject* obj = gameObjects[i];
		if (obj->tag > ObjectType::player)
		{
			delete obj;
		}
	}
	gameObjects.clear();
	
	vector<Chunk*> chunkList;
	for (int i = 0; i <selectedChunk.size(); i++)
	{
		chunkList.push_back(chunks[selectedChunk[i]]);
	}

	currLevel = new Level(levels[stage], chunkList);
	
	gameObjects.push_back(bgObject); // bg go first;

	//special obejects
	for (int i = 0;i < currLevel->specialTiles.size(); i++)
	{
		Tiles t = currLevel->specialTiles[i];
		GameObject* temp = nullptr;
		glm::vec3 pos = currLevel->mapToWorld(t.GetCord(), gamemode);
		glm::vec3 scale = glm::vec3(CELL_SIZE, CELL_SIZE, 0.0f);
		switch (t.GetType())
		{
		case Tiles::spawn:
			cout << "Add Spawn" << endl;
			currSpawn = new GameObject(true,pos ,scale , 0.0f, sprites[ObjectType::spawn]);
			currSpawn->tag = ObjectType::spawn;
			temp = currSpawn;

			haveSpawn = true;

			break;
		case Tiles::goal:
			cout << "Add Goal" << endl;
			temp = new GameObject(true, pos, scale, 0.0f, sprites[ObjectType::goal]);
			temp->tag = ObjectType::goal;
			break;
		case Tiles::spike:
			cout << "Add spike" << endl;
			temp = new GameObject(true, pos, scale, 0.0f, sprites[ObjectType::spike]);
			temp->tag = ObjectType::spike;
			break;
		case Tiles::jumppad:
			cout << "Add jumppad" << endl;
			temp = new GameObject(true, pos, scale, 0.0f, sprites[ObjectType::jumppad]);
			temp->tag = ObjectType::jumppad;
			temp->body.velocity = glm::vec3(0.0f, GRAVITY * 5, 0.0f);
			break;
		case Tiles::doublejump:
			cout << "Add doublejump" << endl;
			temp = new GameObject(true, pos, scale, 0.0f, sprites[ObjectType::doublejump]);
			temp->tag = ObjectType::doublejump;
			break;
		default:
			break;
		}
		if (temp != nullptr) gameObjects.push_back(temp);
	}

	// add player
	gameObjects.push_back(sPlayer);
	sPlayer->body.velocity = vec3(0, 0, 0);
	sPlayer->canDoubleJump = false;
	
	if(cam != nullptr) cam->SetCameraLimit(currLevel->GetMapSize());
}

void GameStateLevel1Load(void)
{
	//load in chunk
	chunks.push_back(new Chunk("Maps/Chunks/Chunk0.txt"));//debug
	chunks.push_back(new Chunk("Maps/Chunks/Chunk1.txt"));//1
	chunks.push_back(new Chunk("Maps/Chunks/Chunk2.txt"));//2
	chunks.push_back(new Chunk("Maps/Chunks/Chunk3.txt"));//3
	chunks.push_back(new Chunk("Maps/Chunks/Chunk4.txt"));//4
	chunks.push_back(new Chunk("Maps/Chunks/Chunk5.txt"));//5
	chunks.push_back(new Chunk("Maps/Chunks/Chunk6.txt"));//6
	chunks.push_back(new Chunk("Maps/Chunks/Chunk7.txt"));//7
	chunks.push_back(new Chunk("Maps/Chunks/Chunk8.txt"));//8
	
	//load in level
	levels.push_back(new Level("Maps/Levels/Level1.txt"));
	levels.push_back(new Level("Maps/Levels/Level2.txt"));
	levels.push_back(new Level("Maps/Levels/Level3.txt"));
	levels.push_back(new Level("Maps/Levels/Level4.txt"));
	levels.push_back(new Level("Maps/Levels/Level5.txt"));
	levels.push_back(new Level("Maps/Levels/Level6.txt"));
	levels.push_back(new Level("Maps/Levels/Level7.txt"));
	levels.push_back(new Level("Maps/Levels/Level8.txt"));

	//load in sprite
	sprites.push_back(new Sprite("Sprites/bg.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Player.png",4,4));
	sprites.push_back(new Sprite("Sprites/ChunkSpriteSheet.png",8,1));
	sprites.push_back(new Sprite("Sprites/tiles.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Spawn.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Goal.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/Spike.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/JumpPad.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/DoubleJump.png", 1, 1));
	sprites.push_back(new Sprite("Sprites/youwin.png", 1, 1));

	stage = STARTLEVEL;
	
	audioEngine = irrklang::createIrrKlangDevice();
	audioEngine->setSoundVolume(0.25f);
	audioEngine->play2D("Audio/bgm1.wav", true);
	playerWalk = audioEngine->play2D("Audio/walk2.wav", true, true, true);
	playerWalk->setVolume(10.0f);
	
	win = false;

	printf("Level1: Load\n");
}

void GameStateLevel1Init(void)
{

	gamemode = true;
	SetCamPosition(CELL_SIZE * 5, CELL_SIZE * 5);
	SetCamZoom(1.0f);

	//+ Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	bgObject = new GameObject(true, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800.0f * 5, 800.0f * 5, 1.0f), 0.0f, sprites[ObjectType::background]);
	bgObject->tag = ObjectType::background;
	
	sPlayer = new Player(true, glm::vec3(CELL_SIZE + CELL_SIZE/2, CELL_SIZE + CELL_SIZE/2, 0.0f), glm::vec3(CELL_SIZE, CELL_SIZE, 0.0f), 0.0f, sprites[ObjectType::player]);
	sPlayer->tag = ObjectType::player;

	BuildLevel();
	currLevel->Debug();

	sPlayer->isActive = gamemode;
	sPlayer->transform.position = currSpawn->transform.position;
	sPlayer->isFalling = true;

	cam = new Camera(&sPlayer->transform);
	cam->SetCameraLimit(currLevel->GetMapSize());

	AnimatedObject* tempUI;
	for (int i = 0; i < 8; i++)
	{
		tempUI = new AnimatedObject(true, glm::vec3(-1200.0f + (350.0f*i), -1000.0f, 0.0f), glm::vec3(300.0f, 300.0f, 1.0f), 0.0f, sprites[ObjectType::chunkui]);
		tempUI->offsetX = (float)i / 8;
		uiObjects.push_back(tempUI);
	}

	ResetUI();

	printf("Level1: Init\n");
}

void LoadNewLevel()
{
	SetCamPosition(CELL_SIZE * 5, CELL_SIZE * 5);
	SetCamZoom(1.0f);

	BuildLevel();

	sPlayer->isActive = gamemode;
	sPlayer->transform.position = currSpawn->transform.position;
	sPlayer->isFalling = true;

	if (win)
	{
		gamemode = false;//Build mode
		SetCamPosition(0.0f, 0.0f);
		SetCamZoom(3.5f);

		BuildLevel();

		sPlayer->isActive = gamemode;

		for (GameObject* obj: gameObjects)
		{
			if (obj->tag != ObjectType::background)
			{
			obj->isActive = false;

			}
		}
		gameObjects.push_back(new GameObject(true, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(700.0f * 4, 100.0f * 4, 10.0f), 0.0f, sprites[ObjectType::win]));
	}

}

void CheckGameMode()
{
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && haveSpawn)
	{
		gamemode = true; //Play mode
		LoadNewLevel();
		
		
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		gamemode = false;//Build mode
		SetCamPosition(0.0f, 0.0f);
		SetCamZoom(3.5f);

		BuildLevel();

		sPlayer->isActive = gamemode;
	}
}

void PlatformModeUpdate(double dt, long frame, int& state)
{
	int playerMoveInput = 0;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		playerMoveInput |= PLAYER_LEFT;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		playerMoveInput |= PLAYER_RIGHT;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		playerMoveInput |= PLAYER_JUMP;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
	{
		playerMoveInput |= PLAYER_STOP_JUMP;
	}

	sPlayer->PlayerInput(playerMoveInput);
	if (sPlayer->isFalling)
	{
		sPlayer->body.velocity.y += GRAVITY * dt;
	}

	sPlayer->body.UpdatePosByVel(dt);
	sPlayer->PlayerMapCollisionBehavior(currLevel->checkCollision(sPlayer->transform.position));
	sPlayer->UpdateAnimation();

	cam->UpdateCamera();

	playerWalk->setIsPaused(!(sPlayer->body.velocity.x != 0 && !sPlayer->isFalling));

	for (GameObject* obj : gameObjects)
	{
		if (obj->tag == ObjectType::jumppad && obj->isActive)
		{
			obj->body.UpdatePosByVel(dt);
			if (currLevel->checkCollision(obj->transform.position) & COLLISION_BOTTOM)
			{
				obj->transform.position.y = (int)obj->transform.position.y + 0.75f;
				obj->body.velocity.y = 0.0f;
			}

			for (GameObject* obj2 : gameObjects)
			{
				if (obj2->tag == ObjectType::spike)
				{
					if (obj->transform.checkCollision(obj2->transform))
					{
						obj->isActive = false;
					}
				}
			}
		}
	}

	//collision check with other objects
	sPlayer->superJump = false;

	for (GameObject* obj: gameObjects)
	{
		bool escape = false;
		if (!obj->isActive)continue;
	
		if (obj->tag >= ObjectType::goal)
		{
			bool collide = sPlayer->transform.checkCollision(obj->transform);
			if (collide)
			{
				switch (obj->tag)
				{
				case ObjectType::goal:
					stage++;
					if (stage == levels.size()-1)
					{
						win = true;
					}
					selectedChunk.clear();
					ResetUI();
					LoadNewLevel();
					escape = true;
					break;
				case ObjectType::spike:
					LoadNewLevel();
					break;
				case ObjectType::jumppad:
					sPlayer->superJump = true;
					break;
				case ObjectType::doublejump:
					sPlayer->canDoubleJump = true;
					break;
				default:
					break;
				}
			}
		}

		if (escape)break;
	}
}

void AddChunk(int input)
{
	if (input == -1)
	{
		selectedChunk.clear();
		ResetUI();
		return;
	}

	//check for dups
	for (int i : selectedChunk)
	{
		if (i == input) return;
	}

	
	if (uiObjects[input - 1]->isActive)
	{
	selectedChunk.push_back(input);
	uiObjects[input - 1]->isActive = false;
	}
}

void BuildModeUpdate(double dt, long frame, int& state)
{
	int input = 0;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) input = -1;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) input = 1;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) input = 2;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) input = 3;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) input = 4;

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) input = 5;

	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) input = 6;

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) input = 7;

	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) input = 8;

	if (input != 0)
	{
		cout << "Player Build: " << input << endl;
		AddChunk(input);
		BuildLevel();
	}
}

void GameStateLevel1Update(double dt, long frame, int& state) {

	if (win)
	{
		return;
	}
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
		PlatformModeUpdate(dt, frame, state);
	}
	else
	{
		BuildModeUpdate(dt, frame, state);
	}

	//-----------------------------------------
	for (auto gameObject : gameObjects)
	{
		if (gameObject->isActive)
		{
		gameObject->transform.UpdateModelMat();
		}
	}

	for (auto ui : uiObjects)
	{
		if (ui->isActive)
		{
			ui->transform.UpdateModelMat();
		}
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

	// Swap the buffer, to present the drawing
	if (!win)
	{
		currLevel->Draw(sprites[ObjectType::tiles], gamemode);

		for (auto ui : uiObjects)
		{
			if (ui->isActive)
			{
				ui->Draw();
			}
		}
	}

	glfwSwapBuffers(window);
}

void GameStateLevel1Free(void)
{
	for (auto gameObject : gameObjects)
	{
		delete gameObject;
	}
	gameObjects.clear();
	for (auto ui : uiObjects)
	{
		delete ui;
	}
	uiObjects.clear();
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

	delete cam;

	playerWalk->drop();
	audioEngine->drop();

	printf("Level1: Unload\n");
}