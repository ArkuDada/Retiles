#include <vector>
#include "GameStateLevel1.h"
#include "CDT.h"
#include "GameObject.h"
#include "TimerSystem.h"
#include <irrKlang.h>
using namespace irrklang;


// -------------------------------------------
// Defines
// -------------------------------------------

#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			2048			// The total number of different game object instances
#define ENEMY_MAX					60

#define PLAYER_MAX_HP				5.0f			// initial number of ship lives
#define PLAYER_VELOCITY				200.0f
#define PLAYER_ROT_SPEED			(0.5f * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 4.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				300.0f			


#define ENEMY_VELOCITY	100.0f
#define ENEMY_ROT_SPEED	(PI / 4.0f)	


#define INIT_ENEMY_SPAWN_RATE		3.0f	
static float enemySpawnRate;

#define PLAYER_INIT_FA		1
#define PLAYER_INIT_FR		1.0f
static int playerFireAmount;
static float playerFireRate;

#define CLOCKSTART		0

enum GameObject_TYPE
{
	// list of game object types
	TYPE_PLAYER = 0,
	TYPE_HPBAR,
	TYPE_BULLET,
	TYPE_ENEMY,
	TYPE_BACKGROUND,
	TYPE_MISSILE,
	TYPE_NUMBER,
	TYPE_BUFF,
	TYPE_WIN
};

#define FLAG_INACTIVE		0
#define FLAG_ACTIVE			1


// -------------------------------------------
// Structure definitions
// -------------------------------------------



// -------------------------------------------
// Level variable, static - visible only in this file
// -------------------------------------------

static CDTMesh		sMeshArray[MESH_MAX];							// Store all unique shape/mesh in your game
static int			sNumMesh;
static CDTTex		sTexArray[TEXTURE_MAX];							// Corresponding texture of the mesh
static int			sNumTex;
static std::vector<GameObject*>	sGameObjectInstList;			// Store all game object instance

ISoundEngine* SoundEngine;

static int			sNumGameObject;

static GameObject* sPlayer;										// Pointer to the Player game object instance
static GameObject* sTimeCounter[3];

static int			sScore;



static int clockValue;

static TimerSystem seconds;
static TimerSystem playerFireTimer;
static TimerSystem enemySpawnTimer;

static TimerSystem lockonBuff;
static TimerSystem pierceBuff;

// functions to create/destroy a game object instance
static GameObject* GameObjectInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient);
static void			GameObjectInstDestroy(GameObject* pInst);

static float BorderTop;
static float BorderBottom;
static float BorderLeft;
static float BorderRight;
// -------------------------------------------
// Game object instant functions
// -------------------------------------------

GameObject* GameObjectInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient)
{
	// loop through all object instance array to find the free slot

	if (sNumGameObject == GAME_OBJ_INST_MAX)return NULL;

	GameObject* pInst = new GameObject(sMeshArray + type, sTexArray + type, type, FLAG_ACTIVE, pos, vel, scale, orient);
	sGameObjectInstList.push_back(pInst);
	sNumGameObject++;
	return pInst;
}

void GameObjectInstDestroy(GameObject* pInst)
{
	for (int i = 0; i < sGameObjectInstList.size(); i++)
	{
		if (sGameObjectInstList[i] == pInst)
		{
			sGameObjectInstList.erase(sGameObjectInstList.begin()+i);
		}
	}
	delete pInst;
	sNumGameObject--;
}

void spawnBuff(GameObject* enemy)
{
	int random = rand() % 10;
	if (random >= 4)
	{
		return;
	}
	int scale = 50;
	GameObject* buff = GameObjectInstCreate(TYPE_BUFF,enemy->position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(scale, scale, 1.0f), 0.0f);
	buff->value = random;
}

void activateBuff(int buff)
{
	switch (buff)
	{
	case 0:
		playerFireRate -= 0.05f;
		if (playerFireRate <= 0.0f) playerFireRate = 0.05f;
		break;
	case 1:
		playerFireAmount++;
		if (playerFireAmount >= 72)
		{
			playerFireAmount = 72;
		}
		break;
	case 2:
		lockonBuff = TimerSystem(10.0f);
		break;
	case 3:
		pierceBuff = TimerSystem(10.0f);
		break;
	}
}

