#pragma once

#include<iostream>
#include <vector>

#include "Vertex.h"
#include "Primitives.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

class Mesh
{
private:
	Vertex* vertexArray;
	unsigned nrOfVertices;
	GLuint* indexArray;
	unsigned nrOfIndices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glm::vec3 position;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelMatrix;

	bool isColliding;

	void initVAO();

	void updateUniforms(Shader* shader);

	void updateModelMatrix();

public:
	Mesh(Vertex* vertexArray, const unsigned& nrOfVertices,
		 GLuint* indexArray, const unsigned& nrOfIndices,
		 glm::vec3 position = glm::vec3(0.f),
		 glm::vec3 origin = glm::vec3(0.f),
		 glm::vec3 rotation = glm::vec3(0.f),
		 glm::vec3 scale = glm::vec3(1.f));

	Mesh(Primitive* primitive,
		 glm::vec3 position = glm::vec3(0.f),
		 glm::vec3 origin = glm::vec3(0.f),
		 glm::vec3 rotation = glm::vec3(0.f),
		 glm::vec3 scale = glm::vec3(1.f));

	Mesh(const Mesh& obj);

	~Mesh();

	//Accessors
	glm::vec3 getPosition() { return this->position; }
	glm::vec3 getRotation() { return this->rotation; }
	glm::vec3 getScale() { return this->scale; }

	bool IsColliding() const { return this->isColliding; }

	//TODO: исправить проблемы с доступом к массивам(они не должны изменяться извне)
	Vertex* getVertices(unsigned int* vertSize) const
	{
		*vertSize = this->nrOfVertices;
		return this->vertexArray;
	}

	GLuint* getIndices(unsigned int* indSize)
	{
		indSize = &this->nrOfIndices;
		return this->indexArray;
	}

	//Modifiers
	void setPosition(const glm::vec3 position) { this->position = position; }
	void setOrigin(const glm::vec3 origin) { this->origin = origin; }
	void setRotation(const glm::vec3 rotation) { this->rotation = rotation; }
	void setScale(const glm::vec3 scale) { this->scale = scale; }

	void setColliding(const bool colliding) { this->isColliding = colliding; }

	//Functions
	void move(const glm::vec3 position) { this->position += position; }
	void rotate(const glm::vec3 rotation) { this->rotation += rotation; }
	void scaleChng(const glm::vec3 scale) { this->scale += scale; }


	void update();
	void render(Shader* shader);
};