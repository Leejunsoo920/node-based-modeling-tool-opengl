#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mymath.h"
class Object
{

public:

	glm::mat4 model = glm::mat4(1.0f);

	Quaternion quaternion = Quaternion(1,0,0,0);
	glm::vec3 position;
	glm::vec3 scale = glm::vec3(1.0);

	glm::vec3 velocity = glm::vec3(0);
	glm::vec3 force = glm::vec3(0);

	Object(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3 velocity = glm::vec3(0.0f),
		glm::vec3 force = glm::vec3(0.0f))
	{
		this-> position = position;
		this-> velocity = position;
		this-> force = position;

	}

	glm::mat4 get_model()
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);
		model = model * quaternion.quaternion_to_r_matrix();
		


		return this->model;

	}



	virtual void vs_seting(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
	{


	}




};














#endif
