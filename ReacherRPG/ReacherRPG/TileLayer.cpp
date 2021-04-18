#include "TileLayer.h"
#include <iostream>
#include "stb_image_write.h"
#include "GLTextureHelper.h"
#define NUM_CHANNELS 4
TileLayers::TileLayers()
{
}


void TileLayers::freeData()
{
	GLClearErrors();
	for (size_t i = 0; i < num_fg_layers; i++) {
		TileLayer layer = fg_layers[i];
		GLPrintErrors("glDeleteTextures(1, &layer.textureID);");
		delete[] layer.tiles;
		layer.sprite.freeData();
	}
	for (size_t i = 0; i < num_bg_layers; i++) {
		TileLayer layer = bg_layers[i];
		GLPrintErrors("glDeleteTextures(1, &layer.textureID);");
		delete[] layer.tiles;
		layer.sprite.freeData();
	}
	delete[] bg_layers;
	delete[] fg_layers;
}

unsigned int TileLayers::getTileAtPosition(glm::vec2 pos, std::string layername)
{
	//std::cout <<"x: " << pos.x << " y: "<< pos.y << std::endl;
	//return 0;
	const TileLayer* layer = nullptr;
	for (size_t i = 0; i < num_bg_layers; i++) {
		if (bg_layers[i].name == layername) {
			layer = &bg_layers[i];
		}
	}
	float bg_width = 2 * base_scale.x;
	float bg_height = 2 * base_scale.y;

	float worldspace_tilesizex = bg_width / static_cast<float>(width);
	float worldspace_tilesizey = bg_height / static_cast<float>(height);
	glm::vec2 map_tl = glm::vec2(-base_scale.x, base_scale.y);
	glm::vec2 pos_rel_maptl = pos - map_tl;

	int tilex = pos_rel_maptl.x / worldspace_tilesizex;
	int tiley = -pos_rel_maptl.y / worldspace_tilesizey;

	int index = tilex + tiley * width;
	if (layer != nullptr) {
//		if (layer->tiles[index] > 0) {
//			std::cout << "tilex = " << tilex << " tiley = " << tiley << std::endl;
//		}
		return layer->tiles[index];
	}
	else {
		return -1;
	}
	
	
}

void TileLayers::debugPrint()
{

}

void TileLayers::draw_fg(const Shader& s, const Camera* camera)
{


	for (size_t i = 0; i < num_fg_layers; i++) {

		TileLayer layer = fg_layers[i];
		layer.sprite.draw(position, base_scale, s, camera);
	}
}
void TileLayers::draw_bg(const Shader& s, const Camera* camera)
{


	for (size_t i = 0; i < num_bg_layers; i++) {

		TileLayer layer = bg_layers[i];
		layer.sprite.draw(position, base_scale, s, camera);
	}
}

void TileLayers::genLayersTextures(TileSet& tileset, TileLayer* layers, size_t numlayers)
{

	// alocate a single tiles worth of pixel bytes to be used to copy from spritesheet to our generated texture
	unsigned char* single_tile_buffer = new unsigned char[tileset.tileheight*tileset.tilewidth * NUM_CHANNELS];
	for (size_t i = 0; i < numlayers; i++) {
		TileLayer& layer = layers[i];
		const unsigned int numtiles = layer.width*layer.height;
		// allocate pixel data for the texture we're generating from the tilemap
		const int buffer_size = numtiles * tileset.tilewidth*tileset.tileheight * NUM_CHANNELS;
		unsigned char* tex_data = new unsigned char[buffer_size];
		// blank out texture with all zeros
		memset(tex_data, 0, buffer_size);
		unsigned char* write_start_ptr = tex_data; // points to top left corner byte of the box in our texture that's about to be written to 
		for (int j = 0; j < numtiles; j++) {
			// x and y coordinates of block (in tiles)
			int x = ((j + 1) % layer.width);
			const int y = ((j + 1) / layer.width);
			// tile to be written - 0 means no tile
			const int tile = layer.tiles[j];
			if (tile > 0) {
				// get bytes from sprite sheet that make up the tile
				tileset.getTileBytes(tile, single_tile_buffer);
				for (size_t k = 0; k < tileset.tileheight; k++) {
					// write row by row to texture being generated
					unsigned char* dest = write_start_ptr + (k*tileset.tilewidth*layer.width*NUM_CHANNELS);
					const unsigned char* src = single_tile_buffer + (k*tileset.tilewidth*NUM_CHANNELS);
					memcpy(dest, src, tileset.tilewidth*NUM_CHANNELS);
				}
			}
			// increment write_start_pointer 
			write_start_ptr = tex_data + (x*tileset.tilewidth*NUM_CHANNELS) + (y*tileset.tileheight*tileset.tilewidth*(layer.width)*NUM_CHANNELS);
		}

		layer.sprite.setup(
			tex_data,
			layer.width*tileset.tilewidth, layer.height*tileset.tileheight
		);
		// for debugging
		//stbi_write_png(("debug_img/"+layer.name+".png").c_str(), tileset.tilewidth*layer.width, tileset.tileheight*layer.height, NUM_CHANNELS, tex_data, tileset.tilewidth*layer.width * NUM_CHANNELS);

		delete[] tex_data;
	}
	delete[] single_tile_buffer;
}

void TileLayers::setInitialScale() {
	base_scale = glm::vec3(
		static_cast<float>(width) / 40.0f,
		static_cast<float>(height) / 40.0f,
		1.0f
	);
	Camera::Instance()->setBounds(glm::vec2(base_scale.x,base_scale.y));
}