#pragma once
#include <vector>
#include <string>
#include "stb_image.h"
// Include GLEW
#include "Shader.h"
#include "gl_error_handling.h"
#include "Camera.h"
#include "Rendering.h"

#define NUM_CHANNELS 4
#define VERTICES_SIZE 20
#define FLOATS_PER_VERTEX 5

#include "TileSet.h"

struct TileLayer {
	std::string          name;
	unsigned int*        tiles;
	unsigned int         width;
	unsigned int         height;
	Sprite               sprite;
};
class TileLayers
{
public:
	friend class AreaLoader;
				   TileLayers();
	               ~TileLayers() { freeData(); }
	void           debugPrint();
	void           draw_bg(Shader& s, const Camera* camera);
	void           draw_fg(Shader& s, const Camera* camera);
	glm::vec2      get_base_scale() { return glm::vec2(base_scale); }
	void           freeData();
	unsigned int   getTileAtPosition(glm::vec2 pos, std::string layername);
private:
	TileLayer*     bg_layers;
	TileLayer*     fg_layers;
	void           genLayersTextures(TileSet& tileset, TileLayer* layers, size_t numlayers);
	unsigned int   num_bg_layers;
	unsigned int   num_fg_layers;
	unsigned int   numtilesets;
	unsigned int   width;
	unsigned int   height;
	unsigned int   tilewidth;
	unsigned int   tileheight;
	void           setInitialScale();
	glm::vec3      base_scale;
	glm::vec2      position = glm::vec2(0.0);
};

