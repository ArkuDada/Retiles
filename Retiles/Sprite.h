#pragma once
#include "CDT.h"
#include <string>

using namespace std;

class Sprite
{
protected:
	CDTMesh mesh;
	CDTTex tex;
	int colume;
	int row;
public:
	Sprite(string filename,int colume,int row);
	CDTTex GetTexture() { return tex; }
	CDTMesh GetMesh() { return mesh; }

	~Sprite();
};

