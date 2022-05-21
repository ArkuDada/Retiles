#pragma once
#include <string>
using namespace std;

/*Layover chunk*/
#define CHUNKSIZE 10

struct Cordinate
{
	int x;
	int y;
};

class Chunk
{
private:
	int** chunkGrid;
public:
	Chunk(string filename);
	int GetTileValue(Cordinate cord);
	void Debug();
};

