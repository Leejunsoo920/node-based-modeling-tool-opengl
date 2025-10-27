#ifndef DEBUG_H
#define DEBUG_H

#include "mesh.h"


#include "object.h"


#include <map>


struct DebugUI_group
{

	std::string name;
	glm::vec3 color;

	bool use = true;
	int ui_num = 0;
	float size = 0.03;


};

struct DebugUI_arrow
{

	std::string name;
	glm::vec3 color;

	float size = 1;
	bool use = true;
	int ui_num = 0;

};

struct DebugUI_axis
{
	std::string name;

	float size;
	bool use = true;
	int ui_num = 0;
};
struct DebugUI_line
{

	std::string name;
	glm::vec3 color;

	float size = 1;
	bool use = true;
	int ui_num = 0;

};





class DebugUI
{
public:


	static unsigned int point_instance_buffer;
	static std::vector<glm::mat4> point_model_M;
	static unsigned int point_color_buffer;
	static std::vector<glm::vec3> point_color_M;


	static unsigned int arrow_instance_buffer;
	static std::vector<glm::mat4> arrow_model_M;
	static unsigned int arrow_color_buffer;
	static std::vector<glm::vec3> arrow_color_M;

	static unsigned int line_instance_buffer;
	static std::vector<glm::mat4> line_model_M;
	static unsigned int line_color_buffer;
	static std::vector<glm::vec3> line_color_M;


	static unsigned int axis_instance_buffer;
	static std::vector<glm::mat4> axis_model_M;

	static std::vector<std::shared_ptr<DebugUI_group>> point_group_list;
	static std::vector<std::string> point_name_list;

	static std::vector<std::shared_ptr<DebugUI_arrow>> arrow_group_list;
	static std::vector<std::string> arrow_name_list;

	static std::vector<std::shared_ptr<DebugUI_line>> line_group_list;
	static std::vector<std::string> line_name_list;

	static std::vector<std::shared_ptr<DebugUI_axis>> axis_group_list;
	static std::vector<std::string> axis_name_list;

	static std::map<std::string, std::shared_ptr<DebugUI_axis>> bone_axis_group_map;

	static std::map<std::string, std::vector<float>> parameter_float_map;
	static std::map<std::string, std::vector<int>> parameter_int_map;



	DebugUI() {}
	~DebugUI() {}

	void set_debugUI_buffer();

	void set_point_group(
		const std::string& name,
		const glm::vec3& point,
		const glm::vec3& color = glm::vec3(0.0, 1.0, 0.0),
		const float& size = 0.03
	);
	void set_arrow_group(
		const std::string& name,
		const glm::vec3& arrow_start,
		const glm::vec3& arrow_end,
		const glm::vec3& color = glm::vec3(0.0, 1.0, 0.0),
		const float& size = 1.0
	);
	void set_line_group(
		const std::string& name,
		const glm::vec3& line_start,
		const glm::vec3& line_end,
		const glm::vec3& color = glm::vec3(0.0, 1.0, 0.0),
		const float& size = 1.0
	);

	void set_parameter(
		const std::string& name,
		float& parameter,
		const float start_float,
		const float end_float
	);
	void set_parameter(
		const std::string& name,
		int& parameter,
		const int start_int,
		const int end_int
	);

	void set_axis(
		const std::string& name,
		const glm::mat4& model,
		const float& size = 0.05f
	);
	void set_bone_axis(
		const std::string& name,
		const glm::mat4& model,
		const float& size = 0.05f
	);

	void set_text(
		const std::string& name,
		const float& num
	);
	void set_text(
		const std::string& name,
		const glm::vec2& num
	);
	void set_text(
		const std::string& name,
		const glm::vec3& point
	);
	void set_text(
		const std::string& name,
		const glm::vec4& point
	);
	void set_text(
		const std::string& name
	);

	void debug_imgui_text();




	void debug_imgui_render(
		const glm::vec3& color);

};



extern DebugUI debug;

#endif