static int enemySpawnSide = 0;
static int enemyCount = 0;
void spawnEnemy(int enemyType) {
	enemySpawnSide = (enemySpawnSide + 1) % 4;
	
	float posX;
	float posY;
	float ran = (float)rand() / (float)(RAND_MAX);

	switch (enemySpawnSide)
	{
	case 0:
		posX = BorderLeft;
		posY = BorderBottom + (GetWindowHeight()*ran);
		break;
	case 1:
		posX = BorderLeft + (GetWindowWidth() * ran);
		posY = BorderTop;
		break;
	case 2:
		posX = BorderRight;
		posY = BorderBottom + (GetWindowHeight() * ran);
		break;
	case 3:
		posX = BorderLeft + (GetWindowWidth() * ran);
		posY = BorderBottom;
		break;
	}
	int scale =50.f;
	int hp = 1;

	switch (enemyType)
	{
	case 0:
		scale = 50.0f;
		hp = 1;
		break;
	case 1:
		scale = 75.0f;
		hp = 10;
		break;
	}
	GameObject* enemy = GameObjectInstCreate(TYPE_ENEMY, glm::vec3(posX, posY, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(scale, scale, 1.0f), 0.0f);
	enemy->target = sPlayer;
	enemy->health = hp;
	enemyCount++;
}

GameObject* tryGetRandomEnemy()
{
	for (int i = 0; i < sGameObjectInstList.size(); i++)
	{
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;
		if (pInst->type == TYPE_ENEMY)
			return pInst;
	}
	return nullptr;
}

// -------------------------------------------
// Game states function
// -------------------------------------------

void GameStateLevel1Load(void) {

	// clear the Mesh array
	memset(sMeshArray, 0, sizeof(CDTMesh) * MESH_MAX);
	sNumMesh = 0;

	//+ clear the Texture array
	memset(sTexArray, 0, sizeof(CDTTex) * TEXTURE_MAX);
	sNumTex = 0;

	//+ clear the game object instance array
	for (int idx = 0; idx < sGameObjectInstList.size(); idx++)
	{
		sGameObjectInstList[idx] = nullptr;
	}

	// Set the ship object instance to NULL
	sPlayer = NULL;


	// --------------------------------------------------------------------------
	// Create all of the unique meshes/textures and put them in MeshArray/TexArray
	//		- The order of mesh should follow enum GameObject_TYPE 
	/// --------------------------------------------------------------------------

	// Temporary variable for creating mesh
	CDTMesh* pMesh;
	CDTTex* pTex;
	std::vector<CDTVertex> vertices;
	CDTVertex v1, v2, v3, v4;

	// Create Ship mesh/texture
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("Player.png");

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("hpbar.png");

	//+ Create Bullet mesh/texture
	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("bullet.png");

	//+ Create Asteroid mesh/texture
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("Enemy.png");

	//+ Create Background mesh/texture
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("bg.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("bullet.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f / 10.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f / 10.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("numbers.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f / 4.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f / 4.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("powerup.png");

	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f; v3.v = 1.0f;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	pMesh = sMeshArray + sNumMesh++;
	pTex = sTexArray + sNumTex++;
	*pMesh = CreateMesh(vertices);
	*pTex = TextureLoad("youwin.png");

	printf("Level1: Load\n");
}

void GameStateLevel1Init(void) {

	//+ Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	GameObjectInstCreate(TYPE_BACKGROUND, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5000.0f, 5000.0f, 1.0f), 0.0f);

	// Create player game object instance
	//	- the position.z should be set to 0
	//	- the scale.z should be set to 1
	//	- the velocity.z should be set to 0
	sPlayer = GameObjectInstCreate(TYPE_PLAYER, glm::vec3(0.0f, -GetWindowHeight() / 4, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 50.0f, 1.0f), 0.0f);
	sPlayer->position.z = 0;
	sPlayer->scale.z = 0;
	sPlayer->velocity.z = 0;
	sPlayer->health = PLAYER_MAX_HP;
	sPlayer->target = GameObjectInstCreate(TYPE_HPBAR,sPlayer->position,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 10.0f, 1.0f), 0.0f);

	static int playerFireAmount = PLAYER_INIT_FA;
	static float playerFireRate = PLAYER_INIT_FR;


	for (int i = 0; i < 3; i++)
	{
		sTimeCounter[i] = GameObjectInstCreate(TYPE_NUMBER, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(60.0f, 60.0f, 1.0f), 0.0f);
	}

	enemySpawnRate = INIT_ENEMY_SPAWN_RATE;
	//+ reset the score and player life
	sScore = 0;

	clockValue = CLOCKSTART;
	seconds = TimerSystem(1.0f);

	playerFireTimer = TimerSystem(playerFireRate);
	enemySpawnTimer = TimerSystem(enemySpawnRate);
	enemyCount = 0;

	lockonBuff = TimerSystem();
	pierceBuff = TimerSystem();

	SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("bgm.wav",true);

	printf("Level1: Init\n");
}


bool CheckCollision(GameObject* pInst1 , GameObject* pInst2)
{
	float rightA = pInst1->position.x + pInst1->scale.x / 2;
	float leftA = pInst1->position.x - pInst1->scale.x / 2;
	float topA = pInst1->position.y + pInst1->scale.y / 2;
	float bottomA = pInst1->position.y - pInst1->scale.y / 2;

	float rightB = pInst2->position.x + pInst2->scale.x / 2;
	float leftB = pInst2->position.x - pInst2->scale.x / 2;
	float topB = pInst2->position.y + pInst2->scale.y / 2;
	float bottomB = pInst2->position.y - pInst2->scale.y / 2;

	//+ Check for collsion
	return !(leftA > rightB || leftB > rightA || bottomA > topB || bottomB > topA);
}

void GameStateLevel1Update(double dt, long frame, int& state) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------

	// Moving the Player
	//	- WS accelereate/deaccelerate the ship
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS||glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			// use acceleration to change velocity
			sPlayer->velocity = glm::vec3(sPlayer->velocity.x, PLAYER_VELOCITY, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			// use acceleration to change velocity
			sPlayer->velocity = glm::vec3(sPlayer->velocity.x, -PLAYER_VELOCITY, 0.0f);
		}
	}
	else
	{
		sPlayer->velocity = glm::vec3(sPlayer->velocity.x, 0.0f, 0.0f);
	}
	
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS|| glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			sPlayer->velocity = glm::vec3(PLAYER_VELOCITY, sPlayer->velocity.y,  0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			sPlayer->velocity = glm::vec3(-PLAYER_VELOCITY, sPlayer->velocity.y, 0.0f);
		}
	}
	else
	{
		sPlayer->velocity = glm::vec3(0.0f, sPlayer->velocity.y, 0.0f);
	}
	
	
	//+ QE: turn the ship
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		sPlayer->orientation += PLAYER_ROT_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		sPlayer->orientation -= PLAYER_ROT_SPEED;
	}

	

	// Fire bullet/missile using JK
	//	- create the bullet at the ship's position
	//	- bullet direction is the same as the ship's orientation
	//	- may use if(frame % n == 0) too slow down the bullet creation
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {

		//+ find the bullet velocity vector
		glm::vec3 vel = glm::vec3(BULLET_SPEED * glm::cos(glm::radians(sPlayer->orientation) + PI / 2.0f),
			BULLET_SPEED * glm::sin(glm::radians(sPlayer->orientation) + PI / 2.0f), 0);
		//+ call GameObjectInstCreate() to create a bullet
		GameObjectInstCreate(TYPE_BULLET, sPlayer->position, vel, glm::vec3(15.0f, 15.0f, 1.0f), sPlayer->orientation);

	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		
	}

	// Cam zoom UI, for Debugging
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		ZoomIn(0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		ZoomOut(0.1f);
	}


	//---------------------------------------------------------
	// Update all timer
	//---------------------------------------------------------
	seconds.Tick(dt);
	if (seconds.TimerEnd())
	{
		clockValue++;
		seconds.TimerReset();

		enemySpawnRate = INIT_ENEMY_SPAWN_RATE - (0.1f * (clockValue/10));
		enemySpawnRate = enemySpawnRate >= 0.1f ? enemySpawnRate : 0.1f;

		switch (clockValue)
		{
		case 60:
			for (int i = 0; i < 8; i++)
			{
				spawnEnemy(0);
			}
			break;
		case 120:
			for (int i = 0; i < 8; i++)
			{
				spawnEnemy(0);
			}
			spawnEnemy(1);
			break;
		case 180:
			for (int i = 0; i < 16; i++)
			{
				spawnEnemy(0);
			}
			for (int i = 0; i < 2; i++)
			{
				spawnEnemy(1);
			}
			break;
		case 240:
			for (int i = 0; i < 16; i++)
			{
				spawnEnemy(0);
			}
			for (int i = 0; i < 4; i++)
			{
				spawnEnemy(1);
			}
			break;
		case 300:
			for (int i = 0; i < 16; i++)
			{
				spawnEnemy(0);
			}
			for (int i = 0; i < 8; i++)
			{
				spawnEnemy(1);
			}
			enemySpawnTimer.SetActive(false);
			break;
		}
		if (clockValue >= 300 && enemyCount == 0)
		{
			enemySpawnTimer.SetActive(false);
			seconds.SetActive(false);
			GameObjectInstCreate(TYPE_WIN, sPlayer->position,glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(700.0f, 100.0f, 1.0f), 0.0f);
			printf("You Win!\n");
		}
	}

	enemySpawnTimer.Tick(dt);
	if (enemySpawnTimer.TimerEnd())
	{
		spawnEnemy(0);
		enemySpawnTimer.TimerReset(enemySpawnRate);
	}

	playerFireTimer.Tick(dt);
	if (playerFireTimer.TimerEnd()) {//+ find the bullet velocity vector
		
		//+ call GameObjectInstCreate() to create a bullet
		float const baseAngle = sPlayer->orientation;
		for (int i = 0; i < playerFireAmount; i++)
		{
			float angle = baseAngle + i * 45 + ((i / 8) * 5);
			glm::vec3 vel = glm::vec3(BULLET_SPEED * glm::cos(glm::radians(angle) + PI / 2.0f),
				BULLET_SPEED * glm::sin(glm::radians(angle) + PI / 2.0f), 0);
			GameObject* missle = GameObjectInstCreate(TYPE_MISSILE, sPlayer->position, vel, glm::vec3(50.0f, 50.0f, 1.0f), angle);
		}
		playerFireTimer.TimerReset();
	}

	lockonBuff.Tick(dt);
	if (lockonBuff.TimerEnd())
	{
		lockonBuff.SetActive(false);
	}
	

	pierceBuff.Tick(dt);
	if (pierceBuff.TimerEnd())
	{
		pierceBuff.SetActive(false);
	}

	//---------------------------------------------------------
	// Update all game obj position using velocity 
	//---------------------------------------------------------

	int numLoop = 0;
	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst == nullptr)
			continue;

		if (pInst->type == TYPE_PLAYER) {

			//+ for ship: add some friction to slow it down
			pInst->velocity = pInst->velocity * 0.995f;

			//+ use velocity to update the position
			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);

			SetCamPosition(pInst->position.x, pInst->position.y);

			BorderTop = pInst->position.y + GetWindowHeight() / 2;
			BorderBottom = pInst->position.y - GetWindowHeight() / 2;
			BorderRight = pInst->position.x + GetWindowWidth() / 2;
			BorderLeft = pInst->position.x - GetWindowWidth() / 2;

			pInst->target->position = glm::vec3(pInst->position.x, pInst->position.y-45.0f, pInst->position.z);
			pInst->target->scale = glm::vec3(100.0f * (pInst->health/PLAYER_MAX_HP) , 10.0f, 1.0f);
			
		}
		else if (pInst->type == TYPE_ENEMY) {

			if (pInst->target != nullptr)
			{
				float cross = glm::cross(pInst->velocity,(pInst->position - pInst->target->position)).z;
				int direction = cross > 0 ? -1 : 1;
				pInst->orientation += direction * ENEMY_ROT_SPEED;

				pInst->velocity = glm::vec3(ENEMY_VELOCITY * glm::cos(glm::radians(pInst->orientation) + PI / 2.0f),
					ENEMY_VELOCITY * glm::sin(glm::radians(pInst->orientation) + PI / 2.0f), 0);
			}
			else
			{
				pInst->target = sPlayer;
			}

			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);

		}
		else if (pInst->type == TYPE_BULLET|| pInst->type == TYPE_MISSILE)
		{

			if (lockonBuff.GetActive())
			{
				if (pInst->target != nullptr)
				{
					int direction = glm::cross(pInst->position, pInst->target->position).z > 0 ? -1 : 1;
					pInst->orientation += direction * HOMING_MISSILE_ROT_SPEED;

					pInst->velocity = glm::vec3(BULLET_SPEED * glm::cos(glm::radians(pInst->orientation) + PI / 2.0f),
						BULLET_SPEED * glm::sin(glm::radians(pInst->orientation) + PI / 2.0f), 0);
				}
				else
				{
					pInst->target = tryGetRandomEnemy();
				}
			}

			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);
		}
		else if (pInst->type == TYPE_NUMBER)
		{
			glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
			float center = BorderLeft + GetWindowWidth() / 2;
			int value;
			if (numLoop == 0)
			{
				offset = glm::vec3(center - 55.0f, BorderTop - 50.0f, 0.0f);
				value = (clockValue / 100) % 10;
			}else if (numLoop == 1)
			{
				offset = glm::vec3(center, BorderTop - 50.0f, 0.0f);
				value = (clockValue / 10) % 10;
			}
			else if (numLoop == 2)
			{
				offset = glm::vec3(center + 55.0f, BorderTop - 50.0f, 0.0f);
				value = clockValue % 10;
			}
			pInst->position = offset;
			pInst->value = value;
			numLoop++;
		}
		else if (pInst->type == TYPE_WIN)
		{
			pInst->position = glm::vec3(BorderLeft + GetWindowWidth() / 2, BorderBottom + 50.0f, 0.0f);
		}
	}


	//-----------------------------------------
	// Update some game obj behavior
	//	- wrap ship around the screen
	//	- destroy bullet that go out of the screen
	//-----------------------------------------

	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst == nullptr)
			continue;

		if (pInst->type == TYPE_BULLET || pInst->type == TYPE_MISSILE || pInst->type == TYPE_BUFF) {

			if (pInst->position.x > BorderRight|| pInst->position.x < BorderLeft|| pInst->position.y > BorderTop || pInst->position.y < BorderBottom)
			{
				GameObjectInstDestroy(pInst);
			}

		}
	}

	//-----------------------------------------
	// Check for collsion, O(n^2)
	//-----------------------------------------

	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst1 = sGameObjectInstList[i];

		// skip inactive object
		if (pInst1== nullptr)
			continue;

		// if pInst1 is an enemy
		if (pInst1->type == TYPE_ENEMY) {

			// compare pInst1 with all game obj instances 
			for (int j = 0; j < sGameObjectInstList.size(); j++) {
				GameObject* pInst2 = sGameObjectInstList[j];

				// skip inactive object
				if (pInst2== nullptr)
					continue;

				// skip asteroid object
				if (pInst2->type == TYPE_ENEMY)
					continue;

				//+ Check for collsion
				bool collide = CheckCollision(pInst1, pInst2);

				if (pInst2->type == TYPE_PLAYER) {

					if (collide) {
						//+ Update game behavior and sGameObjectInstArray
						pInst2->position += pInst1->velocity * 0.5f;
						sPlayer->health--;
					
						if (sPlayer->health == 0)
						{
							GameStateLevel1Free();
							GameStateLevel1Init();
						}
						break;
					}
				}
				else if (pInst2->type == TYPE_BULLET || pInst2->type == TYPE_MISSILE) {

					if (collide) {

						//+ Update game behavior and sGameObjectInstArray
						if (!pierceBuff.GetActive())
						{
							GameObjectInstDestroy(pInst2);
						}

						pInst1->health--;
						if (pInst1->health <= 0)
						{
							enemyCount--;
							spawnBuff(pInst1);
							GameObjectInstDestroy(pInst1);
						}
						break;
					}
				}
				
			}
		}

		if (pInst1->type == TYPE_BUFF)
		{
			bool collide = CheckCollision(pInst1, sPlayer);
			if (collide)
			{
				activateBuff(pInst1->value);
				GameObjectInstDestroy(pInst1);
			}
		}
	}

	//-----------------------------------------
	// Update modelMatrix of all game obj
	//-----------------------------------------

	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst == nullptr)
			continue;

		glm::mat4 rMat = glm::mat4(1.0f);
		glm::mat4 sMat = glm::mat4(1.0f);
		glm::mat4 tMat = glm::mat4(1.0f);

		// Compute the scaling matrix
		sMat = glm::scale(glm::mat4(1.0f), pInst->scale);

		//+ Compute the rotation matrix, we should rotate around z axis 
		rMat = glm::rotate(glm::mat4(1.0f), glm::radians(pInst->orientation), glm::vec3(0.0f, 0.0f, 1.0f));

		//+ Compute the translation matrix
		tMat = glm::translate(glm::mat4(1.0f), pInst->position);

		// Concatenate the 3 matrix to from Model Matrix
		pInst->modelMatrix = tMat * sMat * rMat;
	}

	//printf("Life> %i\n", sPlayerLives);
	//printf("Score> %i\n", sScore);
}

