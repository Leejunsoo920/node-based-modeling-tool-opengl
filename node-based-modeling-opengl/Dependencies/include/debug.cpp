
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"

#include "debug.h"




void DebugUI::set_debugUI_buffer()
{
	// set buffer that using instancing  this function running once, after make opengl context and before enter rendering roop
	glGenBuffers(1, &point_instance_buffer);
	glGenBuffers(1, &arrow_instance_buffer);
	glGenBuffers(1, &axis_instance_buffer);
	glGenBuffers(1, &line_instance_buffer);


	glGenBuffers(1, &point_color_buffer);
	glGenBuffers(1, &arrow_color_buffer);
	glGenBuffers(1, &line_color_buffer);


}

void DebugUI::set_point_group(
	const std::string& name,
	const glm::vec3& point,
	const glm::vec3& color,
	const float& size
)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, point);


	auto now_name = find(point_name_list.begin(), point_name_list.end(), name);
	if (now_name == point_name_list.end())
	{

		model = glm::scale(model, glm::vec3(size));
		std::shared_ptr<DebugUI_group> now_DebugUI_group = std::make_shared<DebugUI_group>();


		now_DebugUI_group->name = name;
		now_DebugUI_group->color = color;
		now_DebugUI_group->size = size;


		point_group_list.push_back(now_DebugUI_group);
		point_name_list.push_back(name);


		point_model_M.push_back(model);
		point_color_M.push_back(now_DebugUI_group->color);


	}
	else
	{
		int now_name_num = now_name - point_name_list.begin();

		if (point_group_list[now_name_num]->use)
		{
			model = glm::scale(model, glm::vec3(point_group_list[now_name_num]->size));
			point_model_M.push_back(model);
			point_color_M.push_back(point_group_list[now_name_num]->color);
		}



	}



}


void DebugUI::set_arrow_group(
	const std::string& name,
	const glm::vec3& arrow_start,
	const glm::vec3& arrow_end,
	const glm::vec3& color,
	const float& size
)
{

	auto now_name = find(arrow_name_list.begin(), arrow_name_list.end(), name);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, arrow_start);
	float length = glm::distance(arrow_start, arrow_end);
	if (abs(length) < 0.0001)
	{
		length = 0.0001;
	}
	glm::vec3 local_end = arrow_end - arrow_start;
	Quaternion rotation;
	if (glm::length(local_end) < 0.0001)
	{
		rotation = Quaternion(0, 0, 0, 0);
	}
	else
	{
		rotation = Quaternion(local_end);
	}
	glm::mat4 rotation_T = rotation.quaternion_to_r_matrix();

	model = model * rotation_T;





	if (now_name == arrow_name_list.end())
	{
		std::shared_ptr<DebugUI_arrow> now_arrow_group = std::make_shared<DebugUI_arrow>();

		now_arrow_group->name = name;
		now_arrow_group->color = color;
		now_arrow_group->size = size;

		arrow_group_list.emplace_back(now_arrow_group);
		arrow_name_list.emplace_back(name);


		model = glm::scale(model, glm::vec3(0.07 * size, length, 0.07 * size));

		arrow_model_M.emplace_back(model);
		arrow_color_M.emplace_back(color);

	}
	else
	{

		model = glm::scale(model, glm::vec3(0.07 * size, length, 0.07 * size));
		int now_name_num = now_name - arrow_name_list.begin();

		if (arrow_group_list[now_name_num]->use)
		{
			arrow_model_M.emplace_back(model);
			arrow_color_M.emplace_back(arrow_group_list[now_name_num]->color);
		}


	}
}
void DebugUI::set_line_group(
	const std::string& name,
	const glm::vec3& line_start,
	const glm::vec3& line_end,
	const glm::vec3& color,
	const float& size
)
{
	auto now_name = find(line_name_list.begin(), line_name_list.end(), name);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, line_start);
	float length = glm::distance(line_start, line_end);
	if (abs(length) < 0.0001)
	{
		//length = 0.0001;
	}
	glm::vec3 local_end = line_end - line_start;

	Quaternion rotation;
	if (glm::length(local_end) < 0.0001)
	{
		rotation = Quaternion(0, 0, 0, 0);
	}
	else
	{
		rotation = Quaternion(local_end);
	}
	rotation = Quaternion(local_end);
	glm::mat4 rotation_T = rotation.quaternion_to_r_matrix();
	model = model * rotation_T;


	if (now_name == line_name_list.end())
	{
		std::shared_ptr<DebugUI_line> now_line_group = std::make_shared<DebugUI_line>();

		now_line_group->name = name;
		now_line_group->color = color;
		now_line_group->size = size;

		line_group_list.emplace_back(now_line_group);
		line_name_list.emplace_back(name);

		model = glm::scale(model, glm::vec3(0.07 * size, length, 0.07 * size));

		line_model_M.emplace_back(model);
		line_color_M.emplace_back(color);

	}
	else
	{

		model = glm::scale(model, glm::vec3(0.07 * size, length, 0.07 * size));
		int now_name_num = now_name - line_name_list.begin();

		if (line_group_list[now_name_num]->use)
		{
			line_model_M.emplace_back(model);
			line_color_M.emplace_back(line_group_list[now_name_num]->color);
		}


	}
}

