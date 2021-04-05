#include "Background.h"
#include <iostream>
#include "stb_image_write.h"
#include "GLTextureHelper.h"
#define NUM_CHANNELS 4
Background::Background()
{
}


void Background::freeData()
{
	GLClearErrors();
	for (size_t i = 0; i < numlayers; i++) {
		BG_Layer layer = layers[i];
		GLPrintErrors("glDeleteTextures(1, &layer.textureID);");
		delete[] layer.tiles;
		layer.sprite.freeData();
	}
	delete[] layers;
}

void Background::debugPrint()
{

}

void Background::draw(Shader& s, const Camera& camera)
{	
	GLClearErrors();
	s.use();
	GLPrintErrors("s.use();");
	
	for (size_t i = 0; i < numlayers; i++) {
		
		BG_Layer layer = layers[i];
		layer.sprite.draw(position, base_scale,s,camera);
	}
}

void Background::genLayersTextures(TileSet& tileset)
{

	// alocate a single tiles worth of pixel bytes to be used to copy from spritesheet to our generated texture
	unsigned char* single_tile_buffer = new unsigned char[tileset.tileheight*tileset.tilewidth * NUM_CHANNELS];    
	for (size_t i = 0; i < numlayers; i++) {
		BG_Layer& layer = layers[i];
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
			const int y = ((j+1) / layer.width);
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
			write_start_ptr = tex_data +(x*tileset.tilewidth*NUM_CHANNELS)+(y*tileset.tileheight*tileset.tilewidth*(layer.width)*NUM_CHANNELS);
		}

		layer.sprite.setup(
			tex_data,
			layer.width*tileset.tilewidth, layer.height*tileset.tileheight,
			0 ,0,
			layer.width*tileset.tilewidth, layer.height*tileset.tileheight
		);
		// for debugging
		//stbi_write_png((layer.name+".png").c_str(), tileset.tilewidth*layer.width, tileset.tileheight*layer.height, NUM_CHANNELS, tex_data, tileset.tilewidth*layer.width * NUM_CHANNELS);
		
		delete[] tex_data;
	}
	delete[] single_tile_buffer;
}

void Background::setInitialScale() {
	base_scale = glm::vec3(
		static_cast<float>(width) / 40.0f,
		static_cast<float>(height) / 40.0f,
		1.0f
	);
	  
}