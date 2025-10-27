
#ifndef OBJECT_SETTING_H
#define OBJECT_SETTING_H

#include <variant>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"

#include "inputctrl.h"

struct Object_setting;

using ValueVariant = std::variant<
	int,
	float,
	glm::vec2,
	glm::vec3,
	glm::vec4,
	glm::mat3,
	glm::mat4,
	Quaternion,
	std::shared_ptr<Object_setting>,
	std::shared_ptr<Shader>,
	Animation

>;


extern std::unique_ptr<Input_ctrl> inputctrl_global;

struct Object_setting
{

public:
	static int object_num;

	Model object;
	std::string name;
	std::shared_ptr<Shader> shader;
	glm::vec3 object_color = glm::vec3(1.0f, 1.0f, 0.31f);
	bool draw = true;
	std::vector<ValueVariant> attribute;

	std::string path;

	Object_setting(const char* path,const char* name, bool gamma = false) : object(path, gamma), name(name), path(path)
	{}
	//// to deep copy
	explicit Object_setting(const Object_setting& obj, int object_count, std::shared_ptr<Shader> shader) : object(obj.object)
	{

		this->name = "object";
		this->name.append(std::to_string(object_count));
		this->shader = shader;

	}


	/// make empty Object
	Object_setting(int object_count, std::shared_ptr<Shader> shader)
	{
		this->name = "object";
		this->name.append(std::to_string(-1));
		this->shader = shader;

		Model empty_model;
		this->object = empty_model;
	}

	Object_setting(std::shared_ptr<Shader> shader)
	{
		this->name = "object";
		this->name.append(std::to_string(-1));
		this->shader = shader;

		Mesh empty_mesh;
		Model empty_model(empty_mesh);
		this->object = empty_model;
	}
	void wolrd_object_Draw(
		const glm::mat4& projection,
		const glm::mat4& view,
		const glm::vec3& camera_position)
	{
		object.set_mesh();

		glm::mat4 model = object.get_model();
		if (this->shader->name == "skin" && object.final_bone_matrix.size() >0)
		{
			this->shader->use();
			this->shader->setMat4("projection", projection);
			this->shader->setMat4("view", view);


			for (int i = 0; i < object.final_bone_matrix.size(); ++i)
			{

				this->shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", object.final_bone_matrix[i]);
				debug.set_axis("final bone", object.final_bone_matrix[i]);
			}


			// render the loaded model

			this->shader->setMat4("model", model);
			object.Draw(*shader);
		}
		else 
		{
			this->shader->use();
			glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
			//stbi_set_flip_vertically_on_load(false);
			//this->shader->setVec3("objectColor", object_color);
			this->shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			this->shader->setVec3("lightPos", lightPos);
			this->shader->setVec3("viewPos", camera_position);

			this->shader->setMat4("projection", projection);
			this->shader->setMat4("view", view);
			this->shader->setMat4("model", model);

			object.Draw(*shader);

		}


	}

	void change_color()
	{
		//object_color = inputctrl_global->color;
	}


};










#endif
