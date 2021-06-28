#include "TileSet.h"
#define NUM_CHANNELS 4
#include "stb_image_write.h"

TileSet::TileSet()
{
}


TileSet::~TileSet()
{
}

void TileSet::getTileBytes(unsigned int tilenum, unsigned char * output)
{
	
	unsigned int tileset_width_px = (tilewidth + spacing) * columns;
	unsigned int start_pixel_x = ((tilewidth + spacing) * (tilenum - 1)) % tileset_width_px;
	unsigned int start_pixel_y = (tileheight + spacing) * ((tilenum - 1) / columns);
	unsigned int start_pixel = (start_pixel_y * imgwidth) + start_pixel_x;

	unsigned char* src_ptr = imgdata + (start_pixel * NUM_CHANNELS);
	unsigned char* output_ptr = output;
	for (size_t i = 0; i < tileheight; i++) {
		memcpy(output_ptr, src_ptr, (tilewidth * NUM_CHANNELS));
		output_ptr += (tilewidth * NUM_CHANNELS);
		src_ptr += imgwidth*NUM_CHANNELS;
	}
	//stbi_write_png("test.png", tileset.tilewidth, tileset.tileheight, 4, output, tileset.tilewidth *NUM_CHANNELS);
}
void TileSet::tileDirectMemCpy(unsigned int tilenum, unsigned char* output, unsigned int output_img_width) {
	// copies tile data direct to a texture being generated - unlike method above which transfers it to a buffer first
	// tilenum is the index of the tile to copy
	unsigned int tileset_width_px = (tilewidth + spacing) * columns;
	unsigned int start_pixel_x = ((tilewidth + spacing) * (tilenum - 1)) % tileset_width_px;
	unsigned int start_pixel_y = (tileheight + spacing) * ((tilenum - 1) / columns);
	unsigned int start_pixel = (start_pixel_y * imgwidth) + start_pixel_x;

	unsigned char* src_ptr = imgdata + (start_pixel * NUM_CHANNELS);
	unsigned char* output_ptr = output;
	for (size_t i = 0; i < tileheight; i++) {
		memcpy(output_ptr, src_ptr, (tilewidth * NUM_CHANNELS));
		output_ptr += (output_img_width * NUM_CHANNELS);
		src_ptr += imgwidth*NUM_CHANNELS;
	}
}

void TileSet::freeData()
{
	stbi_image_free(imgdata);
	namedSpriteMap.clear();
	if (texID > 0) {
		glDeleteTextures(1, &texID);
		texID = 0;
	}
	for (size_t i = 0; i < sprites.size(); i++) {
		sprites[i].freeData();
	}
	sprites.clear();
	sprites.shrink_to_fit();
}

void TileSet::genTexture()
{
	GPULoadTexture(imgdata, imgwidth, imgheight, &texID);
}

void TileSet::genSprites()
{
	for (size_t i = 0; i < tilecount; i++) {
		int x_coord = i % columns;
		int y_coord = i / columns;
		int x = x_coord * tilewidth;
		int y = y_coord * tileheight;
		Sprite sprite;
		sprite.setup_preexisting(
			texID,
			imgwidth, imgheight,
			x,y,
			tilewidth, tileheight
		);
		sprites.push_back(sprite);

	}
}

void TileSet::genMetaSprite(const unsigned int * tiles, const unsigned int width, const unsigned int numtiles, const std::string name)
{
	unsigned int pixelsheight = (numtiles / width) * tileheight;
	unsigned int pixelswidth = width * tilewidth;
	//std::cout << name << std::endl;
	unsigned char* tex_data = new unsigned char[pixelsheight*pixelswidth*NUM_CHANNELS];
	unsigned char* single_tile_buffer = new unsigned char[tileheight*tilewidth*NUM_CHANNELS];
	memset(tex_data, 0, pixelsheight*pixelswidth*NUM_CHANNELS);
	unsigned char* write_ptr = tex_data;
	for (int i = 0; i < numtiles; i++) {
		int x = (i % width);
		const int y = (i / width);
		const int tile = tiles[i];
		write_ptr = tex_data + (y*width*tilewidth*tileheight*NUM_CHANNELS) + (x*tilewidth*NUM_CHANNELS);
		//std::cout << "x: " << x << " y: " << y << std::endl;
		getTileBytes(tile + 1, single_tile_buffer);
		//stbi_write_png(("debug_img/" + std::to_string(tile) + ".png").c_str(), tilewidth, tileheight, NUM_CHANNELS, single_tile_buffer, tilewidth * NUM_CHANNELS);
		if (tile > 0) {
			for (int j = 0; j < tileheight; j++) {
				unsigned char* src;
				unsigned char* dest;
				src = single_tile_buffer + (j*tilewidth*NUM_CHANNELS);
				dest = write_ptr + (j*pixelswidth*NUM_CHANNELS);
				memcpy(dest, src, tilewidth*NUM_CHANNELS);
			}
		}
		
		
	}
	//stbi_write_png(("debug_img/"+name+".png").c_str(), pixelswidth, pixelsheight, NUM_CHANNELS, tex_data, pixelswidth * NUM_CHANNELS);
	Sprite sprite;
	sprite.setup(tex_data, pixelswidth, pixelsheight);//, 0, 0, pixelswidth, pixelsheight);
	sprites.push_back(sprite);
	int index = sprites.size() - 1;
	namedSpriteMap.insert(std::pair<std::string, int>(name, index));
	delete[] tex_data;
	delete[] single_tile_buffer;
}
