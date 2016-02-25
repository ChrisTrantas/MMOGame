#include "Texture.h"

Texture* Texture::getTexture(string texturePath)
{
	Texture* texture = (Texture*)ResourceManager::manager->getResource(texturePath);
	if (!texture)
		texture = new Texture(texturePath);
	return texture;
}

Texture::Texture(string texturePath) : Resource(texturePath, MY_TYPE_INDEX)
{
	FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(texturePath.c_str(), 0), texturePath.c_str()); // raw texture data
	FIBITMAP* theImage = FreeImage_ConvertTo32Bits(bitmap); // texture ready data
	int imageWidth = FreeImage_GetWidth(theImage);
	int imageHeight = FreeImage_GetHeight(theImage);

	//glGenTextures(1, &texID);
	//glBindTexture(GL_TEXTURE_2D, texID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(theImage)); // load data into texture
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FreeImage_Unload(bitmap); // free memory
	FreeImage_Unload(theImage);
}

Texture::~Texture()
{
	//glDeleteTextures(1, &texID);
}

//GLuint Texture::id()
//{
//	return texID;
//}
