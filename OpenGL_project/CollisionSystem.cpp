#include "CollisionSystem.h"
#include "Model.h"
#include "CharacterEntity.h"

void CollisionSystem::doCollision(CollisionPacket* colPack)
{
	if (this->sceneObjs == nullptr)
		throw "sceneObjs is nullptr";

	for (size_t i = 0; i < this->sceneObjs->size(); i++)
	{
		if (this->sceneObjs->at(i) == colPack->owner)
		{
			//std::cout << "here\n";
			continue;
		}

		if (auto temp = dynamic_cast<Model*>(this->sceneObjs->at(i)))
		{
			auto meshesPtr = temp->getMeshes();

			for (size_t j = 0; j < meshesPtr->size(); j++)
			{
				if (!meshesPtr->at(j)->IsColliding())
					continue;

				colPack->isChanged = false;

				//получить все вертексы меша и индексы их соединения в треугольники;
				//первести координаты вертексов в пространство епсилоида;
				//вызвать функцию обнаружения коллизии для каждого треугольника.

				unsigned int nrOfVert = 0;
				glm::vec3 meshPosition = meshesPtr->at(j)->getPosition();
				glm::vec3 meshRotation = meshesPtr->at(j)->getRotation();
				glm::vec3 meshScale = meshesPtr->at(j)->getScale();
				Vertex* vertices = meshesPtr->at(j)->getVertices(&nrOfVert);

				//Vertex turning around
				glm::mat4 transf = glm::mat4(1.f);
				transf = glm::rotate(transf, glm::radians(meshRotation.x), glm::vec3(1.f, 0.f, 0.f));
				transf = glm::rotate(transf, glm::radians(meshRotation.y), glm::vec3(0.f, 1.f, 0.f));
				transf = glm::rotate(transf, glm::radians(meshRotation.z), glm::vec3(0.f, 0.f, 1.f));
				transf = glm::scale(transf, meshScale);

				for (size_t k = 0; k < nrOfVert; k = k + 3)
				{
					glm::vec3 p1 = vertices[k].position + meshPosition;
					p1 = glm::vec4(p1, 1.f) * transf;
					p1 = p1 / colPack->eRadius;

					glm::vec3 p2 = vertices[k + 1].position + meshPosition;
					p2 = glm::vec4(p2, 1.f) * transf;
					p2 = p2 / colPack->eRadius;

					glm::vec3 p3 = vertices[k + 2].position + meshPosition;
					p3 = glm::vec4(p3, 1.f) * transf;
					p3 = p3 / colPack->eRadius;

					checkTriangle(colPack, p1, p2, p3);
				}
			}

			if (colPack->isChanged)
			{
				if (colPack->foundCollision == true && colPack->nearestDistance <= 0.1f)
					colPack->hitObj = temp;
				else
					colPack->hitObj = nullptr;
			}
		}
	}
}