#include "Rendering.h"
#include "stb_image.h"
#include <iterator>
#include <vector>
#define NUM_CHANNELS 4
#define VERTICES_SIZE 20
#define FLOATS_PER_VERTEX 5

GLuint Sprite::FullImgVAO;



void Sprite::generateFullImgVAO() {
	GLClearErrors();
	GLuint VBO;
	GLuint EBO;
	glGenVertexArrays(1, &FullImgVAO);
	GLPrintErrors("glGenVertexArrays(1, &VAO);");
	glGenBuffers(1, &VBO);
	GLPrintErrors("glGenBuffers(1, &VBO);");
	glGenBuffers(1, &EBO);
	GLPrintErrors("glGenBuffers(1, &EBO);");
	glBindVertexArray(FullImgVAO);
	GLPrintErrors("glBindVertexArray(VAO);");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, VBO);");
	glBufferData(GL_ARRAY_BUFFER, sizeof(Full_img_vertices), Full_img_vertices, GL_STATIC_DRAW);
	GLPrintErrors("glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	GLPrintErrors("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(full_img_indices), full_img_indices, GL_STATIC_DRAW);
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
	std::cout << "Static full img VAO = " << FullImgVAO << std::endl;
}




void Sprite::setup_base(
	unsigned int img_width, unsigned int img_height,
	unsigned int sheet_x, unsigned int sheet_y,
	unsigned int sheet_w, unsigned int sheet_h)
{
	glm::vec2 tl_UV, tr_UV, bl_UV, br_UV;
	const float left = static_cast<float>(sheet_x) / static_cast<float>(img_width);
	const float top = static_cast<float>(sheet_y) / static_cast<float>(img_height);

	const float uvwidth = static_cast<float>(sheet_w) / static_cast<float>(img_width);
	const float uvheight = static_cast<float>(sheet_h) / static_cast<float>(img_height);

	tr_UV.x = left + uvwidth;   tr_UV.y = top;             // top right
	br_UV.x = left + uvwidth;   br_UV.y = top + uvheight;  // bottom right
	bl_UV.x = left;             bl_UV.y = top + uvheight;  // bottom left
	tl_UV.x = left;             tl_UV.y = top;             // top left

	glm::vec2 uvs[] = { tr_UV, br_UV, bl_UV, tl_UV };
	size_t onuv = 0;
	for (size_t i = 3; i < 20; i += FLOATS_PER_VERTEX) {
		vertices[i] = uvs[onuv].x;
		vertices[i + 1] = uvs[onuv].y;
		onuv++;
	}
	width_pixels = sheet_w;
	height_pixels = sheet_h;
}
Sprite::Sprite()
{
}
void Sprite::setup_preexisting(
	GLuint tex_id,
	unsigned int img_width, unsigned int img_height,
	unsigned int sheet_x, unsigned int sheet_y,
	unsigned int sheet_w, unsigned int sheet_h)
{

	setup_base(
		img_width, img_height,
		sheet_x, sheet_y,
		sheet_w, sheet_h
	);
	TextureID = tex_id;
	genbuffers();
}

void Sprite::setup(
	unsigned char * img_data,
	unsigned int img_width, unsigned int img_height,
	unsigned int sheet_x, unsigned int sheet_y,
	unsigned int sheet_w, unsigned int sheet_h)
{

	setup_base(
		img_width, img_height,
		sheet_x, sheet_y,
		sheet_w, sheet_h
	);
	GPULoadTexture(img_data, img_width, img_height, &TextureID);
	std::cout << "TEX_ID = " << TextureID << std::endl;
	genbuffers();
}

void Sprite::freeData() {
	if (VAO != FullImgVAO) {
		glDeleteVertexArrays(1, &VAO);
	}
	
	glDeleteTextures(1, &TextureID);
}

void Sprite::draw(const glm::vec2 pos, const glm::vec3 scale, const Shader& s, const Camera* camera)
{
	GLClearErrors();

	s.use();
	s.setInt("Texture", 0);
	GLPrintErrors("s.setInt(\"Texture\", 0);");

	s.setVec3("base_scale", scale);
	GLPrintErrors("s.setVec3(\"base_scale\", base_scale);");

	s.setVec2("position", pos);
	GLPrintErrors("s.setVec2(\"position\",position);");

	s.setVec2("camera_pos", camera->position);
	GLPrintErrors("s.setVec2(\"camera_pos\", camera.position);");

	s.setFloat("camera_zoom", camera->zoom);
	GLPrintErrors("s.setFloat(\"camera_zoom\", camera.zoom);");

	glActiveTexture(GL_TEXTURE0);
	GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
	glBindTexture(GL_TEXTURE_2D, TextureID);
	GLPrintErrors((std::string("glBindTexture(GL_TEXTURE_2D, TextureID); ID= ") + std::to_string(TextureID)).c_str());

	glBindVertexArray(VAO);
	GLPrintErrors((std::string("glBindVertexArray(VAO); VAO =  ") + std::to_string(VAO)).c_str());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	GLPrintErrors("glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);");

	glBindVertexArray(0);
}



void Sprite::genbuffers()
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
	//std::cout << "VAO = " << VAO << std::endl;
}
void Sprite::setup(unsigned char* img_data, unsigned int img_width, unsigned int img_height) {
	GPULoadTexture(img_data, img_width, img_height, &TextureID);
	VAO = FullImgVAO;
}



