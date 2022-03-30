
#include <vector>
#include "GameStateLevel1.h"
#include "CDT.h"
#include "GameObject.h"
#include "TimerSystem.h"


// -------------------------------------------
// Defines
// -------------------------------------------

#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			1024			// The total number of different game object instances
#define PLAYER_INITIAL_NUM			3				// initial number of ship lives
#define NUM_ASTEROID				30
#define SHIP_ACC_FWD				150.0f			// ship forward acceleration (in m/s^2)
#define SHIP_ACC_BWD				-180.0f			// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(0.5f * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 4.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				300.0f			
#define ASTEROID_SPEED				100.0f	
#define MAX_SHIP_VELOCITY			200.0f

#define PLAYER_FIRE_RATE			1.0f							 
#define ENEMY_SPAWN_RATE			2.0f


enum GameObject_TYPE
{
	// list of game object types
	TYPE_SHIP = 0,
	TYPE_BULLET,
	TYPE_ASTEROID,
	TYPE_BACKGROUND,
	TYPE_MISSILE
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
static int			sNumGameObject;

static GameObject* sPlayer;										// Pointer to the Player game object instance

static int			sPlayerLives;									// The number of lives left
static int			sScore;

static TimerSystem playerFireTimer;
static TimerSystem	enemySpawnTimer;

// functions to create/destroy a game object instance
static GameObject* GameObjectInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient);
static void			GameObjectInstDestroy(GameObject* pInst);


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
	// Lazy deletion, not really delete the object, just set it as inactive
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

void spawnAsteroid() {
	float posX = (1000 * (float)rand() / (float)(RAND_MAX)) - (1000.0f / 2.0f);
	float posY = (750 * (float)rand() / (float)(RAND_MAX)) - (750.0f / 2.0f);
	float veloX = ASTEROID_SPEED * (float)rand() / (float)(RAND_MAX);
	float veloY = ASTEROID_SPEED * (float)rand() / (float)(RAND_MAX);
	int randScale = rand() % 30 + 20;
	float radian = 360.0f * ((float)rand() / (float)(RAND_MAX));
	GameObjectInstCreate(TYPE_ASTEROID, glm::vec3(posX, posY, 0.0f), glm::vec3(veloX, veloY, 0.0f), glm::vec3(randScale, randScale, 1.0f), radian);
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
	*pTex = TextureLoad("ship1.png");

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
	*pTex = TextureLoad("asteroid.png");

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
	*pTex = TextureLoad("space_bg1.png");

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
	*pTex = TextureLoad("missile.png");

	printf("Level1: Load\n");
}


void GameStateLevel1Init(void) {

	//+ Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	GameObjectInstCreate(TYPE_BACKGROUND, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1024.0f, 768.0f, 1.0f), 0.0f);

	// Create player game object instance
	//	- the position.z should be set to 0
	//	- the scale.z should be set to 1
	//	- the velocity.z should be set to 0
	sPlayer = GameObjectInstCreate(TYPE_SHIP, glm::vec3(0.0f, -GetWindowHeight() / 4, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f, 50.0f, 1.0f), 0.0f);
	sPlayer->position.z = 0;
	sPlayer->scale.z = 0;
	sPlayer->velocity.z = 0;

	printf("bruh %d \n", sPlayer->type);

	//+ Create all asteroid instance, NUM_ASTEROID, with random pos and velocity
	//	- int a = rand() % 30 + 20;							// a is in the range 20-50
	//	- float b = (float)rand()/(float)(RAND_MAX);		// b is the range 0..1
	for (int i = 0; i < NUM_ASTEROID; i++) // for (1,NUM_ASTEROID] index 1 for BG
	{
		spawnAsteroid();
	}

	//+ reset the score and player life
	sPlayerLives = PLAYER_INITIAL_NUM;
	sScore = 0;

	playerFireTimer = TimerSystem(PLAYER_FIRE_RATE);
	enemySpawnTimer = TimerSystem(ENEMY_SPAWN_RATE);

	printf("Level1: Init\n");
}


