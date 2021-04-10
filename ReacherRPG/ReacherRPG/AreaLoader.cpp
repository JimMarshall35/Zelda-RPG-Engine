
#include "AreaLoader.h"
#define PLAYER_TILESET_NAME "24by24ModernRPGGuy"
AreaLoader* AreaLoader::instance;
#pragma region json file checks
bool AreaLoader::tilesetFileChecks(const rapidjson::Value& val) {
	if (!checkJSONValue("columns", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("image", JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("imageheight", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("imagewidth", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("margin", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("name", JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("spacing", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("tilecount", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("tileheight", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("tilewidth", JSONTYPE::INT, val)) { return false; }
	return true;
}
bool AreaLoader::layersChecks(const rapidjson::Value& val) {
	if (!val.IsObject()) { std::cerr << "a layer value is not an object" << std::endl; return false; }

	if (!checkJSONValue("id", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("name", JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("opacity", JSONTYPE::NUMBER, val)) { return false; }
	if (!checkJSONValue("type", JSONTYPE::STRING, val)) { return false; }
	if (!checkJSONValue("visible", JSONTYPE::BOOL, val)) { return false; }
	if (!checkJSONValue("x", JSONTYPE::NUMBER, val)) { return false; }
	if (!checkJSONValue("y", JSONTYPE::NUMBER, val)) { return false; }
	if (val["type"].GetString() == "tilelayer") {
		if (!checkJSONValue("data", JSONTYPE::ARRAY, val)) { return false; }
		if (!checkJSONValue("height", JSONTYPE::NUMBER, val)) { return false; }
		if (!checkJSONValue("width", JSONTYPE::NUMBER, val)) { return false; }
	}
	if (val["type"].GetString() == "objectgroup") {
		if (!checkJSONValue("objects", JSONTYPE::ARRAY, val)) { return false; }
	}

	return true;
}
bool AreaLoader::tilesetChecks(const rapidjson::Value& val) {
	if (!val.IsObject()) { std::cerr << "a tileset value is not an object" << std::endl; return false; }
	if (!checkJSONValue("firstgid", JSONTYPE::INT, val)) { return false; }
	if (!checkJSONValue("source", JSONTYPE::STRING, val)) { return false; }
	return true;
}
#pragma endregion


bool AreaLoader::loadArea(std::string folder, std::string file, Area & arearef)
{
	using namespace rapidjson;
	std::cout << "loading file " << folder << "/" << file << std::endl;
	std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	arearef.freeData();
	Document doc;
	if (!loadJSONFile(doc, (folder + "/" + file).c_str())) {
		std::cerr << "problem loading " << (folder + "/" + file).c_str() << std::endl;
		return false;
	}
	if (!checkJSONValue("width", JSONTYPE::INT, doc) || !checkJSONValue("height", JSONTYPE::INT, doc)) { return false; }
	arearef.background.width = doc["width"].GetInt();
	arearef.background.height = doc["height"].GetInt();

	if (!checkJSONValue("tilewidth", JSONTYPE::INT, doc) || !checkJSONValue("tileheight", JSONTYPE::INT, doc)) { return false; }
	arearef.background.tilewidth = doc["tilewidth"].GetInt();
	arearef.background.tileheight = doc["tileheight"].GetInt();

	if (!loadTilesets(doc, arearef, folder)) { return false; }
	if (!loadLayers(doc, arearef)) { return false; }

	TileSet t = arearef.tilesets[0];
	arearef.background.genLayersTextures(t);
	arearef.background.setInitialScale();
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	std::cout << "background loading done in " << time_span.count() * 1000 << " ms" << std::endl;
	std::cout << std::endl << std::endl;
	return true;
}



bool AreaLoader::loadTilesets(const rapidjson::Document & doc, Area & arearef, std::string folder)
{
	using namespace rapidjson;
	if (!checkJSONValue("tilesets", JSONTYPE::ARRAY, doc)) { return false; }
	arearef.numtilesets = doc["tilesets"].Size();
	arearef.tilesets = new TileSet[arearef.numtilesets];
	for (SizeType i = 0; i < arearef.numtilesets; i++) {
		const Value& val = doc["tilesets"][i];
		if (!tilesetChecks(val)) { return false; }
		Document doc;
		std::string path = folder + "/" + val["source"].GetString();
		if (!loadJSONFile(doc, path.c_str())) {
			std::cerr << "tileset" << path << " failed to load" << std::endl;
			return false;
		}
		if (!tilesetFileChecks(doc)) { return false; }
		TileSet& tileset = arearef.tilesets[i];

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
		if (!loadTilesetImgData(tileset)) { return false; }
		if (tileset.name == PLAYER_TILESET_NAME) {
			tileset.genTexture();
			tileset.genSprites();
		}
	}
	return true;
}
bool AreaLoader::loadTilesetImgData(TileSet& tileset)
{

	int x, y, n;
	tileset.imgdata = stbi_load(tileset.imgpath.c_str(), &x, &y, &n, 0);
	if (tileset.imgdata == NULL) {
		std::cerr << "problem loading image at " << tileset.imgpath.c_str() << std::endl;
		return false;
	}

	return true;
}
bool AreaLoader::loadLayers(const rapidjson::Document & doc, Area & arearef)
{
	using namespace rapidjson;
	if (!checkJSONValue("layers", JSONTYPE::ARRAY, doc)) { return false; }
	int total_layers = doc["layers"].Size();
	arearef.background.numlayers = 0;
	for (SizeType i = 0; i < total_layers; i++) {
		const Value& val = doc["layers"][i];
		if (!layersChecks(val)) { return false; }
		std::string type = val["type"].GetString();
		if (type == "tilelayer") {
			arearef.background.numlayers++;
		}
	}
	arearef.background.layers = new BG_Layer[arearef.background.numlayers];
	int ontilelayer = 0;

	for (SizeType i = 0; i < total_layers; i++) {
		const Value& val = doc["layers"][i];
		std::string type = val["type"].GetString();
		std::string name = val["name"].GetString();
		if (type == "tilelayer") {

			BG_Layer& layer = arearef.background.layers[ontilelayer];
			layer.width = val["width"].GetInt();
			layer.height = val["height"].GetInt();
			layer.tiles = new unsigned int[layer.width * layer.height];
			layer.name = val["name"].GetString();
			for (SizeType j = 0; j < val["data"].Size(); j++) {
				layer.tiles[j] = val["data"][j].GetInt();
			}
			ontilelayer++;
		}

		else if (type == "objectgroup" && name == "gameobjects") {
			for (SizeType i = 0; i < val["objects"].Size(); i++) {
				const Value& object = val["objects"][i];
				std::string objname = object["name"].GetString();
				if (objname == "start_point") {
					/////////////////////////////// BAD HARD CODED CODE STARTS
					// just quick code to test 
					// will proably write PlayerLoader class or similar
					Player* player = new Player();
					TileSet* player_tileset = arearef.getTilesetByName(PLAYER_TILESET_NAME);
					Sprite** upFrames = new Sprite*[4]{
						player_tileset->getSprite(0),
						player_tileset->getSprite(1),
						player_tileset->getSprite(2),
						player_tileset->getSprite(3)
					};
					Sprite** downFrames = new Sprite*[4]{
						player_tileset->getSprite(4),
						player_tileset->getSprite(5),
						player_tileset->getSprite(6),
						player_tileset->getSprite(7)
					};
					Sprite** rightFrames = new Sprite*[4]{
						player_tileset->getSprite(8),
						player_tileset->getSprite(9),
						player_tileset->getSprite(10),
						player_tileset->getSprite(11)
					};
					Sprite** leftFrames = new Sprite*[4]{
						player_tileset->getSprite(12),
						player_tileset->getSprite(13),
						player_tileset->getSprite(14),
						player_tileset->getSprite(15)
					};

					player->animator.push_animation("walk_up", upFrames, 4, 10, true);
					player->animator.push_animation("walk_down", downFrames, 4, 10, true);
					player->animator.push_animation("walk_left", leftFrames, 4, 10, true);
					player->animator.push_animation("walk_right", rightFrames, 4, 10, true);
					player->animator.set_anim("walk_down");
					player->scale *= glm::vec2(
						player_tileset->tilewidth / (arearef.background.tilewidth * 40.0),
						player_tileset->tileheight/(arearef.background.tileheight * 40.0)
					);
#define RESOLUTION 16
#define LOFFSET 5.0
#define ROFFSET 6.0
#define TOFFSET 12.0
#define BOFFSET 1.0
					player->collider.left_offset = LOFFSET;
					player->collider.right_offset = ROFFSET;
					player->collider.top_offset = TOFFSET;
					player->collider.bottom_offset = BOFFSET;
					player->collider.resolution = RESOLUTION;
					arearef.gameobjects.push_back(player);
					////////////////// BAD CODE ENDS! /////////////////////////////////////
				}
			}
		}
	}
	return true;
}
AreaLoader::AreaLoader()
{
}


AreaLoader::~AreaLoader()
{
}
