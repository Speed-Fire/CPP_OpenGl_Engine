#pragma once

#include<glm.hpp>

#include "CollisionPacket.h"
#include "Plane.h"
#include "Utility.h"


//COLLISION STEP
//Assumes: p1,p2 and p3 are given in ellipsoid space:
static void checkTriangle(
	CollisionPacket* colPackage,
	const glm::vec3& p1,
	const glm::vec3& p2,
	const glm::vec3& p3
) 
{
	//Make the plane containing this triangle.
	Plane trianglePlane(p1, p2, p3);
	
	//Is triangle front-facing to the velocity vector?
	// We only check front-facing triangles
	if (trianglePlane.isFrontFacingTo(colPackage->normalizedVelocity))
	{
		//Get interval of plane intersection:
		float t0, t1;
		bool embeddedInPlane = false;
		
		//Calculate the signed distance from sphere
		// position to triangle plane
		float signedDistToTrianglePlane = trianglePlane.signedDistanceTo(colPackage->basePoint);
		
		//cache this as we're going to use it a few times below:
		float normalDotVelocity = glm::dot<3, float, glm::qualifier::highp>(trianglePlane.normal, colPackage->velocity);

		//If sphere is traveling parallel to the plane:
		if (normalDotVelocity == 0.0f)
		{
			
			if (fabs(signedDistToTrianglePlane) >= 1.0f)
			{
				//Sphere is not embedded in plane.
				//No collision possible: 
				return;
			}
			else
			{
				//sphere is embedded in plane.
				//It intersects in the whole range [0..1]
				embeddedInPlane = true; 
				t0 = 0.0f;
				t1 = 1.0f;
			}
		}
		else
		{
			// N dot D is not 0. Calculate intersection interval:
			t0 = (-1.f - signedDistToTrianglePlane) / normalDotVelocity;
			t1 = (1.f - signedDistToTrianglePlane) / normalDotVelocity;
			
			//Swap so t0 < t1
			if (t0 > t1)
			{
				float temp = t1;
				t1 = t0;
				t0 = temp;
			}

			//Check that at least one result is within range:
			if (t0 > 1.0f || t1 < 0.0f)
			{
				//Both t values are outside values [0,1]
				//No collision possible:
				return; 
			}
			

			//Clamp to [0,1]
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		
		//OK, at this point we have two time values t0 and t1
		// between which the swept sphere intersects with the
		// triangle plane. If any collision is to occur it must
		// happen within this interval.
		glm::vec3 collisionPoint;
		bool foundCollision = false;
		float t = 1.0f;

		//First we check for the easy case - collision inside
		// the triangle. If this happens it must be at time t0
		// as this is when the sphere rests on the front side
		// of the triangle plane. Note, this can only happen if
		// the sphere is not embedded in the triangle plane.
		if (!embeddedInPlane)
		{
			glm::vec3 colPackVelProductT0 = colPackage->velocity; 
			colPackVelProductT0 *= t0;
			glm::vec3 planeIntersectionPoint = (colPackage->basePoint - trianglePlane.normal) + colPackVelProductT0;
			
			if (checkPointInTriangle(planeIntersectionPoint, p1, p2, p3))
			{
				foundCollision = true;
				t = t0; 
				collisionPoint = planeIntersectionPoint;
			}
		}

		//If we haven't found a collision already we'll have to
		// sweep sphere against points and edges of the triangle.
		// Note: A collision inside the triangle (the check above)
		// will always happen before a vertex or edge collision!
		// This is why we can skip the swept test if the above
		// gives a collision!
		if (foundCollision == false)
		{
			//some commonly used terms:
			glm::vec3 velocity = colPackage->velocity;
			glm::vec3 base = colPackage->basePoint;
			float velocitySquaredLength = glm::length(velocity) * glm::length(velocity);
			float a, b, c; //Params for equation
			float newT;

			//For each vertex or edge a quadratic equation have to
			// be solved. We parametrize this equation as
			// a*t^2 + b*t + c = 0 and below we calculate the
			// parameters a,b and c for each test.

			//Check against points:
			a = velocitySquaredLength;

			//P1
			b = 2.0f * (glm::dot<3, float, glm::qualifier::highp>(velocity, base - p1));
			c = glm::length(p1 - base) * glm::length(p1 - base) - 1.0f;
			if (getLowestRoot(a, b, c, t, &newT))
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p1; 
			}

			//P2
			b = 2.0f * (glm::dot<3, float, glm::qualifier::highp>(velocity, base - p2));
			c = glm::length(p2 - base) * glm::length(p2 - base) - 1.0f;
			if (getLowestRoot(a, b, c, t, &newT))
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p2;
			}

			//P3
			b = 2.0f * (glm::dot<3, float, glm::qualifier::highp>(velocity, base - p3));
			c = glm::length(p3 - base) * glm::length(p3 - base) - 1.0f;
			if (getLowestRoot(a, b, c, t, &newT))
			{
				t = newT;
				foundCollision = true;
				collisionPoint = p3;
			}

			//Check against edges:

			// p1 -> p2:
			glm::vec3 edge = p2 - p1;
			glm::vec3 baseToVertex = p1 - base;
			float edgeSquaredLength = glm::length(edge) * glm::length(edge);
			float edgeDotVelocity = glm::dot<3, float, glm::qualifier::highp>(edge, velocity);
			float edgeDotBaseToVertex= glm::dot<3, float, glm::qualifier::highp>(edge, baseToVertex);

			  //Calculate parameters for equation
			a = edgeSquaredLength * (-velocitySquaredLength) + 
				edgeDotVelocity * edgeDotVelocity;
			b = edgeSquaredLength * 2 * glm::dot<3, float, glm::qualifier::highp>(velocity, baseToVertex) - 
				2.0f * edgeDotVelocity * edgeDotBaseToVertex;
			c = edgeSquaredLength * (1 - glm::length(baseToVertex) * glm::length(baseToVertex)) +
				edgeDotBaseToVertex * edgeDotBaseToVertex;

			  //Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a, b, c, t, &newT))
			{
				//Check if intersection is withing line segment:
				float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) /
					edgeSquaredLength;
				if (f >= 0.0f && f <= 1.0f)
				{
					//Intersection took place within segment.
					t = newT;
					foundCollision = true;
					collisionPoint = p1 + f * edge;
				}
			}

			// p2 -> p3:
			edge = p3 - p2;
			baseToVertex = p2 - base;
			edgeSquaredLength = glm::length(edge) * glm::length(edge);
			edgeDotVelocity = glm::dot<3, float, glm::qualifier::highp>(edge, velocity);
			edgeDotBaseToVertex = glm::dot<3, float, glm::qualifier::highp>(edge, baseToVertex);

			  //Calculate parameters for equation
			a = edgeSquaredLength * (-velocitySquaredLength) +
				edgeDotVelocity * edgeDotVelocity;
			b = edgeSquaredLength * 2 * glm::dot<3, float, glm::qualifier::highp>(velocity, baseToVertex) -
				2.0f * edgeDotVelocity * edgeDotBaseToVertex;
			c = edgeSquaredLength * (1 - glm::length(baseToVertex) * glm::length(baseToVertex)) +
				edgeDotBaseToVertex * edgeDotBaseToVertex;

			  //Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a, b, c, t, &newT))
			{
				//Check if intersection is withing line segment:
				float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) /
					edgeSquaredLength;
				if (f >= 0.0f && f <= 1.0f)
				{
					//Intersection took place within segment.
					t = newT;
					foundCollision = true;
					collisionPoint = p2 + f * edge;
				}
			}

			// p3 -> p1:
			edge = p1 - p3;
			baseToVertex = p3 - base;
			edgeSquaredLength = glm::length(edge) * glm::length(edge);
			edgeDotVelocity = glm::dot<3, float, glm::qualifier::highp>(edge, velocity);
			edgeDotBaseToVertex = glm::dot<3, float, glm::qualifier::highp>(edge, baseToVertex);

			  //Calculate parameters for equation
			a = edgeSquaredLength * (-velocitySquaredLength) +
				edgeDotVelocity * edgeDotVelocity;
			b = edgeSquaredLength * 2 * glm::dot<3, float, glm::qualifier::highp>(velocity, baseToVertex) -
				2.0f * edgeDotVelocity * edgeDotBaseToVertex;
			c = edgeSquaredLength * (1 - glm::length(baseToVertex) * glm::length(baseToVertex)) +
				edgeDotBaseToVertex * edgeDotBaseToVertex;

			  //Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a, b, c, t, &newT))
			{
				//Check if intersection is withing line segment:
				float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) /
					edgeSquaredLength;
				if (f >= 0.0f && f <= 1.0f)
				{
					//Intersection took place within segment.
					t = newT;
					foundCollision = true;
					collisionPoint = p3 + f * edge;
				}
			}
		}

		//Set result:
		if (foundCollision == true)
		{
			//Distance to collision: 't' is time of collision
			float distToCollision = t * glm::length(colPackage->velocity);

			

			//Does this triangle quality for the closest hit?
			// it does if it's the first hit or the closet
			if (colPackage->foundCollision == false ||
				distToCollision < colPackage->nearestDistance)
			{
				//Collision information necessary for sliding
				colPackage->nearestDistance = distToCollision;
				colPackage->intersectionPoint = collisionPoint;
				colPackage->foundCollision = true;

				colPackage->isChanged = true;
			}
		}
	}// if not backface
	else
	{
		//std::cout << "ne ta storona" << "\n";
	}
}