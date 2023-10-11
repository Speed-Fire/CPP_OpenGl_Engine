#pragma once

#include <iostream>
#include <string>

#include <glew.h>
#include <glfw3.h>
#include <SOIL2.h>

struct TextureParams
{
	int soil_load_type = SOIL_LOAD_RGBA;

	int texture_wrap_s = GL_REPEAT;
	int texture_wrap_t = GL_REPEAT;
	float texture_border_color[4] = {0.f, 0.f, 0.f, 0.f};
	int texture_min_filter = GL_LINEAR;
	int texture_mag_filter = GL_LINEAR_MIPMAP_LINEAR;
};

class Texture
{
private:
	GLuint id;
	int height;
	int width;
	unsigned int type;

public:
	Texture(const char* fileName, GLenum type, TextureParams prms)
	{
		this->type = type;
		this->loadFromFile(fileName, prms);
	}

	//Only for materials
	/*Texture(){}*/

	~Texture()
	{
		glDeleteTextures(1, &this->id);
	}

	inline GLuint getID() const { return this->id; }


	void bind(const GLint texture_unit)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(this->type, this->id);
	}

	void unbind()
	{
		glActiveTexture(0);
		glBindTexture(this->type, 0);
	}

	void loadFromFile(const char* fileName, TextureParams prms)
	{
		if (this->id)
		{
			glDeleteTextures(1, &this->id);
		}
		
		unsigned char* image = SOIL_load_image(fileName, &this->width, &this->height, NULL, prms.soil_load_type);

		//GLuint texture0;
		glGenTextures(1, &this->id);
		glBindTexture(this->type, this->id);

		glTexParameteri(this->type, GL_TEXTURE_WRAP_S, prms.texture_wrap_s);
		glTexParameteri(this->type, GL_TEXTURE_WRAP_T, prms.texture_wrap_t);
		glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, prms.texture_mag_filter);
		glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, prms.texture_min_filter);
		glTexParameterfv(this->type, GL_TEXTURE_BORDER_COLOR, prms.texture_border_color);
		
		if (image)
		{
			if (prms.soil_load_type == SOIL_LOAD_RGBA)
				glTexImage2D(type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			else if(prms.soil_load_type == SOIL_LOAD_RGB)
				glTexImage2D(type, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(type);
		}
		else
		{
			std::cout << "ERROR::TEXTURE::TEXTURE_LOADING_FAILED: " << fileName << "\n";
		}

		glActiveTexture(0);
		glBindTexture(this->type, 0);
		SOIL_free_image_data(image);
	}
};