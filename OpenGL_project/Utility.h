#pragma once

#include <glm.hpp>

typedef unsigned int uint32;
#define in(a) ((uint32&) a)

//Determines if a point is inside a triangle or not.
//This is used in the collision detection step.
static bool checkPointInTriangle(
	const glm::vec3& point,
	const glm::vec3& pa,
	const glm::vec3& pb,
	const glm::vec3& pc
)
{
	glm::vec3 e10 = pb - pa;
	glm::vec3 e20 = pc - pa;

	float a = glm::dot<3, float, glm::qualifier::highp>(e10, e10);
	float b = glm::dot<3, float, glm::qualifier::highp>(e10, e20);
	float c = glm::dot<3, float, glm::qualifier::highp>(e20, e20);
	float ac_bb = (a * c) - (b * b);
	glm::vec3 vp(point.x - pa.x, point.y - pa.y, point.z - pa.z);

	float d = glm::dot<3, float, glm::qualifier::highp>(vp, e10);
	float e = glm::dot<3, float, glm::qualifier::highp>(vp, e20);
	float x = (d * c) - (e * b);
	float y = (e * a) - (d * b);
	float z = x + y - ac_bb;

	return ((in(z) & ~(in(x) | in(y)) ) & 0x80000000);
}

//Finds the lowest root of the quadratic equation
static bool getLowestRoot(
	float a,
	float b,
	float c,
	float maxR,
	float* root	// output parameter
)
{
	float determinant = b * b - 4.0f * a * c;

	//Check if a solution exists
	if (determinant < 0.0f)
		return false;

	//Calculates two roots
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2 * a);
	float r2 = (-b + sqrtD) / (2 * a);

	//Sort so x1 <= x2
	if (r1 > r2)
	{
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}

	//Get lowest root
	if (r1 > 0 && r1 < maxR)
	{
		*root = r1;
		return true;
	}

	//It is possible that we want x2 - this can happen if x1 < 0
	if (r2 > 0 && r2 < maxR)
	{
		*root = r2;
		return true;
	}

	//No (valid) solutions
	return false;
}