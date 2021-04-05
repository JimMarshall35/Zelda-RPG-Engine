#include "TileSet.h"
#define NUM_CHANNELS 4


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
void TileSet::tileDirectMemCpy(unsigned int tilenum, TileSet& tileset, unsigned char* output) {
	// will copy tile data direct to a texture being generated - unlike method above which transfers it to a buffer first
	// was done the way above because i planned to use glTextureSubImage2D to modify the background texture once uploaded to gpu
}