#include "Level.h"
#include "Collision.h"


#include <iostream>
#include <fstream>

void Level::appendSpecialTiles()
{
	specialTiles.clear();
	for (int y = 0; y < MapSize.y; y++) {
		for (int x = 0; x < MapSize.x; x++) {
			if (levelGrid[y][x] > Tiles::block)
			{
				Cordinate temp_cord = { x,  MapSize. y- 1 -y };
				Tiles speTile((Tiles::TILETYPE)levelGrid[y][x], temp_cord);
				specialTiles.push_back(speTile);
			}
		}
	}
}

void Level::appendAnchors(int c)
{
	for(int i = 0;i < c &&i < 6;i++ )
	{
		Cordinate a = AnchorRef[i];
		
		anchors.insert(anchors.end(),a);
	}
	printf("{%d,%d} anochor size %d\n", MapSize.x, MapSize.y, anchors.size());
}

void Level::LoadLevelFromFile(string filename)
{
	levelName = filename;
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		myfile >> MapSize.y;
		myfile >> MapSize.x;
		int size;
		myfile >> size;
		levelGrid = new int* [MapSize.y];

		for (int y = 0; y < MapSize.y; y++) {
			levelGrid[y] = new int[MapSize.x];
			for (int x = 0; x < MapSize.x; x++) {
				myfile >> levelGrid[y][x];
			}
		}
		myfile.close();
		appendAnchors(size);
	}
}

Level::Level(string filename)
{
	LoadLevelFromFile(filename);
	appendSpecialTiles();
	Debug();
}

Level::Level(Level* ori, vector<Chunk*> overlay)
{
	LoadLevelFromFile(ori->levelName);

	for (int i = 0; i < anchors.size() && i < overlay.size(); i++)
	{
		for (int y = 0; y < CHUNKSIZE; y++)
		{
			for (int x = 0; x < CHUNKSIZE; x++)
			{
				int mapYPos = (MapSize.y - 1) - anchors[i].y - y;
				int mapXPos = anchors[i].x + x;

				Cordinate cordToGet = { x,y };
				int tileValue = overlay[i]->GetTileValue(cordToGet);
				if (tileValue != 0)
				{
					levelGrid[mapYPos][mapXPos] = tileValue;
				}

			}
		}
	}
	appendSpecialTiles();
	Debug();
}

void Level::Draw(Sprite* sprite,bool focus)
{
	glm::mat4 tMat = focus?glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, 0.0f)): glm::translate(glm::mat4(1.0f), glm::vec3(-MapSize.x / 2.0f, -MapSize.y / 2.0f, 0.0f));
	glm::mat4 sMat = glm::scale(glm::mat4(1.0f), glm::vec3(CELL_SIZE, CELL_SIZE, 1.0f));
	glm::mat4 sMapMatrix = sMat * tMat;
	glm::mat4 matTransform;
	glm::mat4 cellMatrix;

	for (int y = 0; y < MapSize.y; y++)
	{
		for (int x = 0; x < MapSize.x; x++)
		{
			int tileValue = levelGrid[y][x];
			if (tileValue == Tiles::block)
			{
				//+ Find transformation matrix of each cell, cellMatrix is just a translation matrix
				cellMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x + 0.5f, (MapSize.y - y) - 0.5f, 0.0f));

				// Transform cell from map space [0,MAP_SIZE] to screen space [-width/2,width/2]
				matTransform = sMapMatrix * cellMatrix;

				// Render each cell
				SetRenderMode(CDT_TEXTURE, 1.0f);
				SetTexture(sprite->GetTexture(), 0.0f, 0.0f);
				SetTransform(matTransform);

				CDTMesh mesh = sprite->GetMesh();
				DrawMesh(mesh);
			}
		}

	}
}

glm::vec3 Level::mapToWorld(Cordinate cord, bool focus)
{
	glm::vec3 result = glm::vec3(cord.x * CELL_SIZE + CELL_SIZE/2, cord.y* CELL_SIZE + CELL_SIZE/2, 0.0f);

	if (!focus) result += glm::vec3(-MapSize.x * CELL_SIZE / 2.0f, -MapSize.y * CELL_SIZE / 2.0f, 0.0f);
	return result;
}

int Level::checkCollision(glm::vec3 position)
{
	float PosX = (position.x- CELL_SIZE / 2)/CELL_SIZE + 0.5f;
	float PosY = (position.y - CELL_SIZE / 2) / CELL_SIZE + 0.5f;
	//std::cout << PosX << "," << PosY << endl;
	int result = 0;

	// Check Left Collision
	try
	{
		
		if (levelGrid[MapSize.y - 1 - (int)(PosY + 0.25f)][(int)(PosX - 0.5f)] == 1|| levelGrid[MapSize.y - 1 - (int)(PosY - 0.25f)][(int)(PosX - 0.5f)] == 1 )
		{
			result |= COLLISION_LEFT;
		}
		
		// Check Right Collision
		if (levelGrid[MapSize.y - 1 - (int)(PosY + 0.25f)][(int)(PosX + 0.5f)] == 1 || levelGrid[MapSize.y - 1 - (int)(PosY - 0.25f)][(int)(PosX + 0.5f)] == 1)
		{
			result |= COLLISION_RIGHT;
		}

		// Check Top Collision
		if (levelGrid[MapSize.y - 1 - (int)(PosY + 0.5f)][(int)(PosX - 0.25f)] == 1|| levelGrid[MapSize.y - 1 - (int)(PosY + 0.5f)][(int)(PosX + 0.25f)] == 1 )
		{
			result |= COLLISION_TOP;
		}

		// Check Bottom Collision (Special : Y Need to be lower)
		if (levelGrid[MapSize.y - 1 - (int)(PosY - 0.5f)][(int)(PosX - 0.25f)] == 1|| levelGrid[MapSize.y - 1 - (int)(PosY - 0.5f)][(int)(PosX + 0.25f)] == 1 )
		{
			result |= COLLISION_BOTTOM;
		}
	}
	catch (const std::exception&)
	{

	}

	return result;
}

void Level::Debug()
{
	printf("Debug of %s\n",levelName.c_str());
	for (int y = 0; y < MapSize.y; y++)
	{
		for (int x = 0; x < MapSize.x; x++)
		{
			printf("%d",levelGrid[y][x]);
		}
		printf("\n");
	}
	for (int i = 0; i < specialTiles.size(); i++)
	{
		printf("%d : {%d,%d}\n", specialTiles[i].GetType(), specialTiles[i].GetCord().x, specialTiles[i].GetCord().y);
	}
}