void DebugUI::set_parameter(
	const std::string& name,
	float& parameter,
	const float start_float,
	const float end_float
)
{

	auto now_name = parameter_float_map.find(name);

	if (now_name == parameter_float_map.end())
	{
		parameter_float_map[name] = std::vector<float>{ parameter, start_float, end_float };
	}
	else
	{
		parameter = now_name->second[0];

	}

}
void DebugUI::set_parameter(
	const std::string& name,
	int& parameter,
	const int start_int,
	const int end_int
)
{
	auto now_name = parameter_int_map.find(name);

	if (now_name == parameter_int_map.end())
	{
		parameter_int_map[name] = std::vector<int>{ parameter, start_int, end_int };
	}
	else
	{
		parameter = now_name->second[0];

	}
}



void DebugUI::set_axis(
	const std::string& name,
	const glm::mat4& model,
	const float& size
)
{
	auto now_name = find(axis_name_list.begin(), axis_name_list.end(), name);
	if (now_name == axis_name_list.end())
	{
		std::shared_ptr<DebugUI_axis> now_axis_group = std::make_shared<DebugUI_axis>();

		now_axis_group->name = name;
		now_axis_group->size = size;

		axis_group_list.push_back(now_axis_group);

		axis_name_list.push_back(name);

		glm::mat4 this_model = glm::scale(model, glm::vec3(size));

		axis_model_M.push_back(this_model);




	}
	else
	{
		int now_name_num = now_name - axis_name_list.begin();

		if (axis_group_list[now_name_num]->use)
		{
			glm::mat4 this_model = glm::scale(model, glm::vec3(axis_group_list[now_name_num]->size));

			axis_model_M.push_back(this_model);

		}


	}


}

void DebugUI::set_bone_axis(
	const std::string& name,
	const glm::mat4& model,
	const float& size
)
{
	auto now_name = bone_axis_group_map.find(name);

	if (now_name == bone_axis_group_map.end())
	{
		std::shared_ptr<DebugUI_axis> now_axis_group = std::make_shared<DebugUI_axis>();

		now_axis_group->name = name;
		now_axis_group->size = size;

		bone_axis_group_map[name] = now_axis_group;


		glm::mat4 this_model = glm::scale(model, glm::vec3(size));

		axis_model_M.push_back(this_model);




	}
	else
	{

		if (bone_axis_group_map[now_name->first]->use)
		{
			glm::mat4 this_model = glm::scale(model, glm::vec3(bone_axis_group_map[now_name->first]->size));

			axis_model_M.push_back(this_model);

		}


	}

}


void DebugUI::set_text(
	const std::string& name,
	const float& num
)
{
	ImGui::Begin("debug text");
	ImGui::Text((name + " %.5f").c_str(), num);
	ImGui::End();
}
void DebugUI::set_text(
	const std::string& name,
	const glm::vec2& point
)
{
	ImGui::Begin("debug text");
	ImGui::Text((name + " %.6f , %.6f , %.3f").c_str(), point.x, point.y);
	ImGui::End();
}

void DebugUI::set_text(
	const std::string& name,
	const glm::vec3& point
)
{
	ImGui::Begin("debug text");
	ImGui::Text((name + " %.6f , %.6f , %.3f").c_str(), point.x, point.y, point.z);
	ImGui::End();
}

void DebugUI::set_text(
	const std::string& name,
	const glm::vec4& point
)
{
	ImGui::Begin("debug text");
	ImGui::Text((name + " %.3f , %.3f , %.3f, %.3f").c_str(), point.x, point.y, point.z, point.w);
	ImGui::End();
}

void DebugUI::set_text(
	const std::string& name
)
{
	ImGui::Begin("debug text");
	ImGui::Text((name).c_str());
	ImGui::End();
}

