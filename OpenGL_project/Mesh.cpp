#include "Mesh.h"

Mesh::Mesh(Vertex* vertexArray, const unsigned& nrOfVertices,
		   GLuint* indexArray, const unsigned& nrOfIndices,
		   glm::vec3 position,
		   glm::vec3 origin,
		   glm::vec3 rotation,
		   glm::vec3 scale)
	:
	isColliding(true)
{
	this->position = position;
	this->origin = origin;
	this->rotation = rotation;
	this->scale = scale;

	this->nrOfVertices = nrOfVertices;
	this->nrOfIndices = nrOfIndices;

	this->vertexArray = new Vertex[this->nrOfVertices];
	for (size_t i = 0; i < nrOfVertices; i++)
	{
		this->vertexArray[i] = vertexArray[i];
	}

	this->indexArray = new GLuint[this->nrOfIndices];
	for (size_t i = 0; i < nrOfIndices; i++)
	{
		this->indexArray[i] = indexArray[i];
	}

	this->initVAO();
	this->updateModelMatrix();
}

Mesh::Mesh(Primitive* primitive, glm::vec3 position, glm::vec3 origin, glm::vec3 rotation, glm::vec3 scale)
	:
	isColliding(true)
{
	this->position = position;
	this->origin = origin;
	this->rotation = rotation;
	this->scale = scale;

	this->nrOfVertices = primitive->getNrOfVertices();
	this->nrOfIndices = primitive->getNrOfIndices();

	this->vertexArray = new Vertex[this->nrOfVertices];
	for (size_t i = 0; i < this->nrOfVertices; i++)
	{
		this->vertexArray[i] = primitive->getVertices()[i];
	}

	this->indexArray = new GLuint[this->nrOfIndices];
	for (size_t i = 0; i < this->nrOfIndices; i++)
	{
		this->indexArray[i] = primitive->getIndices()[i];
	}

	this->initVAO();
	this->updateModelMatrix();
}

Mesh::Mesh(const Mesh& obj)
{
	this->position = obj.position;
	this->origin = obj.origin;
	this->rotation = obj.rotation;
	this->scale = obj.scale;

	this->isColliding = obj.isColliding;

	this->nrOfVertices = obj.nrOfVertices;
	this->nrOfIndices = obj.nrOfIndices;

	this->vertexArray = new Vertex[this->nrOfVertices];
	for (size_t i = 0; i < this->nrOfVertices; i++)
	{
		this->vertexArray[i] = obj.vertexArray[i];
	}

	this->indexArray = new GLuint[this->nrOfIndices];
	for (size_t i = 0; i < this->nrOfIndices; i++)
	{
		this->indexArray[i] = obj.indexArray[i];
	}

	this->initVAO();
	this->updateModelMatrix();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);

	if (this->nrOfIndices > 0)
	{
		glDeleteBuffers(1, &this->EBO);
	}

	delete[] this->vertexArray;
	delete[] this->indexArray;
}

void Mesh::initVAO()
{
	//Create VAO
	//GEN VAO AND BIND									//// VAO - vertex array object.
	glCreateVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	//GEN VBO AND BIND AND SEND DATA					//// VBO - vertex buffer object.
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->nrOfVertices * sizeof(Vertex), this->vertexArray, GL_STATIC_DRAW);

	//GEN EBO AND BIND AND SEND DATA					//// EBO - element buffer object.
	if (this->nrOfIndices > 0)
	{
		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrOfIndices * sizeof(GLuint), this->indexArray, GL_STATIC_DRAW);
	}

	//SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
	//Position
	//GLuint attribLocation = glGetAttribLocation(core_program, "vertex_position");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	//Texcoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	//Normal
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(3);
	
	//BIND VAO 0
	glBindVertexArray(0);
}

void Mesh::updateUniforms(Shader* shader)
{
	shader->setMat4fv(this->ModelMatrix, "ModelMatrix");
}

void Mesh::updateModelMatrix()
{
	this->ModelMatrix = glm::mat4(1.f);
	this->ModelMatrix = glm::translate(this->ModelMatrix, this->origin);
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
	this->ModelMatrix = glm::translate(this->ModelMatrix, this->position - this->origin);
	this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}

void Mesh::update()
{

}

void Mesh::render(Shader* shader)
{
	//Update uniforms
	this->updateModelMatrix();
	this->updateUniforms(shader);

	shader->use();

	//Bind VAO
	glBindVertexArray(this->VAO);

		//Render
		if (this->nrOfIndices == 0)
		{
			glDrawArrays(GL_TRIANGLES, 0, this->nrOfVertices);
		}
		else
			glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);
	
	
	//Cleanup
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
