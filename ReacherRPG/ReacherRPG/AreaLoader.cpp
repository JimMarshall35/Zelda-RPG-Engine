
#include "AreaLoader.h"
#define PLAYER_TILESET_NAME "24by24ModernRPGGuy_edit"
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
	//if (!checkJSONValue("gensprites", JSONTYPE::BOOL, val)) { return false; }
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

	

	//std::cout << "loading file " << folder << "/" << file << std::endl;
	std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
	arearef.freeData();
	Document doc;
	if (!loadJSONFile(doc, (folder + "/" + file).c_str())) {
		std::cerr << "problem loading " << (folder + "/" + file).c_str() << std::endl;
		return false;
	}
	if (!checkJSONValue("width", JSONTYPE::INT, doc) || !checkJSONValue("height", JSONTYPE::INT, doc)) { return false; }
	arearef.tilelayers.width = doc["width"].GetInt();
	arearef.tilelayers.height = doc["height"].GetInt();

	if (!checkJSONValue("tilewidth", JSONTYPE::INT, doc) || !checkJSONValue("tileheight", JSONTYPE::INT, doc)) { return false; }
	arearef.tilelayers.tilewidth = doc["tilewidth"].GetInt();
	arearef.tilelayers.tileheight = doc["tileheight"].GetInt();

	if (!loadTilesets(doc, arearef, folder)) { return false; }
	if (!loadLayers(doc, arearef)) { return false; }

	TileSet* t = arearef.tilesets;
	arearef.tilelayers.genLayersTextures(t, arearef.tilelayers.bg_layers, arearef.tilelayers.num_bg_layers);
	arearef.tilelayers.genLayersTextures(t, arearef.tilelayers.fg_layers, arearef.tilelayers.num_fg_layers);
	arearef.tilelayers.setInitialScale();
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0);
	std::cout << "area "<< file << " loading done in " << time_span.count() * 1000 << " ms" << std::endl;
	//std::cout << std::endl << std::endl;
	/*
	ScriptableGameObject* testgo1 = new ScriptableGameObject("scripts/test1.lua");
	ScriptableGameObject* testgo2 = new ScriptableGameObject("scripts/test2.lua");
	testgo1->setGamePtr(arearef.getGamePtr());
	testgo2->setGamePtr(arearef.getGamePtr());
	arearef.gameobjects.push_back(testgo1);
	arearef.gameobjects.push_back(testgo2);
	*/
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
		if (checkJSONValue("gensprites", JSONTYPE::BOOL, doc, false)) {
			if (doc["gensprites"].GetBool()) {
				tileset.genTexture();
				tileset.genSprites();
			}
		}

		if (checkJSONValue("metasprites", JSONTYPE::ARRAY, doc, false)) {
			for (SizeType j = 0; j < doc["metasprites"].Size(); j++) {
				const Value& metasprite = doc["metasprites"][j];
				if (!checkJSONValue("name", JSONTYPE::STRING, metasprite)) {
					std::cerr << "object " << j << " in metasprites of tileset " << path << std::endl;
					break;
				}
				if (!checkJSONValue("tiles", JSONTYPE::ARRAY, metasprite)) {
					std::cerr << "object " << j << " in metasprites of tileset " << path << std::endl;
					break;
				}
				if (!checkJSONValue("width", JSONTYPE::INT, metasprite)) {
					std::cerr << "object " << j << " in metasprites of tileset " << path << std::endl;
					break;
				}
				unsigned int width = metasprite["width"].GetInt();
				std::string name = metasprite["name"].GetString();
				unsigned int numtiles = metasprite["tiles"].Size();
				unsigned int* tiles = new unsigned int[numtiles];
				for (SizeType k = 0; k < numtiles; k++) {
					tiles[k] = metasprite["tiles"][k].GetInt();
				}
				tileset.genMetaSprite(tiles, width, numtiles, name);
				delete[] tiles;
			}
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
	arearef.tilelayers.num_bg_layers = 0;
	arearef.tilelayers.num_fg_layers = 0;
	for (SizeType i = 0; i < total_layers; i++) {
		const Value& val = doc["layers"][i];
		if (!layersChecks(val)) { return false; }
		std::string type = val["type"].GetString();
		if (type == "tilelayer") {
			std::string layername = val["name"].GetString();
			if (layername.substr(0, 3) == "fg_") {
				arearef.tilelayers.num_fg_layers++;
			}
			else {
				arearef.tilelayers.num_bg_layers++;
			}
		}
	}
	arearef.tilelayers.bg_layers = new TileLayer[arearef.tilelayers.num_bg_layers];
	arearef.tilelayers.fg_layers = new TileLayer[arearef.tilelayers.num_fg_layers];
	int onbgtilelayer = 0;
	int onfgtilelayer = 0;
	for (SizeType i = 0; i < total_layers; i++) {
		const Value& val = doc["layers"][i];
		std::string type = val["type"].GetString();
		std::string name = val["name"].GetString();
		if (type == "tilelayer" && name.substr(0, 3) == "fg_") {
			TileLayer& layer = arearef.tilelayers.fg_layers[onfgtilelayer];
			layer.width = val["width"].GetInt();
			layer.height = val["height"].GetInt();
			layer.tiles = new unsigned int[layer.width * layer.height];
			layer.name = val["name"].GetString();
			for (SizeType j = 0; j < val["data"].Size(); j++) {
				layer.tiles[j] = val["data"][j].GetInt();
			}
			onfgtilelayer++;
		}
		else if (type == "tilelayer") {
			TileLayer& layer = arearef.tilelayers.bg_layers[onbgtilelayer];
			layer.width = val["width"].GetInt();
			layer.height = val["height"].GetInt();
			layer.tiles = new unsigned int[layer.width * layer.height];
			layer.name = val["name"].GetString();
			for (SizeType j = 0; j < val["data"].Size(); j++) {
				layer.tiles[j] = val["data"][j].GetInt();
			}
			onbgtilelayer++;
		}
		else if (type == "objectgroup" && name == "gameobjects") {
			for (SizeType i = 0; i < val["objects"].Size(); i++) {
				const Value& object = val["objects"][i];
				std::string objname = object["name"].GetString();
				std::string objtype = object["type"].GetString();
				
				if (objtype == "sciptable_gameobject") {

					if (!checkJSONValue("x", JSONTYPE::NUMBER, object)) { continue; }
					if (!checkJSONValue("y", JSONTYPE::NUMBER, object)) { continue; }
					float json_x = object["x"].GetFloat();
					float json_y = object["y"].GetFloat();
					TiledProperty script_url;
					if (!getTiledObjectProperty(object["properties"], "scripts", script_url)) {}//continue; }
					glm::vec2 player_start_pos = tiledPosToGameEnginePos(glm::vec2(json_x, json_y), arearef);
					/////////////////////////////////////////////////
					ScriptableGameObject* scriptable = new ScriptableGameObject();


					scriptable->position = player_start_pos;

					scriptable->setGamePtr(arearef.getGamePtr());
					scriptable->init(script_url.s);
					/*
					player->position += glm::vec2(
						(player->scale.x * 2.0f) / 2.0f,
						(player->scale.y * 2.0f) / 2.0f
					);
					*/
					arearef.gameobjects.push_back(scriptable);
					////////////////////////////////////////////////////
				}
				
				if (objtype == "static_metasprite_object") {
					TiledProperty tileset_name_value, metasprite_name_value, t_offset, b_offset, l_offset, r_offset, collidable;
					StaticSprite* s_sprite = new StaticSprite();
					if (!getTiledObjectProperty(object["properties"], "tileset_name", tileset_name_value)) {}//continue; }
					if (!getTiledObjectProperty(object["properties"], "metasprite_name", metasprite_name_value)) {}//continue; }
					if (!getTiledObjectProperty(object["properties"], "collider_b_offset", b_offset)) {}
					if (!getTiledObjectProperty(object["properties"], "collider_t_offset", t_offset)) {}
					if (!getTiledObjectProperty(object["properties"], "collider_l_offset", l_offset)) {}
					if (!getTiledObjectProperty(object["properties"], "collider_r_offset", r_offset)) {}
					if (!getTiledObjectProperty(object["properties"], "collidable", collidable)) { std::cout << "error" << std::endl; }

					s_sprite->sprite = arearef.getTilesetByName(tileset_name_value.s)->getNamedSprite(metasprite_name_value.s);
					glm::vec2 json_pos(object["x"].GetFloat(), object["y"].GetFloat());
					s_sprite->position = tiledPosToGameEnginePos(json_pos, arearef);
					s_sprite->scale *= glm::vec2(
						object["width"].GetFloat() / (arearef.tilelayers.tilewidth * 40.0),
						object["height"].GetFloat() / (arearef.tilelayers.tileheight * 40.0)
					);

					s_sprite->position += glm::vec2(
						(s_sprite->scale.x),
						(s_sprite->scale.y)
					);

					s_sprite->collider.top_offset = t_offset.f;
					s_sprite->collider.bottom_offset = b_offset.f;
					s_sprite->collider.left_offset = l_offset.f;
					s_sprite->collider.right_offset = r_offset.f;
					s_sprite->collider.pixelswidth = object["width"].GetFloat();
					s_sprite->collider.pixelsheight = object["height"].GetFloat();
					s_sprite->collider.init(s_sprite);
					if (!collidable.b) {
						s_sprite->issolidvsgameobjects = false;
					}

					arearef.gameobjects.push_back(s_sprite);
				}
				else if (objtype == "dialogue_trigger") {
					TiledProperty text;
					if (!getTiledObjectProperty(object["properties"], "text", text)) {}
					DialogueTrigger* d_trigger = new DialogueTrigger(arearef.getGamePtr(), "hello world");
					glm::vec2 json_pos(object["x"].GetFloat(), object["y"].GetFloat());
					d_trigger->position = tiledPosToGameEnginePos(json_pos, arearef);
					d_trigger->scale *= glm::vec2(
						object["width"].GetFloat() / (arearef.tilelayers.tilewidth * 40.0),
						object["height"].GetFloat() / (arearef.tilelayers.tileheight * 40.0)
					);

					d_trigger->position += glm::vec2(             // need to properly sort out translation from tiled coordinates to this game's
						(d_trigger->scale.x),
						-(d_trigger->scale.y) 
					);
					d_trigger->setString(text.s);
					d_trigger->collider.pixelswidth = object["width"].GetFloat();
					d_trigger->collider.pixelsheight = object["height"].GetFloat();
					d_trigger->collider.top_offset = 0;
					d_trigger->collider.bottom_offset = 0;
					d_trigger->collider.left_offset = 0;
					d_trigger->collider.right_offset = 0;
					d_trigger->collider.init(d_trigger);
					arearef.gameobjects.push_back(d_trigger);
				}
				
				else if (objtype == "area_change_trigger"){
					TiledProperty folder, file;
					if (!getTiledObjectProperty(object["properties"], "folder", folder)) {}
					if (!getTiledObjectProperty(object["properties"], "file", file)) {}
					AreaChangeTrigger* a_trigger = new AreaChangeTrigger(arearef.getGamePtr(), folder.s,file.s);
					glm::vec2 json_pos(object["x"].GetFloat(), object["y"].GetFloat());
					a_trigger->position = tiledPosToGameEnginePos(json_pos, arearef);
					a_trigger->scale *= glm::vec2(
						object["width"].GetFloat() / (arearef.tilelayers.tilewidth * 40.0),
						object["height"].GetFloat() / (arearef.tilelayers.tileheight * 40.0)
					);

					a_trigger->position += glm::vec2(             // need to properly sort out translation from tiled coordinates to this game's
						(a_trigger->scale.x),
						-(a_trigger->scale.y)
					);
					a_trigger->collider.pixelswidth = object["width"].GetFloat();
					a_trigger->collider.pixelsheight = object["height"].GetFloat();
					a_trigger->collider.top_offset = 0;
					a_trigger->collider.bottom_offset = 0;
					a_trigger->collider.left_offset = 0;
					a_trigger->collider.right_offset = 0;
					a_trigger->collider.init(a_trigger);
					arearef.gameobjects.push_back(a_trigger);
				}
				
			}
		}
	}
	return true;
}
bool AreaLoader::getTiledObjectProperty(const rapidjson::Value & props_array, std::string name, TiledProperty & output)
{
	using namespace rapidjson;
	if (!props_array.IsArray()) {
		std::cout << "getTiledObjectProperty has been passed a rapidjson value that is not an array" << std::endl;
		return false;
	}
	for (SizeType i = 0; i < props_array.Size(); i++) {
		const Value& val = props_array[i];
		if (!checkJSONValue("name", JSONTYPE::STRING, val)) { return false; }
		if (!checkJSONValue("type", JSONTYPE::STRING, val)) { return false; }
		if (!checkJSONValue("value", JSONTYPE::ANY,   val)) { return false; }
		std::string jsonname = val["name"].GetString();
		if (jsonname == name) {
			std::string type = val["type"].GetString();
			if (type == "string") {
				output.s = val["value"].GetString();
				return true;
			}
			else if (type == "float") {
				output.f = val["value"].GetFloat();
				return true;
			}
			else if (type == "bool") {
				output.b = val["value"].GetBool();
				return true;
			}
		}
	}
	std::cout << "no property '" << name << "'" << std::endl;
	return false;
}
glm::vec2 AreaLoader::tiledPosToGameEnginePos(glm::vec2 tiledpos, const Area & arearef)
{
	float json_x = tiledpos.x;
	float json_y = tiledpos.y;
	float tl_x = -1.0 * (arearef.tilelayers.width / 40.0f);
	float tl_y = (arearef.tilelayers.height / 40.0f);

	glm::vec2 map_tl(tl_x, tl_y);
	float worldx = map_tl.x + (json_x / (arearef.tilelayers.tilewidth*arearef.tilelayers.width))*(arearef.tilelayers.width / 40.0f)*2.0f;
	float worldy = map_tl.y - (json_y / (arearef.tilelayers.tileheight*arearef.tilelayers.height))*(arearef.tilelayers.height / 40.0f)*2.0f;
	return glm::vec2(worldx, worldy);
}
AreaLoader::AreaLoader()
{
}


AreaLoader::~AreaLoader()
{
}