void Sprite::setup_preexisting(GLuint tex_id, unsigned int img_width, unsigned int img_height) {
	TextureID = tex_id;
	VAO = FullImgVAO;
}
void Sprite::freeFullImgVAO() {
	glDeleteVertexArrays(1, &FullImgVAO);
}



void initRendering()
{
	Sprite::generateFullImgVAO();
}

void cleanupRendering()
{
	Sprite::freeFullImgVAO();
}



void TextRenderer::loadFont( std::string font)
{
	
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	FT_Set_Pixel_Sizes(face, 0, base_text_height);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; c++)
	{
		GLClearErrors();
		// load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// generate texture
		unsigned int texture;
		GPULoadUITexture(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, &texture);
		// now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
	std::cout << "text renderer initialized successfully" << std::endl;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
TextRenderer::TextRenderer()
{
	GLClearErrors();
	shader = Shader("shaders/text_vert.glsl", "shaders/text_frag.glsl");
	shader.use();
	glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 1024.0f);
	shader.setMat4("projection", projection);
	genBuffers();
}
void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 colour)
{
	GLClearErrors();
	// activate corresponding render state	
	shader.use();
	GLPrintErrors("s.use();");
	//glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);

	
	//shader.setVec3("textColour",colour);
	GLPrintErrors("s.setVec3(\"textColour\",colour);");
	glActiveTexture(GL_TEXTURE0);
	GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
	glBindVertexArray(VAO);
	GLPrintErrors("glBindVertexArray(VAO);");
	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		GLPrintErrors("glBindTexture(GL_TEXTURE_2D, ch.TextureID);");
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, VBO);");
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		GLPrintErrors("glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, 0);");
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		GLPrintErrors("glDrawArrays(GL_TRIANGLES, 0, 6);");
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void TextRenderer::freeData()
{
	auto it = Characters.begin();
	while (it != Characters.end())
	{
		glDeleteTextures(1,&it->second.TextureID);
		it++;
	}

	Characters.clear();
}
unsigned int TextRenderer::geTextWidth(std::string text)
{
	unsigned int accumulated_width = 0;

	std::string::const_iterator c;
	
	
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];
		accumulated_width += ch.Advance;
	}
	return accumulated_width >> 6;
}
void TextRenderer::genBuffers() {
	GLClearErrors();
	glGenVertexArrays(1, &VAO);
	GLPrintErrors("glGenVertexArrays(1, &VAO);");
	glGenBuffers(1, &VBO);
	GLPrintErrors("glGenBuffers(1, &VBO);");
	glBindVertexArray(VAO);
	GLPrintErrors("glBindVertexArray(VAO);");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, VBO);");
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	GLPrintErrors("glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);");
	glEnableVertexAttribArray(0);
	GLPrintErrors("glEnableVertexAttribArray(0);");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	GLPrintErrors("glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UISpriteRenderer::RenderSprite(std::string name, float x, float y, float scale)
{
	GLClearErrors();
	// activate corresponding render state	
	shader.use();
	GLPrintErrors("shader.use(); ui sprite renderer");
	//glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);


	GLPrintErrors("shader.setMat4(\"projection\", projection);");

	glActiveTexture(GL_TEXTURE0);
	GLPrintErrors("glActiveTexture(GL_TEXTURE0);");
	glBindVertexArray(VAO);
	GLPrintErrors("glBindVertexArray(VAO);");

	UISprite ch = sprites[name];

	float xpos = x + ch.Bearing.x * scale;
	float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

	float w = ch.Size.x * scale;
	float h = ch.Size.y * scale;
	// update VBO for each character
	float vertices[6][4] = {
		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos,     ypos,       0.0f, 1.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },

		{ xpos,     ypos + h,   0.0f, 0.0f },
		{ xpos + w, ypos,       1.0f, 1.0f },
		{ xpos + w, ypos + h,   1.0f, 0.0f }
	};
	// render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	GLPrintErrors("glBindTexture(GL_TEXTURE_2D, ch.TextureID);");
	// update content of VBO memory
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, VBO);");
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	GLPrintErrors("glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLPrintErrors("glBindBuffer(GL_ARRAY_BUFFER, 0);");
	// render quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	GLPrintErrors("glDrawArrays(GL_TRIANGLES, 0, 6);");
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool UISpriteRenderer::loadUISprite(std::string img_path, std::string name)
{
	int x, y, n;
	unsigned char* data = stbi_load(img_path.c_str(), &x, &y, &n, 0);
	if (data == NULL) {
		std::cerr << "problem loading image at " << img_path.c_str() << std::endl;
		return false;
	}
	GLuint tex_id;
	GPULoadTexture(data, x, y, &tex_id);
	UISprite sprite;
	sprite.Size = glm::ivec2(x, y);
	sprite.Bearing = glm::ivec2(0, 0);
	sprite.TextureID = tex_id;
	sprites.insert(std::pair<std::string, UISprite>(name, sprite));
	return true;
}

void UISpriteRenderer::init()
{
	shader = Shader("shaders/text_vert.glsl", "shaders/sprite_frag.glsl");
	shader.use();
	glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 1024.0f);
	shader.setMat4("projection", projection);
}

void UISpriteRenderer::freeData()
{
	auto it = sprites.begin();
	while (it != sprites.end())
	{
		glDeleteTextures(1, &it->second.TextureID);
		it++;
	}

	sprites.clear();
}