void DebugUI::debug_imgui_text(

)
{
	ImGui::Begin("debug text");
	ImGui::End();

	ImGui::Begin("debug parameter");

	for (auto it = parameter_float_map.begin(); it != parameter_float_map.end(); it++)
	{
		ImGui::SliderFloat((it->first).c_str(), &it->second[0], it->second[1], it->second[2]);


	}
	for (auto it = parameter_int_map.begin(); it != parameter_int_map.end(); it++)
	{
		ImGui::SliderInt((it->first).c_str(), &it->second[0], it->second[1], it->second[2]);


	}


	ImGui::End();
}



void DebugUI::debug_imgui_render(
	const glm::vec3& color)
{

	ImGuiTabBarFlags tab_bar_flags2 = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar_new", tab_bar_flags2))
	{
		if (ImGui::BeginTabItem("point"))
		{
			for (unsigned int i = 0; i < point_group_list.size(); i++)
			{
				ImGui::Checkbox((point_group_list[i]->name).c_str(), &point_group_list[i]->use);
				if (ImGui::Button(("change color##" + point_group_list[i]->name).c_str()))
				{
					point_group_list[i]->color = color;
				}
				ImGui::SliderFloat(("##size ##" + point_group_list[i]->name).c_str(), &point_group_list[i]->size, 0.01, 0.5);

			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("arrow"))
		{
			for (unsigned int i = 0; i < arrow_group_list.size(); i++)
			{

				ImGui::Checkbox((arrow_group_list[i]->name).c_str(), &arrow_group_list[i]->use);
				if (ImGui::Button(("change color##" + arrow_group_list[i]->name).c_str()))
				{
					arrow_group_list[i]->color = color;
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Line"))
		{
			for (unsigned int i = 0; i < line_group_list.size(); i++)
			{

				ImGui::Checkbox((line_group_list[i]->name).c_str(), &line_group_list[i]->use);
				if (ImGui::Button(("change color##" + line_group_list[i]->name).c_str()))
				{
					line_group_list[i]->color = color;
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("axis"))
		{
			for (unsigned int i = 0; i < axis_group_list.size(); i++)
			{
				ImGui::Checkbox((axis_group_list[i]->name + "##check##").c_str(), &axis_group_list[i]->use);
				ImGui::SliderFloat(("##slider##" + axis_group_list[i]->name).c_str(), &axis_group_list[i]->size, 0.01, 1);

			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("bone"))
		{
			for (auto it = bone_axis_group_map.begin(); it != bone_axis_group_map.end(); it++)
			{
				ImGui::Checkbox((it->second->name + "##check##").c_str(), &it->second->use);
				ImGui::SliderFloat(("##slider##" + it->second->name).c_str(), &it->second->size, 0.01, 1);

			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	debug_imgui_text();




}


unsigned int DebugUI::point_instance_buffer;
unsigned int DebugUI::point_color_buffer;
std::vector<glm::mat4> DebugUI::point_model_M;
std::vector<glm::vec3> DebugUI::point_color_M;

unsigned int DebugUI::arrow_instance_buffer;
unsigned int DebugUI::arrow_color_buffer;
std::vector<glm::mat4> DebugUI::arrow_model_M;
std::vector<glm::vec3> DebugUI::arrow_color_M;

unsigned int DebugUI::line_instance_buffer;
unsigned int DebugUI::line_color_buffer;
std::vector<glm::mat4> DebugUI::line_model_M;
std::vector<glm::vec3> DebugUI::line_color_M;

unsigned int DebugUI::axis_instance_buffer;
std::vector<glm::mat4> DebugUI::axis_model_M;


std::vector<std::shared_ptr<DebugUI_group>> DebugUI::point_group_list;
std::vector<std::string> DebugUI::point_name_list;

std::vector<std::shared_ptr<DebugUI_arrow>> DebugUI::arrow_group_list;
std::vector<std::string> DebugUI::arrow_name_list;

std::vector<std::shared_ptr<DebugUI_line>> DebugUI::line_group_list;
std::vector<std::string> DebugUI::line_name_list;

std::vector<std::shared_ptr<DebugUI_axis>> DebugUI::axis_group_list;
std::vector<std::string> DebugUI::axis_name_list;

std::map<std::string, std::shared_ptr<DebugUI_axis>> DebugUI::bone_axis_group_map;


std::map<std::string, std::vector<float>> DebugUI::parameter_float_map;
std::map<std::string, std::vector<int>> DebugUI::parameter_int_map;


DebugUI debug;
