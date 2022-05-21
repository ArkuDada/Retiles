#include "Chunk.h"
#include <fstream>

Chunk::Chunk(string filename)
{
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		chunkGrid = new int* [CHUNKSIZE];

		for (int y = 0; y < CHUNKSIZE; y++) {

			chunkGrid[y] = new int[CHUNKSIZE];

			for (int x = 0; x < CHUNKSIZE; x++) {
				myfile >> chunkGrid[y][x];
			}
		}
		myfile.close();
		Debug();
	}
	
}

int Chunk::GetTileValue(Cordinate cord)
{
	return chunkGrid[(CHUNKSIZE - 1) - cord.y][cord.x];
}

void Chunk::Debug()
{
	for (int y = 0; y < CHUNKSIZE; y++)
	{
		for (int x = 0; x < CHUNKSIZE; x++)
		{
			printf("%d", chunkGrid[CHUNKSIZE - 1 - y][x]);
		}
		printf("\n");
	}
}
