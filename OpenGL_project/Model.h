#pragma once

#include "PhysicalObject.h"

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Material.h"
#include "OBJLoaderr.h"

#include <iostream>

class Model : public PhysicalObject
{
private:
	Material* material;
	Texture* overrideTextureDiffuse;
	Texture* overrideTextureSpecular;
	std::vector<Mesh*> meshes;

	void updateUniforms()
	{

	}

public:
	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		std::vector<Mesh*> meshes
		)
		:
		PhysicalObject()
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		this->setPhysicalStatus(true);

		this->mass = 10.f;

		for (auto* i : meshes)
		{
			this->meshes.push_back(new Mesh(*i));
		}

		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	//OBJ file loaded model
	Model(
		glm::vec3 position,
		Material* material,
		Texture* orTexDif,
		Texture* orTexSpec,
		const char* obj_file
	)
		:
		PhysicalObject()
	{
		this->position = position;
		this->material = material;
		this->overrideTextureDiffuse = orTexDif;
		this->overrideTextureSpecular = orTexSpec;

		std::vector<Vertex> mesh = loadOBJ(obj_file);
		this->meshes.push_back(new Mesh(mesh.data(), mesh.size(), NULL, 0, glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)));


		for (auto& i : this->meshes)
		{
			i->move(this->position);
			i->setOrigin(this->position);
		}
	}

	~Model()
	{
		for (auto*& i : this->meshes)
		{
			delete i;
		}
	}

	//Accessors
	std::vector<Mesh*>* getMeshes() { return &this->meshes; }

	//Functions
	virtual void setPosition(const glm::vec3 position) override
	{
		glm::vec3 prevPos = this->position;
		this->position = position;
		for (auto& i : this->meshes)
		{
			i->setPosition(i->getPosition() + position - prevPos);
			i->setOrigin(this->position);
		}
	}

	virtual void setRotation(const glm::vec3 rotation) override
	{
		for (auto& i : this->meshes)
			i->setRotation(rotation);
	}

	virtual void setScale(const glm::vec3 scale) override
	{
		for (auto*& i : this->meshes)
			i->setScale(scale);
	}

	virtual void move(const glm::vec3 position) override
	{
		this->position += position;

		for (auto& i : this->meshes)
		{
			i->setOrigin(this->position);
			i->move(position);
		}
	}

	virtual void rotate(const glm::vec3 rotation) override
	{
		for (auto& i : this->meshes)
			i->rotate(rotation);
	}

	virtual void scaleChng(const glm::vec3 scale) override
	{
		for (auto*& i : this->meshes)
			i->scaleChng(scale);
	}

	virtual void applyForce(glm::vec3 force) override 
	{
		this->velocity += force;
		//std::cout << "Length = " << glm::length(this->velocity) << "\n";
	}

	virtual void setCollisionStatus(const bool status) override
	{
		PhysicalObject::setCollisionStatus(status);

		for (auto*& i : this->meshes)
		{
			i->setColliding(status);
		}
	}


	virtual void update() override
	{

		PhysicalObject::update();
	}

	virtual void render(Shader* shader) override
	{
		//Update uniforms
		this->updateUniforms();

		this->material->sendToShader(*shader);

		//Use a shader
		shader->use();

		//Draw
		for (auto& i : this->meshes)
		{
			//Activate texture for each mesh
			this->overrideTextureDiffuse->bind(0);
			this->overrideTextureSpecular->bind(1);

			i->render(shader); //Activates shader also
		}
	}

protected:
	virtual void moveUpd() override
	{
		if (glm::length(this->velocity) > 0)
		{
			if (glm::length(this->velocity) < 0.001f)
			{
				this->velocity = glm::vec3(0.f);
			}
			else
			{
				if (this->checkIsInAir())
					this->velocity *= 0.95f;
				else
					this->velocity *= 0.9f * (1 / this->mass);
			}
		}

		this->collideAndSlide(this->velocity * this->dt, _GRAVITY * this->dt);

		/*if (this->checkIsInAir())
			std::cout << "In Air\n";
		else
			std::cout << this->position.y << "\n";*/
	}

//EVENT DISPATHCERS

	//Event Tick:
	virtual void Tick(Event* event/*float dt*/) override 
	{
		PhysicalObject::Tick(event);
	}

	//Event dispatcher: BeginPlay
	virtual void BeginPlay(Event* event) override { }

	virtual void BeginOverlap(Event* event) override {}
};
