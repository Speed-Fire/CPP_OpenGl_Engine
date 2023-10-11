#pragma once

#include"libs.h"

class Light
{
protected:
	float intensity;
	glm::vec3 color;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	unsigned int type;

public:
	Light(
		float intensity, 
		glm::vec3 color,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular
	)
	{
		this->intensity = intensity;
		this->color = color;

		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;

		this->type = 0;
	}

	~Light()
	{

	}

	//Functions
	virtual void sendToShader(Shader& program, int id) = 0;

	unsigned int getType() const { return this->type; }
};

class PointLight : public Light
{
protected:
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

public:
	PointLight(
		glm::vec3 position, 
		float intensity = 1.f, 
		glm::vec3 color = glm::vec3(1.f),
		glm::vec3 ambient = glm::vec3(1.f),
		glm::vec3 diffuse = glm::vec3(1.f),
		glm::vec3 specular = glm::vec3(1.f),
		float constant = 1.f,
		float linear = 0.045f,
		float quadratic = 0.0075f
	)
		: 
		Light(intensity, color, ambient, diffuse, specular)
	{
		this->position = position;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;

		this->type = 1;
	}

	~PointLight()
	{

	}

	void setPosition(const glm::vec3 position)
	{
		this->position = position;
	}

	virtual void sendToShader(Shader& program, int id) override
	{
		//COLOR PARTY DANCE

		/*this->color.x = (float)sin(glfwGetTime() * 2.0f);
		this->color.y = (float)sin(glfwGetTime() * 0.7f);
		this->color.z = (float)sin(glfwGetTime() * 1.3f);

		this->diffuse = this->color * glm::vec3(0.5f);
		this->ambient = this->diffuse * glm::vec3(0.2f);*/

		//END PARTY

		program.setVec3f(this->position,	("pointLights[" + std::to_string(id) + "].position").c_str());
		program.set1f(this->intensity,		("pointLights[" + std::to_string(id) + "].intensity").c_str());
		program.setVec3f(this->color,		("pointLights[" + std::to_string(id) + "].color").c_str());
		program.set1f(this->constant,		("pointLights[" + std::to_string(id) + "].constant").c_str());
		program.set1f(this->linear,			("pointLights[" + std::to_string(id) + "].linear").c_str());
		program.set1f(this->quadratic,		("pointLights[" + std::to_string(id) + "].quadratic").c_str());

		program.setVec3f(this->ambient,		("pointLights[" + std::to_string(id) + "].ambient").c_str());
		program.setVec3f(this->diffuse,		("pointLights[" + std::to_string(id) + "].diffuse").c_str());
		program.setVec3f(this->specular,	("pointLights[" + std::to_string(id) + "].specular").c_str());
	}
};

class DirectionalLight : public Light
{
protected:
	glm::vec3 direction;

public:
	DirectionalLight(
		glm::vec3 direction,
		float intensity = 1.f,
		glm::vec3 color = glm::vec3(1.f),
		glm::vec3 ambient = glm::vec3(1.f),
		glm::vec3 diffuse = glm::vec3(1.f),
		glm::vec3 specular = glm::vec3(1.f)
	)
		: 
		Light(intensity, color, ambient, diffuse, specular)
	{
		this->direction = direction;

		this->type = 0;
	}

	virtual void sendToShader(Shader& program, int id) override
	{
		program.setVec3f(this->direction,	"dirLight.direction");
		program.set1f(this->intensity,		"dirLight.intensity");
		program.setVec3f(this->color,		"dirLight.color");
		program.setVec3f(this->ambient,		"dirLight.ambient");
		program.setVec3f(this->diffuse,		"dirLight.diffuse");
		program.setVec3f(this->specular,	"dirLight.specular");
	}
};

class SpotLight : public Light
{
protected:
	glm::vec3 position;
	glm::vec3 direction;
	float cuttOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

public:
	SpotLight(
		glm::vec3 position,
		glm::vec3 direction,
		float cuttOff,
		float outerCutOff,
		float intensity = 1.f,
		glm::vec3 color = glm::vec3(1.f),
		glm::vec3 ambient = glm::vec3(1.f),
		glm::vec3 diffuse = glm::vec3(1.f),
		glm::vec3 specular = glm::vec3(1.f),
		float constant = 1.f,
		float linear = 0.045f,
		float quadratic = 0.0075f
	)
		:
		Light(intensity, color, ambient, diffuse, specular)
	{
		this->position = position;
		this->direction = direction;

		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;

		if (cuttOff <= 360.f && cuttOff >= 0.f)
		{
			this->cuttOff = cuttOff;
		}
		else
		{
			this->cuttOff = cuttOff;

			while (this->cuttOff > 360.f)
				this->cuttOff -= 360.f;
			while (this->cuttOff < 0.f)
				this->cuttOff += 360.f;
		}

		if (outerCutOff <= 360.f && outerCutOff >= 0.f)
		{
			this->outerCutOff = outerCutOff;
		}
		else
		{
			this->outerCutOff = outerCutOff;

			while (this->outerCutOff > 360.f)
				this->outerCutOff -= 360.f;
			while (this->outerCutOff < 0.f)
				this->outerCutOff += 360.f;
		}

		if (this->cuttOff > this->outerCutOff)
		{
			float temp = this->cuttOff;
			this->cuttOff = this->outerCutOff;
			this->outerCutOff = temp;
		}

		this->type = 2;
	}

	virtual void sendToShader(Shader& program, int id) override
	{
		//COLOR PARTY DANCE

		this->color.x = (float)sin(glfwGetTime() * 2.0f);
		this->color.y = (float)sin(glfwGetTime() * 0.7f);
		this->color.z = (float)sin(glfwGetTime() * 1.3f);

		this->diffuse = this->color * glm::vec3(0.5f);
		this->ambient = this->diffuse * glm::vec3(0.2f);
		this->specular = this->diffuse * glm::vec3(0.6f);

		//END PARTY

		program.setVec3f(this->position,	("spotLights[" + std::to_string(id) + "].position").c_str());
		program.setVec3f(this->direction,	("spotLights[" + std::to_string(id) + "].direction").c_str());
		program.set1f(this->cuttOff,		("spotLights[" + std::to_string(id) + "].cutOff").c_str());
		program.set1f(this->outerCutOff,	("spotLights[" + std::to_string(id) + "].outerCutOff").c_str());

		program.set1f(this->intensity,		("spotLights[" + std::to_string(id) + "].intensity").c_str());
		program.setVec3f(this->color,		("spotLights[" + std::to_string(id) + "].color").c_str());

		program.set1f(this->constant,		("spotLights[" + std::to_string(id) + "].constant").c_str());
		program.set1f(this->linear,			("spotLights[" + std::to_string(id) + "].linear").c_str());
		program.set1f(this->quadratic,		("spotLights[" + std::to_string(id) + "].quadratic").c_str());

		program.setVec3f(this->ambient,		("spotLights[" + std::to_string(id) + "].ambient").c_str());
		program.setVec3f(this->diffuse,		("spotLights[" + std::to_string(id) + "].diffuse").c_str());
		program.setVec3f(this->specular,	("spotLights[" + std::to_string(id) + "].specular").c_str());
	}
};