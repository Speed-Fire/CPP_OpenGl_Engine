#pragma once

#include <glm.hpp>
#include "SceneObject.h"

class CollisionPacket
{
public:
	SceneObject* owner = nullptr;

	glm::vec3 eRadius = glm::vec3(0.f); // ellipsoid radius

	//Information about the move being requested: (in R3).  R3 - standard dimension.
	glm::vec3 R3Velocity = glm::vec3(0.f);
	glm::vec3 R3Position = glm::vec3(0.f);

	//Information about the move being requested: (in eSpace)
	glm::vec3 velocity = glm::vec3(0.f);
	glm::vec3 normalizedVelocity = glm::vec3(0.f);
	glm::vec3 basePoint = glm::vec3(0.f);

	//Hit information
	bool foundCollision = false;
	double nearestDistance = 0.0;
	glm::vec3 intersectionPoint = glm::vec3(0.f);

	SceneObject* hitObj = nullptr;
	bool isChanged = false;
};