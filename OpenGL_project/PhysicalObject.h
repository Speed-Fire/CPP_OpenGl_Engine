#pragma once

#include "SceneObject.h"

#define _GRAVITY glm::vec3(0.f, -1.f, 0.f)

class CollisionPacket;

class PhysicalObject : public SceneObject
{
private:
	bool physicStatus;

	//Collision
	CollisionPacket* collisionPackage;
	int collisionRecursionDepth = 0;
	float acceleration = 1.f;
	const float unitsPerMeter = 100.0f;
	const float minMeshHeight = 0.1f;
	bool isCollision;

	SceneObject* objBelow;

protected:
	glm::vec3 velocity;
	float dt;

	float mass;

public:
	PhysicalObject();

	glm::vec3 getVelocity() const { return this->velocity; }
	bool getCollisionStatus() const{ return this->isCollision; }
	bool getPhysicStatus() const { return this->physicStatus; }

	virtual void setCollisionStatus(const bool status) { this->isCollision = status; }
	void setPhysicalStatus(const bool status) { this->physicStatus = status; }

	virtual void applyForce(glm::vec3 force) = 0;

	virtual void update() override
	{
		this->moveUpd();
	}

protected:
	virtual void moveUpd() = 0;

	virtual void Tick(Event* event) override
	{
		this->dt = *static_cast<float*>(event->getParameter());
	}

	void collideAndSlide(const glm::vec3& vel, const glm::vec3& gravity);

	glm::vec3 collideWithWorld(const glm::vec3& pos, const glm::vec3& vel);

	bool checkIsInAir();
};