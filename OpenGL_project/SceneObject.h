#pragma once

#include <glm.hpp>
#include "Shader.h"
#include "EventSystem.h"
#include "MethodEventHandler.h"

class SceneObject
{
protected:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

public:
	SceneObject()
	{
		this->position = glm::vec3(0.f);
		this->rotation = glm::vec3(0.f);
		this->scale = glm::vec3(1.f);

		Listener* handl1 = new MethodEventHandler<SceneObject>(this, &SceneObject::Tick);
		Listener* handl2 = new MethodEventHandler<SceneObject>(this, &SceneObject::BeginPlay);
		EventSystem::Instance()->RegisterClient("TICK", handl1);
		EventSystem::Instance()->RegisterClient("BEGINPLAY", handl2);
	}

	inline glm::vec3 getPosition() const { return this->position; }
	inline glm::vec3 getRotation() const { return this->rotation; }
	inline glm::vec3 getScale() const { return this->scale; }

	virtual void setPosition(const glm::vec3 position) = 0;
	virtual void setRotation(const glm::vec3 rotation) = 0;
	virtual void setScale(const glm::vec3 scale) = 0;

	virtual void move(const glm::vec3 position) = 0;
	virtual void rotate(const glm::vec3 rotation) = 0;
	virtual void scaleChng(const glm::vec3 scale) = 0;

	virtual void update() = 0;
	virtual void render(Shader* shader) = 0;


protected:

	//Event Tick:
	virtual void Tick(Event* event) = 0;

	//Event BeginPlay:
	virtual void BeginPlay(Event* event) = 0;

	//Event BeginOverlap:
	virtual void BeginOverlap(Event* event) = 0;
};