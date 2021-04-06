#pragma once
#include <string>
#include "jsonhelper.h"
#include "Background.h"
#include "Area.h"
#include <chrono>

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
};

