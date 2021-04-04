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
		glDeleteTextures(1, &layer.textureID);
		GLPrintErrors("glDeleteTextures(1, &layer.textureID);");
		delete[] layer.tiles;
	}
	delete[] layers;
	glDeleteVertexArrays(1, &VAO);
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

		s.setInt("Texture", 0);
		GLPrintErrors("s.setInt(\"Texture\", 0);");

		s.setVec3("base_scale", base_scale);
		GLPrintErrors("s.setVec3(\"base_scale\", base_scale);");

		s.setVec2("position",position);
		GLPrintErrors("s.setVec2(\"position\",position);");

		s.setVec2("camera_pos", camera.position);
		GLPrintErrors("s.setVec2(\"camera_pos\", camera.position);");

		s.setFloat("camera_zoom", camera.zoom);
		GLPrintErrors("s.setFloat(\"camera_zoom\", camera.zoom);");

		glActiveTexture(GL_TEXTURE0);
		GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
		glBindTexture(GL_TEXTURE_2D, layer.textureID);
		GLPrintErrors("glBindTexture(GL_TEXTURE_2D, layer.textureID);");

		glBindVertexArray(VAO);
		GLPrintErrors("glBindVertexArray(VAO);");
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		GLPrintErrors("glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);");
	}
}



void Background::getTileBytes(unsigned int tilenum, TileSet & tileset, unsigned char * output)
{
	unsigned int tileset_width_px = (tileset.tilewidth + tileset.spacing) * tileset.columns;
	unsigned int start_pixel_x = ((tileset.tilewidth + tileset.spacing) * (tilenum-1)) % tileset_width_px;
	unsigned int start_pixel_y = (tileset.tileheight + tileset.spacing) * ((tilenum - 1) / tileset.columns);
	unsigned int start_pixel = (start_pixel_y * tileset.imgwidth) + start_pixel_x;

	unsigned char* src_ptr = tileset.imgdata + (start_pixel * NUM_CHANNELS);
	unsigned char* output_ptr = output;
	for (size_t i = 0; i < tileset.tileheight; i++) {
		memcpy(output_ptr, src_ptr, (tileset.tilewidth * NUM_CHANNELS));
		output_ptr += (tileset.tilewidth * NUM_CHANNELS);
		src_ptr += tileset.imgwidth*NUM_CHANNELS;
	}
	//stbi_write_png("test.png", tileset.tilewidth, tileset.tileheight, 4, output, tileset.tilewidth *NUM_CHANNELS);
}

void Background::tileDirectMemCpy(unsigned int tilenum, TileSet& tileset, unsigned char* output) {
	// will copy tile data direct to a texture being generated - unlike method above which transfers it to a buffer first
	// was done the way above because i planned to use glTextureSubImage2D to modify the background texture once uploaded to gpu
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
				getTileBytes(tile, tileset, single_tile_buffer);
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
		// upload to gpu, setting layer.textureID 
		GPULoadTexture(tex_data, layer.width*tileset.tilewidth, layer.height*tileset.tileheight, &layer.textureID);
		std::cout << "layer " << layer.name << " has texture id " << layer.textureID << std::endl;
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
		0.0f
	);
	  
}

void Background::genBuffers()
{
	GLClearErrors();
	GLuint VBO;
	GLuint EBO;
	glGenVertexArrays(1, &VAO);
	GLPrintErrors("glGenVertexArrays(1, &VAO);");
	glGenBuffers(1, &VBO);
	GLPrintErrors("glGenBuffers(1, &VBO);");
	glGenBuffers(1, &EBO);
	GLPrintErrors("glGenBuffers(1, &EBO);");
	glBindVertexArray(VAO);
	GLPrintErrors("glBindVertexArray(VAO);");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, VBO);");
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GLPrintErrors("glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	GLPrintErrors("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	GLPrintErrors("glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);");
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)0);
	GLPrintErrors("glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);");
	glEnableVertexAttribArray(0);
	GLPrintErrors("glEnableVertexAttribArray(0);");
	// texcoords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(3 * sizeof(float)));
	GLPrintErrors("glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));");
	glEnableVertexAttribArray(1);
	GLPrintErrors("glEnableVertexAttribArray(1);");
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, 0);");
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
	GLPrintErrors("glBindVertexArray(0);");
	std::cout << "VAO = " << VAO << std::endl;
}

