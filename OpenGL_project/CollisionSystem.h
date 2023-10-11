#pragma once

#include<vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Mesh.h"
#include "CollisionLogic.h"

#include "SceneObject.h"

#include <iostream>

class Model;

class CollisionSystem
{
private:
	//constructors/ destructors
	CollisionSystem() : sceneObjs(nullptr) {}
	~CollisionSystem() {}

	//Arrays
	std::vector<SceneObject*>* sceneObjs;

	bool isActive = false;
public:
	static CollisionSystem* getInstance() 
	{
		static CollisionSystem instance;
		return &instance;
	}

	void setObjs(std::vector<SceneObject*>* objects)
	{
		this->sceneObjs = objects;
	}

	
	void doCollision(CollisionPacket* colPack);
};