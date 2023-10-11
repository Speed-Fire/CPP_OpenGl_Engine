#include "CharacterEntity.h"
#include "CollisionSystem.h"

void CharacterEntity::getFrontAndRightVec()
{
	this->front = this->camera.getFrontVec();
	this->front.y = 0.f;
	this->right = this->camera.getRightVec();
}

CharacterEntity::CharacterEntity(
	GLFWwindow* window, 
	Model* model, 
	glm::vec3 position, 
	glm::vec3 rotation, 
	glm::vec3 scale
)
	:
	PhysicalObject(),
	camera(position, position, glm::vec3(0.f, 1.f, 0.f))
{
	this->window = window;

	this->model = model;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;

	this->front = glm::vec3(0.f);
	this->right = glm::vec3(0.f);
	this->movementSpeed = 3.f;

	this->setPhysicalStatus(true);

	this->camera.move(glm::vec3(0.f, 0.7f, 0.f));

	if (this->model != nullptr)
	{
		this->model->setPosition(this->position);
		this->model->setRotation(this->rotation);
		this->model->setScale(this->scale);
	
		this->model->setCollisionStatus(false);
		this->model->setPhysicalStatus(false);

		for (size_t i = 0; i < this->model->getMeshes()->size(); i++)
			this->model->getMeshes()->at(i)->setColliding(false);
	}

	this->getFrontAndRightVec();

	Listener* handl1 = new MethodEventHandler<CharacterEntity>(this, &CharacterEntity::Tick);
	Listener* handl2 = new MethodEventHandler<CharacterEntity>(this, &CharacterEntity::BeginPlay);
	EventSystem::Instance()->RegisterClient("TICK", handl1);
	EventSystem::Instance()->RegisterClient("BEGINPLAY", handl2);
}

CharacterEntity::~CharacterEntity()
{
}

const glm::mat4 CharacterEntity::getViewMatrix()
{
	return this->camera.getViewMatrix();
}

const glm::vec3 CharacterEntity::getCamPosition()
{
	return this->camera.getPosition();
}

void CharacterEntity::setPosition(glm::vec3 newPosition)
{
	this->position = newPosition;
	this->camera.setPosition(newPosition + glm::vec3(0.f, 0.7f, 0.f));

	if (this->model != nullptr)
	{
		for (size_t i = 0; i < this->model->getMeshes()->size(); i++)
		{
			this->model->getMeshes()->at(i)->setOrigin(newPosition);
			this->model->getMeshes()->at(i)->setPosition(newPosition);
		}
	}
}

void CharacterEntity::setRotation(glm::vec3 newRotation)
{
}

void CharacterEntity::setScale(glm::vec3 newScale)
{
}

void CharacterEntity::move(glm::vec3 newPosition)
{
}

void CharacterEntity::rotate(glm::vec3 newRotation)
{
}

void CharacterEntity::scaleChng(glm::vec3 newScale)
{
}

void CharacterEntity::applyForce(glm::vec3 force)
{
}

void CharacterEntity::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	//Calc offset
	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->mouseY - this->lastMouseY;

	//Set last X and Y
	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;
}

void CharacterEntity::updateKeyboardInput()
{
	//Camera
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->move(FORWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->move(BACKWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->move(LEFT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->move(RIGHT);
	}
}

void CharacterEntity::updateInput()
{
	glfwPollEvents();

	this->updateKeyboardInput();
	this->updateMouseInput();
	this->camera.updateInput(this->dt, -1, this->mouseOffsetX, this->mouseOffsetY);
}

void CharacterEntity::move(const int direction)
{
	//Update position vector
	switch (direction)
	{
	case FORWARD:
		this->isMovingFront += 1;
		break;
	case BACKWARD:
		this->isMovingFront += -1;
		break;
	case LEFT:
		this->isMovingRight += -1;
		break;
	case RIGHT:
		this->isMovingRight += 1;
		break;
	default:
		break;
	}

	if (this->isMovingFront > 1)
		this->isMovingFront = 1;
	else if (this->isMovingFront < -1)
		this->isMovingFront = -1;

	if (this->isMovingRight > 1)
		this->isMovingRight = 1;
	else if (this->isMovingRight < -1)
		this->isMovingRight = -1;
}

void CharacterEntity::moveUpd()
{
	this->getFrontAndRightVec();

	this->velocity = this->front * (float)this->isMovingFront;
	this->velocity += this->right * (float)this->isMovingRight;
	this->rotation = this->right + this->front;
	//this->position += this->velocity * this->movementSpeed * this->dt;

	this->collideAndSlide(this->velocity * this->movementSpeed * this->dt, glm::vec3(0.f, -1.f, 0.f) * this->dt);

	if (this->model != nullptr)
	{
		this->model->move(this->velocity * this->movementSpeed * this->dt);
		float rot = this->camera.getYaw();
		//TODO: заставить работать поворот модели вместе с поворотом камеры
		this->model->setRotation(this->rotation);
	}

	//this->camera.move(this->velocity * this->movementSpeed * this->dt);
	this->camera.setPosition(this->position + glm::vec3(0.f, 0.7f, 0.f));

	this->isMovingFront = 0;
	this->isMovingRight = 0;
}

void CharacterEntity::update()
{
	this->updateInput();
	PhysicalObject::update();
}

void CharacterEntity::render(Shader* shader)
{
	if (this->model != nullptr)
		this->model->render(shader);
}

void CharacterEntity::Tick(Event* event)
{
	PhysicalObject::Tick(event);
}

void CharacterEntity::BeginPlay(Event* event)
{

}

void CharacterEntity::BeginOverlap(Event* event)
{
}
