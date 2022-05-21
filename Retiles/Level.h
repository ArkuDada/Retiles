#pragma once

#include <string>
#include <vector>

#include "Sprite.h"
#include "Chunk.h"
using namespace std;

#define CELL_SIZE 50

static Cordinate AnchorRef[] = { {0,0},{CHUNKSIZE-1,0},{CHUNKSIZE - 1,CHUNKSIZE - 1},{(CHUNKSIZE*2)-1,0},{(CHUNKSIZE*2)-1,CHUNKSIZE - 1},{(CHUNKSIZE*2)-1,(CHUNKSIZE*2)-1}};

class Tiles
{
public:
	static enum TILETYPE
	{
		air,
		block,
		spawn,
		goal
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
	void Draw(Sprite*);
	void Debug();
};