void GameStateLevel1Draw(void) {

	// Clear the screen
	glClearColor(127/256.0f, 112 / 256.0f, 138 / 256.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all game object instance in the sGameObjectInstArray
	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];
		// skip inactive object
		if (pInst == nullptr)
			continue;

		// 4 steps to draw sprites on the screen
		//	1. SetRenderMode()
		//	2. SetTexture()
		//	3. SetTransform()
		//	4. DrawMesh()
		
		if (pInst->type == TYPE_NUMBER)
		{
			continue;
		}

		SetRenderMode(CDT_TEXTURE, 1.0f);

		SetTexture(*pInst->tex, 0.0f, 0.0f);
		if (pInst->type == TYPE_BUFF)
		{
			SetTexture(*pInst->tex, pInst->value / 4.0f, 0.0f);
		}

		
		SetTransform(pInst->modelMatrix);
		DrawMesh(*pInst->mesh);
	}

	for (int i = 0; i < 3; i++)
	{
		GameObject* pInst = sTimeCounter[i];
		SetRenderMode(CDT_TEXTURE, 1.0f);
		SetTexture(*pInst->tex, pInst->value / 10.0f, 0.0f);
		SetTransform(pInst->modelMatrix);
		DrawMesh(*pInst->mesh);

	}
	// Swap the buffer, to present the drawing
	glfwSwapBuffers(window);
}

void GameStateLevel1Free(void) {

	//+ call GameObjectInstDestroy for all object instances in the sGameObjectInstArray
	
	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst == nullptr)
			continue;

		GameObjectInstDestroy(pInst);
	}
	sGameObjectInstList.clear();

	//SoundEngine->drop();

	// reset camera
	ResetCam();

	printf("Level1: Free\n");
}

void GameStateLevel1Unload(void) {

	// Unload all meshes in MeshArray
	for (int i = 0; i < sNumMesh; i++) {
		UnloadMesh(sMeshArray[i]);
	}

	//+ Unload all textures in TexArray


	printf("Level1: Unload\n");
}