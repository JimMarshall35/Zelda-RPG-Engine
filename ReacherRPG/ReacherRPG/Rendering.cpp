#include "Rendering.h"

#define NUM_CHANNELS 4
#define VERTICES_SIZE 20
#define FLOATS_PER_VERTEX 5
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
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &TextureID);
}

void Sprite::draw(const glm::vec2 pos, const glm::vec3 scale, Shader& s, const Camera* camera)
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
	GLPrintErrors((std::string("glBindVertexArray(VAO); VAO =  ")+std::to_string(VAO)).c_str());
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
	std::cout << "VAO = " << VAO << std::endl;
}
