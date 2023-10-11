#pragma once

#include "Camera.h"

#include <glm.hpp>
#include <glfw3.h>

#include "Model.h"

enum direction { FORWARD = 0, BACKWARD, LEFT, RIGHT };

class CharacterEntity : public PhysicalObject
{
private:
	GLFWwindow* window;

	//Components
	Model* model;
	Camera camera;

	//Movement
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 worldup;
	int isMovingFront;
	int isMovingRight;
	float movementSpeed;


	//Mouse input
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;

protected:
	void getFrontAndRightVec();

public:
	CharacterEntity(GLFWwindow* window, Model* model, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	~CharacterEntity();

	//FUNCTIONS

	//Accessors
	const glm::mat4 getViewMatrix();
	const glm::vec3 getCamPosition();

	//Modifiers
	virtual void setPosition(glm::vec3 position) override;
	virtual void setRotation(glm::vec3 rotation) override;
	virtual void setScale(glm::vec3 scale) override;

	virtual void move(glm::vec3 position) override;
	virtual void rotate(glm::vec3 rotation) override;
	virtual void scaleChng(glm::vec3 scale) override;

	virtual void applyForce(glm::vec3 force) override;

	virtual void update() override;
	virtual void render(Shader* shader) override;

private:
	//Inputs
	void updateMouseInput();
	virtual void updateKeyboardInput();
	void updateInput();
	void move(const int direction);
	virtual void moveUpd() override;

protected:

	//Event Tick:
	virtual void Tick(Event* event) override;

	//Event BeginPlay:
	virtual void BeginPlay(Event* event) override;

	//Event BeginOverlap:
	virtual void BeginOverlap(Event* event) override;

};