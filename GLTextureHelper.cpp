#include "GLTextureHelper.h"


bool GPULoadTexture(const unsigned char * data, GLuint width, GLuint height, GLuint* id)
{
	GLClearErrors();
	glGenTextures(1, id);
	GLPrintErrors("glGenTextures(1, id);");
	glBindTexture(GL_TEXTURE_2D, *id);
	GLPrintErrors("glBindTexture(GL_TEXTURE_2D, *id);");
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}