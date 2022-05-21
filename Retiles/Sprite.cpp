#include "Sprite.h"
#include <vector>

Sprite::Sprite(string filename, int col, int r):colume(col),row(r)
{
	std::vector<CDTVertex> vertices;
	CDTVertex v1, v2, v3, v4;
	vertices.clear();
	v1.x = -0.5f; v1.y = -0.5f; v1.z = 0.0f; v1.r = 1.0f; v1.g = 0.0f; v1.b = 0.0f; v1.u = 0.0f; v1.v = 0.0f;
	v2.x = 0.5f; v2.y = -0.5f; v2.z = 0.0f; v2.r = 0.0f; v2.g = 1.0f; v2.b = 0.0f; v2.u = 1.0f / (float)colume; v2.v = 0.0f;
	v3.x = 0.5f; v3.y = 0.5f; v3.z = 0.0f; v3.r = 0.0f; v3.g = 0.0f; v3.b = 1.0f; v3.u = 1.0f / (float)colume; v3.v = 1.0f/(float)row;
	v4.x = -0.5f; v4.y = 0.5f; v4.z = 0.0f; v4.r = 1.0f; v4.g = 1.0f; v4.b = 0.0f; v4.u = 0.0f; v4.v = 1.0f / (float)row;
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);

	mesh = CreateMesh(vertices);
	tex = TextureLoad(filename.c_str());
}

Sprite::~Sprite()
{
	UnloadMesh(mesh);
	TextureUnload(tex);
}
