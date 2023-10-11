#include "PhysicalObject.h"
#include "CollisionSystem.h"

PhysicalObject::PhysicalObject()
	:
	SceneObject()
{
	this->velocity = glm::vec3(0.f);

	this->isCollision = true;
	this->physicStatus = false;
	this->dt = 0.f;

	this->mass = 1.f;

	this->collisionPackage = new CollisionPacket();
	this->collisionPackage->owner = this;
	this->collisionPackage->eRadius = glm::vec3(0.5f, 1.f, 0.5f);
}

void PhysicalObject::collideAndSlide(const glm::vec3& vel, const glm::vec3& gravity)
{
	if (!this->physicStatus)
		return;
	
	//Do collision detection:
	this->collisionPackage->R3Position = this->position;
	this->collisionPackage->R3Velocity = vel;

	//calculate position and velocity eSpace
	glm::vec3 eSpacePosition = this->collisionPackage->R3Position /
		this->collisionPackage->eRadius;
	glm::vec3 eSpaceVelocity = this->collisionPackage->R3Velocity /
		this->collisionPackage->eRadius;

	//Iterate until we have our final position
	this->collisionRecursionDepth = 0;

	glm::vec3 finalPosition = collideWithWorld(eSpacePosition, eSpaceVelocity);

	//TODO: включить гравитацию(раскомментировать этот блок)
	//Add gravity pull:

	if (this->checkIsInAir())
	{
		///Set the new R3 position (convert back from eSpace to R3)
		this->collisionPackage->R3Position = finalPosition * this->collisionPackage->eRadius;
		this->collisionPackage->R3Velocity = gravity * this->acceleration;

		eSpaceVelocity = (gravity * this->acceleration) / this->collisionPackage->eRadius;
		this->collisionRecursionDepth = 0;

		finalPosition = collideWithWorld(finalPosition, eSpaceVelocity);

		//Ускорение свободного падения
		if(this->acceleration < 50.f)
			this->acceleration += 0.5f;
		///gravity pull end.
	}
	else
	{
		this->acceleration = 1.f;

		if (this->objBelow)
		{
			if (auto obj = dynamic_cast<PhysicalObject*>(this->objBelow))
			{
				glm::vec3 objVel = obj->getVelocity();
				
				std::cout << this->objBelow << "  " << this << "  ObjVel = " << glm::length(objVel) << "\n";

				if (glm::length(objVel) > 0)
				{
					this->collisionPackage->R3Position = finalPosition * this->collisionPackage->eRadius;
					this->collisionPackage->R3Velocity = objVel;

					/*if (objVel.y != 0.f)
					{
						this->collisionPackage->R3Position.y += objVel.y * this->dt;
						finalPosition += objVel.y * this->dt / this->collisionPackage->eRadius;
					}*/

					eSpaceVelocity = objVel / this->collisionPackage->eRadius;
					this->collisionRecursionDepth = 0;

					finalPosition = collideWithWorld(finalPosition, eSpaceVelocity * this->dt);
				}
			}
		}
	}

	this->objBelow = nullptr;

	//Convert final result back to R3
	finalPosition = finalPosition * this->collisionPackage->eRadius;

	if (finalPosition.y <= 0.0f)
		finalPosition.y = 0.f;

	//Move the entity
	this->setPosition(finalPosition);
}

glm::vec3 PhysicalObject::collideWithWorld(const glm::vec3& pos, const glm::vec3& vel)
{
	float unitScale = this->unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	//do we need to worry?
	if (this->collisionRecursionDepth > 5)
		return pos;

	//OK, we need to worry:
	this->collisionPackage->velocity = vel;
	this->collisionPackage->normalizedVelocity = glm::normalize(vel);
	this->collisionPackage->basePoint = pos;
	this->collisionPackage->foundCollision = false;

	//Check for collision (calls the collision routines)
	CollisionSystem::getInstance()->doCollision(this->collisionPackage);

	//If no collision we just move along the velocity
	if (this->collisionPackage->foundCollision == false)
	{
		return pos + vel;
	}

	// *** Collision occurred ***

	//The original destination point
	glm::vec3 destinationPoint = pos + vel;
	glm::vec3 newBasePoint = pos;

	//only update if we are not already very close
	// and if so we only move very close to intersection.. not to exact spot.
	if (this->collisionPackage->nearestDistance >= veryCloseDistance)
	{
		glm::vec3 V = vel;
		//TODO: возможно нужно брать по модулю
		V *= abs(this->collisionPackage->nearestDistance - veryCloseDistance);
		newBasePoint = this->collisionPackage->basePoint + V;

		//Adjust polygon intersection point (so sliding
		//plane will be unaffected by the fact that we
		//move slightly less than collision tells us)
		V = glm::normalize(V);
		this->collisionPackage->intersectionPoint -= veryCloseDistance * V;
	}

	//Determine the sliding plane
	glm::vec3 slidePlaneOrigin = this->collisionPackage->intersectionPoint;
	glm::vec3 slidePlaneNormal = glm::normalize(newBasePoint - this->collisionPackage->intersectionPoint);
	Plane slidingPlane(slidePlaneOrigin, slidePlaneNormal);

	glm::vec3 newDestinationPoint = destinationPoint - (float)slidingPlane.signedDistanceTo(destinationPoint) * slidePlaneNormal;

	//Generate the slide vector, which will become our new
	// velocity vector for the next iteration
	glm::vec3 newVelosityVector = newDestinationPoint - this->collisionPackage->intersectionPoint;


	//Recurse:

	// don't recurse if the new velocity is very small
	if (glm::length(newVelosityVector) < veryCloseDistance)
	{
		return newBasePoint;
	}
	
	if (!this->checkIsInAir())
	{
		if (newVelosityVector.y < 0.f)
		{
			newVelosityVector.y = 0.f;
		}
	}

	this->collisionRecursionDepth++;
	return this->collideWithWorld(newBasePoint, newVelosityVector);
}

bool PhysicalObject::checkIsInAir()
{
	if (this->position.y <= 0.0f)
		return false;

	//TODO: неправильно работает, если игрок находится на ребре модели

	CollisionPacket tempColPack = CollisionPacket();
	tempColPack.eRadius = this->collisionPackage->eRadius;
	tempColPack.R3Position = this->position;
	tempColPack.R3Velocity = glm::vec3(0.f, -tempColPack.eRadius.y, 0.f);
	tempColPack.owner = this;

	tempColPack.velocity = tempColPack.R3Velocity / tempColPack.eRadius;
	tempColPack.normalizedVelocity = glm::normalize(tempColPack.velocity);
	tempColPack.basePoint = tempColPack.R3Position / tempColPack.eRadius;
	tempColPack.foundCollision = false;

	CollisionSystem::getInstance()->doCollision(&tempColPack);
	
	//std::cout << tempColPack.foundCollision << "\n";

	if (tempColPack.foundCollision == false || tempColPack.nearestDistance > this->minMeshHeight)
	{
		return true;
	}
	else
	{
		this->objBelow = tempColPack.hitObj;
		return false;
	}
}