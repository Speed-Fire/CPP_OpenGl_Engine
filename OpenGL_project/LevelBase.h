#pragma once

#include "libs.h"
#include "CharacterEntity.h"

class Level
{
protected:
	CharacterEntity* character;

	std::vector<Texture*> textures;

	std::vector<Material*> materials;

	std::vector<SceneObject*> sceneObjects;

	std::vector<Light*> lights;

	void loadTextures()
	{

	}

	void loadMaterials()
	{

	}

	void loadsceneObjects()
	{

	}

	void loadLights()
	{

	}

public:
	Level(std::string path)
	{
		//TODO: сделать сериализацию и десереализацию объектов уровня в файл

		this->loadTextures();
		this->loadMaterials();
		this->loadsceneObjects();
		this->loadLights();
	}

	~Level()
	{
		for (size_t i = 0; i < this->textures.size(); i++)
			delete this->textures[i];

		for (size_t i = 0; i < this->materials.size(); i++)
			delete this->materials[i];

		for (auto*& i : this->sceneObjects)
			delete i;

		for (size_t i = 0; i < this->lights.size(); i++)
			delete this->lights[i];
	}

	void getLevelObjects(
		std::vector<Texture*>* texArr,
		std::vector<Material*>* matArr,
		std::vector<SceneObject*>* sceneObjArr,
		std::vector<Light*>* lightsArr,
		CharacterEntity* mainCharacter
	)
	{
		texArr = &this->textures;
		matArr = &this->materials;
		sceneObjArr = &this->sceneObjects;
		lightsArr = &this->lights;
		mainCharacter = this->character;
	}
};