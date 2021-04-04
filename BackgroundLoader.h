#pragma once
#include <string>
#include "jsonhelper.h"
#include "Background.h"
class BackgroundLoader
{
public:
	
	static BackgroundLoader* Instance() {
		if (_Instance == nullptr) {
			_Instance = new BackgroundLoader();
		}
		return _Instance;
	}
	bool loadBackground(std::string folder, std::string file, Background& backgroundref);
private:
	BackgroundLoader();
	static BackgroundLoader* _Instance;
	bool tilesetFileChecks(const rapidjson::Value& val);
	bool layersChecks(const rapidjson::Value& val);
	bool tilesetChecks(const rapidjson::Value& val);
	bool loadTilesets(const rapidjson::Document& doc, Background& backgroundref, std::string folder);
	bool loadLayers(const rapidjson::Document& doc, Background& backgroundref);
};

