#pragma once

#include <string>
#include <vector>

#include "Sprite.h"
#include "Chunk.h"
using namespace std;

#define CELL_SIZE 50

static Cordinate AnchorRef[] = { {0,0},{9,0},{9,9},{18,0},{18,9},{18,18}};

class Tiles
{
public:
	static enum TILETYPE
	{
		air,
		block,
		spawn,
		goal,
		spike,
		jumppad,
		doublejump
	};
private:
	TILETYPE type;
	Cordinate levelCord;
public:
	Tiles(TILETYPE t, Cordinate c) :type(t), levelCord(c) {};
	TILETYPE GetType() { return type; }
	Cordinate GetCord() { return levelCord; }
};

class Level
{
private:
	string levelName;
	Cordinate MapSize;
	int** levelGrid;
	vector<Cordinate> anchors;

	void appendAnchors(int);
	void appendSpecialTiles();
	void LoadLevelFromFile(string filename);

public:
	vector <Tiles> specialTiles;

	Level(string filename);
	Level(Level* base, vector<Chunk*> overlay);
	string GetLevelName() { return levelName; }
	Cordinate GetMapSize() { return MapSize; }
	void Draw(Sprite*,bool);
	void Debug();

	int checkCollision(glm::vec3 position);
	glm::vec3 mapToWorld(Cordinate cord, bool focus);
};