void GameStateLevel1Update(double dt, long frame, int& state) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------

	// Moving the Player
	//	- WS accelereate/deaccelerate the ship
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

		// find acceleration vector
		glm::vec3 acc = glm::vec3(SHIP_ACC_FWD * glm::cos(glm::radians(sPlayer->orientation) + PI / 2.0f),
			SHIP_ACC_FWD * glm::sin(glm::radians(sPlayer->orientation) + PI / 2.0f), 0.0f);

		// use acceleration to change velocity
		sPlayer->velocity += acc * glm::vec3(dt, dt, 0.0f);

		//+ velocity cap to MAX_SHIP_VELOCITY
		sPlayer->velocity = glm::clamp(sPlayer->velocity, -MAX_SHIP_VELOCITY, MAX_SHIP_VELOCITY);
		//printf("%f %f %f %f\n", sPlayer->velocity.x, sPlayer->velocity.y, sPlayer->velocity.z, sPlayer->orientation);

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// find acceleration vector
		glm::vec3 acc = glm::vec3(SHIP_ACC_BWD * glm::cos(glm::radians(sPlayer->orientation) + PI / 2.0f),
			SHIP_ACC_BWD * glm::sin(glm::radians(sPlayer->orientation) + PI / 2.0f), 0);

		// use acceleration to change velocity
		sPlayer->velocity += acc * glm::vec3(dt, dt, 0.0f);

		//+ velocity cap to MAX_SHIP_VELOCITY
		sPlayer->velocity = glm::clamp(sPlayer->velocity, -MAX_SHIP_VELOCITY, MAX_SHIP_VELOCITY);
	}

	
	//+ AD: turn the ship
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		sPlayer->orientation -= SHIP_ROT_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		sPlayer->orientation += SHIP_ROT_SPEED;
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
	enemySpawnTimer.Tick(dt);
	if (enemySpawnTimer.TimerEnd())
	{
		spawnAsteroid();
		enemySpawnTimer.TimerReset();
	}

	playerFireTimer.Tick(dt);
	if (playerFireTimer.TimerEnd()) {//+ find the bullet velocity vector
		glm::vec3 vel = glm::vec3(BULLET_SPEED * glm::cos(glm::radians(sPlayer->orientation) + PI / 2.0f),
			BULLET_SPEED * glm::sin(glm::radians(sPlayer->orientation) + PI / 2.0f), 0);
		//+ call GameObjectInstCreate() to create a bullet
		GameObject* missle = GameObjectInstCreate(TYPE_MISSILE, sPlayer->position, vel, glm::vec3(50.0f, 50.0f, 1.0f), sPlayer->orientation);
		//missle->target = tryGetRandomAsteroid();

		playerFireTimer.TimerReset();
	}
	//---------------------------------------------------------
	// Update all game obj position using velocity 
	//---------------------------------------------------------

	for (int i = 0; i < sGameObjectInstList.size(); i++) {
		GameObject* pInst = sGameObjectInstList[i];

		// skip inactive object
		if (pInst == nullptr)
			continue;

		if (pInst->type == TYPE_SHIP) {

			//+ for ship: add some friction to slow it down
			pInst->velocity = pInst->velocity * 0.995f;

			//+ use velocity to update the position
			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);
			SetCamPosition(pInst->position.x, pInst->position.y);
		}
		else if ((pInst->type == TYPE_BULLET) || (pInst->type == TYPE_ASTEROID)) {

			//+ use velocity to update the position
			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);

		}
		else if (pInst->type == TYPE_MISSILE)
		{
			

			pInst->position += pInst->velocity * glm::vec3(dt, dt, 0.0f);
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

		if ((pInst->type == TYPE_SHIP) || (pInst->type == TYPE_ASTEROID)) {

			//+ wrap the ship and asteroid around the screen 
			if (pInst->position.x < -GetWindowWidth() / 2)
			{
				pInst->position.x = GetWindowWidth() / 2;
			}
			else if (pInst->position.x > GetWindowWidth() / 2)
			{
				pInst->position.x = -GetWindowWidth() / 2;
			}

			if (pInst->position.y < -GetWindowHeight() / 2)
			{
				pInst->position.y = GetWindowHeight() / 2;
			}
			else if (pInst->position.y > GetWindowHeight() / 2)
			{
				pInst->position.y = -GetWindowHeight() / 2;
			}

		}
		else if (pInst->type == TYPE_BULLET || pInst->type == TYPE_MISSILE) {

			if (pInst->position.x < -GetWindowWidth() / 2|| pInst->position.x > GetWindowWidth() / 2||pInst->position.y < -GetWindowHeight() / 2|| pInst->position.y > GetWindowHeight() / 2)
			{
				GameObjectInstDestroy(pInst);
			}
			//+ call GameObjectInstDestroy() on bullet that go out of the screen X [-width/2,width/2], Y [-height/2,height/2]

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

		// if pInst1 is an asteroid
		if (pInst1->type == TYPE_ASTEROID) {

			// compare pInst1 with all game obj instances 
			for (int j = 0; j < sGameObjectInstList.size(); j++) {
				GameObject* pInst2 = sGameObjectInstList[j];

				// skip inactive object
				if (pInst2== nullptr)
					continue;

				// skip asteroid object
				if (pInst2->type == TYPE_ASTEROID)
					continue;

				float rightA = pInst1->position.x + pInst1->scale.x / 2;
				float leftA = pInst1->position.x - pInst1->scale.x / 2;
				float topA = pInst1->position.y + pInst1->scale.y / 2;
				float bottomA = pInst1->position.y - pInst1->scale.y / 2;

				float rightB = pInst2->position.x + pInst2->scale.x / 2;
				float leftB = pInst2->position.x - pInst2->scale.x / 2;
				float topB = pInst2->position.y + pInst2->scale.y / 2;
				float bottomB = pInst2->position.y - pInst2->scale.y / 2;

				//+ Check for collsion
				bool collide = !(leftA > rightB || leftB > rightA || bottomA > topB || bottomB > topA);

				if (pInst2->type == TYPE_SHIP) {

					if (collide) {
						//+ Update game behavior and sGameObjectInstArray
						pInst2->position = glm::vec3(0.0f, -GetWindowHeight() / 4, 0.0f);
						sPlayerLives--;
					
						if (sPlayerLives == 0)
						{
							sPlayerLives = PLAYER_INITIAL_NUM;
							
						}
						break;
					}
				}
				else if (pInst2->type == TYPE_BULLET) {

					if (collide) {

						//+ Update game behavior and sGameObjectInstArray
						GameObjectInstDestroy(pInst1);
						GameObjectInstDestroy(pInst2);
						break;
					}
				}
				else if (pInst2->type == TYPE_MISSILE) {
					if (collide) {

						//+ Update game behavior and sGameObjectInstArray
						GameObjectInstDestroy(pInst1);
						GameObjectInstDestroy(pInst2);
						break;
					}

				}
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
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
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
	

		SetRenderMode(CDT_TEXTURE, 1.0f);
		SetTexture(*pInst->tex, 0.0f, 0.0f);
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
	// reset camera
	sGameObjectInstList.clear();
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