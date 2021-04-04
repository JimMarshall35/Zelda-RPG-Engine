#include "Area.h"



Area::Area()
{

}




void Area::draw(Shader & s, const Camera & camera)
{
	background.draw(s, camera);
}

void Area::freeData()
{
	for (size_t i = 0; i < numtilesets; i++) {
		delete[] tilesets[i].imgdata;
	}
	delete[] tilesets;
	background.freeData();
}
