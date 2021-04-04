#include "BackgroundLoader.h"

#include <iostream>

BackgroundLoader::BackgroundLoader()
{
}
BackgroundLoader*  BackgroundLoader::_Instance;
#pragma region json file checks
bool BackgroundLoader::tilesetFileChecks(const rapidjson::Value& val) {
	if (!checkJSONValue("columns",     JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("image",       JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("imageheight", JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("imagewidth",  JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("margin",      JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("name",        JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("spacing",     JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("tilecount",   JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("tileheight",  JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("tilewidth",   JSONTYPE::INT,    val)) { return false; }
	return true;
}
bool BackgroundLoader::layersChecks(const rapidjson::Value& val) {
	if (!val.IsObject()) { std::cerr << "a layer value is not an object" << std::endl; return false; }
	
	if (!checkJSONValue("id",      JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("name",    JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("opacity", JSONTYPE::NUMBER, val)) { return false; }
	if (!checkJSONValue("type",    JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("visible", JSONTYPE::BOOL, val)) { return false; }
	if (!checkJSONValue("x", JSONTYPE::NUMBER, val)) { return false; }
	if (!checkJSONValue("y", JSONTYPE::NUMBER, val)) { return false; }
	if (val["type"].GetString() == "tilelayer") {
		if (!checkJSONValue("data", JSONTYPE::ARRAY, val)) { return false; }
		if (!checkJSONValue("height", JSONTYPE::NUMBER, val)) { return false; }
		if (!checkJSONValue("width", JSONTYPE::NUMBER, val)) { return false; }
	}
	
	return true;
}
bool BackgroundLoader::tilesetChecks(const rapidjson::Value& val) {
	if (!val.IsObject()) { std::cerr << "a tileset value is not an object" << std::endl; return false; }
	if (!checkJSONValue("firstgid", JSONTYPE::INT,    val)) { return false; }
	if (!checkJSONValue("source",   JSONTYPE::STRING, val)) { return false; }
	return true;
}
#pragma endregion
bool BackgroundLoader::loadTilesets(const rapidjson::Document & doc, Background & backgroundref, std::string folder)
{
	using namespace rapidjson;
	if (!checkJSONValue("tilesets", JSONTYPE::ARRAY, doc)) { return false; }
	backgroundref.numtilesets = doc["tilesets"].Size();
	backgroundref.tilesets = new BG_TileSet[backgroundref.numtilesets];
	for (SizeType i = 0; i < backgroundref.numtilesets; i++) {
		const Value& val = doc["tilesets"][i];
		if (!tilesetChecks(val)) { return false; }
		Document doc;
		std::string path = folder + "/"+ val["source"].GetString();
		if (!loadJSONFile(doc, path.c_str())) {
			std::cerr << "tileset" << path << " failed to load" << std::endl;
			return false;
		}
		if (!tilesetFileChecks(doc)) { return false; }
		BG_TileSet& tileset = backgroundref.tilesets[i];

		tileset.columns = doc["columns"].GetInt();
		tileset.imgheight = doc["imageheight"].GetInt();
		tileset.imgpath = doc["image"].GetString();
		tileset.imgwidth = doc["imagewidth"].GetInt();
		tileset.margin = doc["margin"].GetInt();
		tileset.name = doc["name"].GetString();
		tileset.spacing = doc["spacing"].GetInt();
		tileset.tilecount = doc["tilecount"].GetInt();
		tileset.tileheight = doc["tileheight"].GetInt();
		tileset.tilewidth = doc["tilewidth"].GetInt();
	}
	return true;
}
bool BackgroundLoader::loadLayers(const rapidjson::Document & doc, Background & backgroundref)
{
	using namespace rapidjson;
	if (!checkJSONValue("layers", JSONTYPE::ARRAY, doc)) { return false; }
	int total_layers = doc["layers"].Size();
	backgroundref.numlayers = 0;
	for (SizeType i = 0; i < total_layers; i++) {
		const Value& val = doc["layers"][i];
		if (!layersChecks(val)) { return false; }
		std::string type = val["type"].GetString();
		if(type == "tilelayer") {
			backgroundref.numlayers++;
		}
	}
	backgroundref.layers = new BG_Layer[backgroundref.numlayers];
	int ontilelayer = 0;
	
	for (SizeType i = 0; i < backgroundref.numlayers; i++) {
		const Value& val = doc["layers"][i];
		std::string type = val["type"].GetString();
		if ( type == "tilelayer") {
			
			BG_Layer& layer = backgroundref.layers[ontilelayer];
			layer.width = val["width"].GetInt();
			layer.height = val["height"].GetInt();
			layer.tiles = new unsigned int[layer.width * layer.height];
			layer.name = val["name"].GetString();
			for (SizeType j = 0; j < val["data"].Size(); j++) {
				layer.tiles[j] = val["data"][j].GetInt();
			}
			ontilelayer++;
		}
		
		else if (type == "objectgroup" && val["name"].GetString() == "gameobjects") {

		}
	}
	return true;
}
bool BackgroundLoader::loadBackground(std::string folder, std::string file, Background& backgroundref) {
	using namespace rapidjson;
	Document doc;
	if (!loadJSONFile(doc, (folder + "/" + file).c_str())) {
		std::cerr << "problem loading " << (folder + "/" + file).c_str() << std::endl;
		return false;
	}
	if (!checkJSONValue("width", JSONTYPE::INT, doc) || !checkJSONValue("height", JSONTYPE::INT, doc)) { return false; }
	backgroundref.width = doc["width"].GetInt();
	backgroundref.height = doc["height"].GetInt();

	if (!checkJSONValue("tilewidth", JSONTYPE::INT, doc) || !checkJSONValue("tileheight", JSONTYPE::INT, doc)) { return false; }
	backgroundref.tilewidth = doc["tilewidth"].GetInt();
	backgroundref.tileheight = doc["tileheight"].GetInt();

	if (!loadTilesets(doc, backgroundref, folder)) { return false; }

	if (!loadLayers(doc, backgroundref)) { return false; }
	backgroundref.loadTilesets();
	unsigned char testdata[16 * 16 * 4];
	backgroundref.genLayersTextures();
	backgroundref.setInitialScale();
	backgroundref.genBuffers();
	return true;
}