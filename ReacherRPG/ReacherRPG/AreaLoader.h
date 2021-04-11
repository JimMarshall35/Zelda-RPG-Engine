#pragma once
#include <string>
#include "jsonhelper.h"
#include "Background.h"
#include "Area.h"
#include <chrono>
struct TiledProperty {
	std::string   s;
	float         f;
	int           i;
	unsigned int  ui;
};
class AreaLoader
{
public:
	
	~AreaLoader();
	static AreaLoader* Instance() { 
		if (instance == NULL) {
			instance = new AreaLoader();
		}
		return instance; 
	}
	bool loadArea(std::string folder, std::string file, Area& arearef);
private:
	AreaLoader();
	static AreaLoader* instance;
	bool tilesetFileChecks(const rapidjson::Value& val);
	bool layersChecks(const rapidjson::Value& val);
	bool tilesetChecks(const rapidjson::Value& val);
	bool loadTilesets(const rapidjson::Document & doc, Area & arearef, std::string folder);
	bool loadTilesetImgData(TileSet& tileset);
	bool loadLayers(const rapidjson::Document & doc, Area & backgroundref);
	bool getTiledObjectProperty(const rapidjson::Value& props_array, std::string name, TiledProperty& output);
	glm::vec2 tiledPosToGameEnginePos(glm::vec2 tiledpos, const Area & arearef);
};

