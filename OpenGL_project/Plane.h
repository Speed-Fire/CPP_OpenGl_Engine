#pragma once

#include<glm.hpp>
#include <iostream>

class Plane
{
public:
	float equation[4];
	glm::vec3 origin;
	glm::vec3 normal;

	//CONSTRUCTORS
	//Constructor from origin and normal
	Plane(const glm::vec3& origin, const glm::vec3& normal)
	{
		this->normal = normal;
		this->origin = origin;

		equation[0] = normal.x;
		equation[1] = normal.y;
		equation[2] = normal.z;
		equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
	}

	//Constructor from triangle
	Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
	{
		this->normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
		this->origin = p1;

		equation[0] = normal.x;
		equation[1] = normal.y;
		equation[2] = normal.z;
		equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
	}


	//FUNCTIONS
	bool isFrontFacingTo(const glm::vec3& direction) const
	{
		double dot = glm::dot<3, double, glm::qualifier::highp>(this->normal, direction);
		return (dot <= 0); 
	}

	float signedDistanceTo(const glm::vec3& point) const
	{
		float dot = glm::dot<3, float, glm::qualifier::highp>(point, normal);

		return dot + equation[3];
	}
};