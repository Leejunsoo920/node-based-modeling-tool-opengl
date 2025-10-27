#include "Node.h"
#include "imgui_func.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include <ctime>
#include <sstream>
#include <regex>
#include "utility_func.h"

extern Frame_manager frame_manager;

const char* MySequence::SequencerItemTypeNames[5] = { "Float","Position","Rotation","Scale","Animation" }; 

//hash func to using string in swith
// 0xEDB88320 is CRC32 algorithm
constexpr unsigned int hash_table(const char* str)
{
	////check const char* by ternary operator
	return str[0] ? static_cast<unsigned int>(str[0]) + 0xEDB8832Full * hash_table(str + 1) : 8603;//// if str[0] == null -> 8603
}


void Node::imgui_render()
{
	ImGui::BeginChild((name).c_str(), ImVec2(250, 200), ImGuiWindowFlags_NoTitleBar);
	{
		ImGui::Text((name).c_str());
	}
	ImGui::EndChild();
}

bool GraphEditorDelegate::AllowedLink(
	GraphEditor::NodeIndex from,
	GraphEditor::NodeIndex to
)
{
	return true;
}

void GraphEditorDelegate::SelectNode(
	GraphEditor::NodeIndex nodeIndex,
	bool selected
)
{
	mNodes[nodeIndex]->mSelected = selected;
}
void GraphEditorDelegate::MoveSelectedNodes(
	const ImVec2 delta
)
{
	for (auto& node : mNodes)
	{
		if (!node->mSelected)
		{
			continue;
		}
		node->x += delta.x;
		node->y += delta.y;
	}
}
void GraphEditorDelegate::RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput)
{

}
void GraphEditorDelegate::AddLink(
	GraphEditor::NodeIndex inputNodeIndex,
	GraphEditor::SlotIndex inputSlotIndex,
	GraphEditor::NodeIndex outputNodeIndex,
	GraphEditor::SlotIndex outputSlotIndex
)
{
	auto copy_before_mLinks = mLinks;
	mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mLinks = copy_before_mLinks;
	}
} 
void GraphEditorDelegate::DelLink(
	GraphEditor::LinkIndex linkIndex
)
{
	auto copy_before_mLinks = mLinks;
	auto this_node_index = mLinks[linkIndex].mOutputNodeIndex;
	mLinks.erase(mLinks.begin() + linkIndex);
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
		mNodes[this_node_index]->evaluated = false;
	}
	else
	{
		mLinks = copy_before_mLinks;
	}

}
void GraphEditorDelegate::CustomDraw(
	ImDrawList* drawList,
	ImRect rectangle,
	GraphEditor::NodeIndex nodeIndex)
{
	drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
	drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
}
const size_t  GraphEditorDelegate::GetTemplateCount()
{
	return sizeof(GraphEditorDelegate::mTemplates) / sizeof(GraphEditor::Template);
}
const GraphEditor::Template GraphEditorDelegate::GetTemplate(
	GraphEditor::TemplateIndex index
)
{
	return mTemplates[index];
}
const size_t GraphEditorDelegate::GetNodeCount(

)
{
	return mNodes.size();
}
const GraphEditor::Node GraphEditorDelegate::GetNode(
	GraphEditor::NodeIndex index
)
{
	int d = mNodes[index]->templateIndex;
	return GraphEditor::Node
	{
		(mNodes[index]->name).c_str(),
		mNodes[index]->templateIndex,
		ImRect(ImVec2(mNodes[index]->x, mNodes[index]->y), ImVec2(mNodes[index]->x + 200, mNodes[index]->y + 200)),
		mNodes[index]->mSelected
	};
}
const size_t GraphEditorDelegate::GetLinkCount()
{
	return this->mLinks.size();
}


const GraphEditor::Link GraphEditorDelegate::GetLink(
	GraphEditor::LinkIndex index
)
{
	return this->mLinks[index];
}


template <typename T> 
void GraphEditorDelegate::gen_node(ImVec2 nodeRectangleMin, const char* node_name, const T& ani)
{
	auto copy_before_mNodes = mNodes;// Make a copy of the node settings; if an error occurs during checks,// revert to the previous safe state.

	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{
	case hash_table("Float"):
	{
		new_name = "Float";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_float>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("Int"):
	{
		new_name = "Int";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_int>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4"):
	{
		new_name = "mat4";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_mat4>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("add"):
	{
		new_name = "add";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_add>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("result"):
	{
		new_name = "result";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_result>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("subtract"):
	{
		new_name = "subtract";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_subtract>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("divide"):
	{
		new_name = "divide";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_divide>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("multiply"):
	{
		new_name = "multiply";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_multiply>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("vec3"):
	{
		new_name = "vec3";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_vec3>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("line"):
	{
		new_name = "line";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_line>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}

	case hash_table("line_to_global_matrix"):
	{
		new_name = "line_to_global_matrix";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_line_to_global_matrix>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("bezier_curve"):
	{
		new_name = "bezier curve";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_bezier_curve>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("cubic_bezier_curve"):
	{
		new_name = "cubic_bezier_curve";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_cubic_bezier_curve>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	
	case hash_table("sweep"):
	{
		new_name = "sweep";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_sweep>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("select_one_value"):
	{
		new_name = "select_one_value";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_select_one_value>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4_rotate"):
	{
		new_name = "mat4_rotate";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_matrix_rotation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4_z_lookup"):
	{
		new_name = "mat4_z_lookup";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_matrix_z_look_up>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("now_frame"):
	{
		new_name = "now_frame";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_now_frame>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mesh_projection"):
	{
		new_name = "mesh_projection";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_mesh_projection>(new_name, node_count, 2, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}

	case hash_table("rotation"):
	{
		new_name = "rotation";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_rotation>(new_name, node_count, 2, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("translation"):
	{
		new_name = "translation";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_translation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("arrange"):
	{
		new_name = "arrange";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_object_arrange>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("half_two_circles"):
	{
		new_name = "half_two_circles";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_half_two_circles>(new_name, node_count, 2, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("line_to_chain"):
	{
		new_name = "line_to_chain";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_line_to_chain_matrix>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}

	case hash_table("catenary_curve"):
	{
		new_name = "catenary_curve";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_catenary_curve>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("vec3_projection_to_mesh"):
	{
		new_name = "vec3_projection_to_mesh";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_find_projection_point>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("cloth_sim"):
	{
		new_name = "cloth_sim";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_ExplicitEuler_cloth_simulation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}

}

template <typename U>
void GraphEditorDelegate::gen_object_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Object_setting>& object,int& object_count, const U& shader)
{
	auto copy_before_mNodes = mNodes;// Make a copy of the node settings; if an error occurs during checks,// revert to the previous safe state.
	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	debug.set_text("mouse pos", glm::vec2(nodeRectangleMin.x, nodeRectangleMin.y));
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{

	case hash_table("object"):
	{
		if (object != nullptr)
		{
			new_name = "object" + object->name;
			std::shared_ptr<Node> new_node = std::make_shared<Node_object>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, object, object_count, shader);
			mNodes.push_back(std::move(new_node));

			object_count++;

		}

		break;
	}
	
	case hash_table("circle"):
	{
		std::string object_name = "circle";

		new_name = "gen_object" + object_name;
		std::shared_ptr<Node> new_node = std::make_shared<Node_generate_circle_object>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, nullptr, object_count, shader);
		mNodes.push_back(std::move(new_node));
		object_count++;
		break;
	}
	case hash_table("sphere"):
	{

		std::string object_name = "sphere";

		new_name = "gen_object" + object_name;
		std::shared_ptr<Node> new_node = std::make_shared<Node_generate_sphere_object>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, nullptr, object_count, shader);
		mNodes.push_back(std::move(new_node));
		object_count++;
		break;
	}
	


	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}


void GraphEditorDelegate::gen_setting_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Shader> shader = nullptr)
{
	auto copy_before_mNodes = mNodes; // Make a copy of the node settings; if an error occurs during checks,// revert to the previous safe state.
	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	debug.set_text("mouse pos", glm::vec2(nodeRectangleMin.x, nodeRectangleMin.y));
	static int node_count = 0;
	std::string new_name;
	std::string setting = "setting_";
	switch (hash_table(node_name))
	{

	
	case hash_table("output_model"):
	{
		new_name = setting + "output_model";
		std::shared_ptr<Node> new_node = std::make_shared<Node_object_setting>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550,false, shader);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("key_frame_sequence"):
	{
		new_name = setting + "key_frame_sequence";
		std::shared_ptr<Node> new_node = std::make_shared<Node_keyframe_sequence>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	


	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}

void GraphEditorDelegate::gen_shader_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Shader> shader)
{
	auto copy_before_mNodes = mNodes; // Make a copy of the node settings; if an error occurs during checks,// revert to the previous safe state.
	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	debug.set_text("mouse pos", glm::vec2(nodeRectangleMin.x, nodeRectangleMin.y));
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{

	case hash_table("shader"):
	{
		if (shader != nullptr)
		{
			new_name = "shader" + shader->name;
			std::shared_ptr<Node> new_node = std::make_shared<Node_shader>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, shader);
			mNodes.push_back(std::move(new_node));

		}

		break;
	}

	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}

void GraphEditorDelegate::gen_animation_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Animation> animation)
{
	auto copy_before_mNodes = mNodes; // Make a copy of the node settings; if an error occurs during checks,// revert to the previous safe state.
	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	debug.set_text("mouse pos", glm::vec2(nodeRectangleMin.x, nodeRectangleMin.y));
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{

	case hash_table("animation"):
	{
		if (animation != nullptr)
		{
			new_name = "animation" + animation->name;
			std::shared_ptr<Node> new_node = std::make_shared<Node_animation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, animation);
			mNodes.push_back(std::move(new_node));
		}

		break;
	}

	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}


void GraphEditorDelegate::del_node()
{
	auto copy_before_mNodes = mNodes;

	for (auto it = mNodes.begin(); it != mNodes.end(); ) 
	{
		if ((*it)->mSelected)
		{

			int node_index = it - mNodes.begin();

			for (auto link_it = mLinks.begin(); link_it != mLinks.end(); )
			{
				if (link_it->mInputNodeIndex == node_index || link_it->mOutputNodeIndex == node_index)
				{
					link_it = mLinks.erase(link_it);
				}
				else
				{
					++link_it;
				}
			}
			for (auto& link : mLinks)
			{
				if (link.mInputNodeIndex > node_index)
				{
					link.mInputNodeIndex -= 1;
				}
				if (link.mOutputNodeIndex > node_index)
				{
					link.mOutputNodeIndex -= 1;
				}
			}
			it = mNodes.erase(it);  
		}
		else {
			++it; 
		}
	}

	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}

bool GraphEditorDelegate::topological_sort(
	const std::vector<std::shared_ptr<Node>>& mNodes,
	const std::vector<GraphEditor::Link>& mlinks
)
{
	auto copy_before_sorted_graph = this->sorted_graph;

	int node_size = mNodes.size();

	std::vector<std::vector<int>> graph(node_size);
	std::vector<int> inDegree(node_size, 0);

	//// 1. generate graph by link
	for (const auto& link : mlinks)
	{
		int from = link.mOutputNodeIndex;
		int to = link.mInputNodeIndex;

		graph[from].push_back(to);
		inDegree[to]++;
	}

	//// 2. input node that indegree == 0 in queue
	std::queue<int> q;
	for (int i = 0; i < node_size; i++)
	{
		if (inDegree[i] == 0)
			q.push(i);
	}


	//// 3. topological sort
	std::vector<int> sortedOrder;
	while (!q.empty())
	{
		int curr = q.front();
		q.pop();
		sortedOrder.push_back(curr);

		for (int next : graph[curr])
		{
			inDegree[next]--;
			if (inDegree[next] == 0)
			{
				q.push(next);
			}

		}
	}

	//// 4. check cycle ( Cannot execute when nodes form a cycle)
	if (sortedOrder.size() != node_size)
	{
		sortedOrder.clear();
		this->sorted_graph = copy_before_sorted_graph;
		return false;

	}
	else
	{

		std::reverse(sortedOrder.begin(), sortedOrder.end());
		this->sorted_graph = sortedOrder;

		//// clear all node input and output before link setting
		for (const auto& node : mNodes)
		{
			node->input_node.clear();
			node->output_node_num = 0;
		}
		//// set inputnode pointer to after node
		for (const auto& link : mlinks)
		{
			int from = link.mOutputNodeIndex;
			int to = link.mInputNodeIndex;

			int output_slot_num = link.mInputSlotIndex;
			int input_slot_num = link.mOutputSlotIndex;

			//std::cout << input_num << std::endl;

			int now_input_node_list_size =mNodes[from]->input_node.size();

			if(now_input_node_list_size ==input_slot_num)
				mNodes[from]->input_node.push_back(mNodes[to]);
			if (now_input_node_list_size > input_slot_num)
				mNodes[from]->input_node[input_slot_num] = mNodes[to];
			if (now_input_node_list_size < input_slot_num)
			{
				for (unsigned int i = now_input_node_list_size; i < input_slot_num ; i++)
				{
					mNodes[from]->input_node.emplace_back(nullptr);
				}
				mNodes[from]->input_node.push_back(mNodes[to]);

			}
			mNodes[to]->output_node_num++;
		}
		return true;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////

void Node_manager::update(
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3& camera_position)
{
	std::filesystem::current_path(project_path);

	if (inputctrl_global->mesh_fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	debug_UI_draw(projection,view); // // UI debug objects are rendered using instancing for high performance
	imgui_render();
	imgui_node_property_render(projection, view, camera_position);
}

void Node_manager::select_gen_node(objects_map_t& pre_object_map, const ImVec2& mouse_pos_when_click)
{

	//Prevent node creation window from going outside the screen
	ImVec2 using_mouse_pos = mouse_pos_when_click;
	if (mouse_pos_when_click.x > 1400)
	{
		float difference = mouse_pos_when_click.x - 1400;
		using_mouse_pos.x -= difference;

	}


	ImGui::SetNextWindowPos(using_mouse_pos);
	ImGui::Begin("node select");
	{
		

		const char* math_node_names[] = 
		{
			"Float",
			"Int",
			"mat4",
			"add",
			"result",
			"subtract",
			"divide",
			"multiply",
			"vec3",
			"line",
			"line_to_global_matrix",
			"bezier_curve",
			"cubic_bezier_curve",
			"sweep",
			"select_one_value",
			"mat4_rotate",
			"mat4_z_lookup",
			"now_frame",
			"rotation",
			"translation",
			"arrange",
			"half_two_circles",
			"line_to_chain",
			"catenary_curve",
			"vec3_projection_to_mesh",
			"cloth_sim"
		};

		const char* setting_list[] =
		{
			"output_model",
			"key_frame_sequence"
		};



		const char* animation_node_names[] =
		{
			"animation"
		};

		const char* gen_object_names[] =
		{
			"circle",
			"sphere"
		};

		ImVec2 temp = ImVec2(0, 0);
		ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
		ImVec2 nodeRectangleMin = offset + temp * viewState.mFactor;

		std::string selection = "none";
		
		if (button_list("func", selection, math_node_names))
		{
			delegate.gen_node(nodeRectangleMin, (selection).c_str(), nullptr);
		}
		if (button_list_map("animation", selection, pre_animation_map))
		{
			delegate.gen_animation_node(nodeRectangleMin, "animation", pre_animation_map[selection]);
		}
		if (button_list("gen_object", selection, gen_object_names))
		{
			// generate object defined only by code
			delegate.gen_object_node(nodeRectangleMin, (selection).c_str(), nullptr, this->object_count, pre_shader_map["nomal shader"]);
		}
		if (button_list_map("object", selection, pre_object_map))
		{
			// generate imported object node
			delegate.gen_object_node(nodeRectangleMin, "object", pre_object_map[selection], this->object_count, pre_shader_map["nomal shader"]);
		}
		if (button_list_map("shader", selection, pre_shader_map))
		{
			delegate.gen_shader_node(nodeRectangleMin, "shader", pre_shader_map[selection]);
		}
		if (button_list("setting", selection, setting_list))
		{
			delegate.gen_setting_node(nodeRectangleMin, (selection).c_str(), pre_shader_map["nomal shader"]);
		}




	}
	ImGui::End();
}

void Node_manager::imgui_render()
{

	ImGui::Begin("editor controls");
	frame_manager.update();
	frame_manager.frame_manager_imgui_render();

	if ( mySequence.myItems_ptr != nullptr && mySequence.rampEdit != nullptr)
	{
		// let's create the sequencer
		static int selectedEntry = -1;
		static int firstFrame = 0;
		static bool expanded = true;

		static int currentFrame = 100;
		
		
		ImGui::PushItemWidth(130);
		ImGui::InputInt("Frame Min", &mySequence.mFrameMin); 
		ImGui::SameLine();
		ImGui::InputInt("Frame ", &frame_manager.now_frame);
		ImGui::SameLine();
		ImGui::InputInt("Frame Max", &mySequence.mFrameMax);

		if (mySequence.now_select_slot > -1 && mySequence.now_select_slot < (*mySequence.myItems_ptr).size())
		{
			ImGui::SameLine();
			ImGui::InputFloat("min value", &(*mySequence.myItems_ptr)[mySequence.now_select_slot].min_value, 0.1, 1);
			ImGui::SameLine();
			ImGui::InputFloat("max value", &(*mySequence.myItems_ptr)[mySequence.now_select_slot].max_value, 0.1, 1);
		}


		ImGui::PopItemWidth();
		ImSequencer::Sequencer(&mySequence, &frame_manager.now_frame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);
		// add a UI to edit that particular item
		if (selectedEntry != -1)
		{
			const MySequenceItem& item = (*mySequence.myItems_ptr)[selectedEntry];
			ImGui::Text("I am a %s, please edit me", mySequence.SequencerItemTypeNames[item.mType]);
			// switch (type) ....
		}
	}
	if (ImGui::CollapsingHeader("Graph Editor")) 
	{
		ImGui::Checkbox("Show GraphEditor", &showGraphEditor);
		GraphEditor::EditOptions(options);
	}

	ImGui::End();

	debug.set_text("n", glm::vec3(0, 0, 0));


	if (showGraphEditor)
	{
		ImGui::Begin("Graph Editor", NULL, 0);
		if (ImGui::Button("Fit all nodes"))
		{
			fit = GraphEditor::Fit_AllNodes;
		}
		ImGui::SameLine();
		if (ImGui::Button("Fit selected nodes"))
		{
			fit = GraphEditor::Fit_SelectedNodes;
		}
		ImGui::SameLine();

		////node save system 
		ImGui::SameLine();
		if (ImGui::Button("save as"))
		{
			Utility utility;
			std::string save_path = utility.OpenFileDialog();
			current_file_path = save_path;
			Node_manager::save_all_node_by_json(save_path);
		}
		ImGui::SameLine();
		if (ImGui::Button("save"))
		{
			if (std::filesystem::exists(current_file_path))
			{
				// Automatically create a backup file to prevent loss of data 
				// in case the original file is accidentally overwritten during save

				std::filesystem::path file_path(current_file_path);
				
				std::filesystem::path backup_folder = file_path.parent_path() / "backup";
				if (!std::filesystem::exists(backup_folder))
					std::filesystem::create_directory(backup_folder);

				std::time_t t = std::time(nullptr);
				std::tm tm;
				localtime_s(&tm, &t);
				char time_buf[32];
				std::strftime(time_buf, sizeof(time_buf), "%Y%m%d_%H%M%S", &tm);

				std::filesystem::path backup_file = backup_folder / (file_path.stem().string() + "_backup" + time_buf + file_path.extension().string());

				try {
					std::filesystem::copy_file(file_path, backup_file, std::filesystem::copy_options::overwrite_existing);

				}
				catch (std::exception& e)
				{
					std::cout << "failed to backup file" << std::endl;
				}

				// overwritten the current nodes to the file
				Node_manager::save_all_node_by_json(current_file_path);
			}
			else
			{
				Utility utility;
				std::string save_path = utility.OpenFileDialog();
				current_file_path = save_path;
				Node_manager::save_all_node_by_json(save_path);

			}
		}

		ImGui::SameLine();
		////node load system 
		if (ImGui::Button("load node graph"))
		{
			Utility utility;
			std::string load_path = utility.OpenFileDialog(); // get file path
			current_file_path = load_path;
			if (!load_path.empty()) {
				if (load_path.size() >= 5 && load_path.substr(load_path.size() - 5) == ".json")
				{
					Node_manager::load_all_node_by_json(load_path);
				}
				else
				{
					std::cout << "Invalid file type. Please select a .json file." << std::endl;
				}

			}
			else {
				//std::cout << "cancle" << std::endl;
			}



		}


		if (ImGui::Button("new object import"))
		{
			Utility utility;
			std::string path = utility.OpenFileDialog(); // get file path

			std::filesystem::path file_path(path); // saved file

			if (!file_path.is_absolute())
			{
				file_path = project_path / file_path;
			}
			if (std::filesystem::exists(file_path))
			{
				std::string object_name = std::filesystem::path(path).stem().string();
				set_pre_animation((path).c_str(), (object_name).c_str());
			}

		}

		GraphEditor::Show(delegate, options, viewState, true, &fit);

		static ImVec2 mouse_pos_when_click;
		if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ImGui::IsMouseClicked(1))
		{

			if (gen_node_click)
			{
				gen_node_click = false;
			}
			else
			{
				gen_node_click = true;
				mouse_pos_when_click = ImGui::GetMousePos();

			}

		}

		//// gen, delete node
		if (gen_node_click)
		{

			select_gen_node(pre_object_map, mouse_pos_when_click);
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Delete) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
		{
			delegate.del_node();
		}

		ImVec2 mouse_pos = ImGui::GetMousePos();
		debug.set_text("mouse position", glm::vec2(mouse_pos.x, mouse_pos.y));




		ImGui::End();
	}
}



void Node_manager::imgui_node_property_render(
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3& camera_position
)
{
	const ImGuiIO& io = ImGui::GetIO();

	if (delegate.sorted_graph.size() > 0 && delegate.node_changed)
	{
		delegate.mNodes[delegate.sorted_graph[0]]->evaluated = false;
		delegate.node_changed = false;
	}

	// Initializes before traversing nodes.
	mySequence.myItems_ptr = nullptr;
	mySequence.rampEdit = nullptr;



	ImGui::Begin("node property");
	{
		//Run evaluation for all sorted node
		// sorting is already done by topological sort when nodes, links and their value are changed 
		for (auto& graph_index : delegate.sorted_graph)
		{
			delegate.mNodes[graph_index]->evaluate();
			
			for (auto& it : delegate.mNodes[graph_index]->input_node)
			{
				if(it != nullptr)
					it->now_output_count++;
			}
		}
		glEnable(GL_DEPTH_TEST);
		// check node's Draw option for all sorted node and real draw 
		int before_select_graph_index = -1;

		for (auto& graph_index : delegate.sorted_graph)
		{
			// Retrieves and applies the keyframe list and points from nodes that contain keyframes, using pointer references.
			if (delegate.mNodes[graph_index]->have_keyframe_check())
			{
				mySequence.myItems_ptr = delegate.mNodes[graph_index]->return_sequence_Items();
				mySequence.rampEdit = delegate.mNodes[graph_index]->return_sequence_rampEdit();

				ImSequencer::calculate_value(&mySequence, frame_manager.now_frame);


				// Conditional check to display the keyframe sequence of the currently selected node in the UI
				if (!delegate.mNodes[graph_index]->mSelected)
				{
					if (before_select_graph_index == -1)
					{
						mySequence.myItems_ptr = nullptr;
						mySequence.rampEdit = nullptr;
					}
					else
					{
						mySequence.myItems_ptr = delegate.mNodes[before_select_graph_index]->return_sequence_Items();
						mySequence.rampEdit = delegate.mNodes[before_select_graph_index]->return_sequence_rampEdit();
					}

				}
				else
				{
					before_select_graph_index = graph_index;
				}

			}

			// if node draw option is ture and evaluated was done
			if (delegate.mNodes[graph_index]->Draw_check && delegate.mNodes[graph_index]->evaluated)
			{
				// if draw is done, draw func return true
				if (delegate.mNodes[graph_index]->Draw(projection, view, camera_position)&& delegate.mNodes[graph_index]->mSelected)
				{
					Gizmo gizmo;
					//// check vertex num of object by mouse pointing
					//// if select object node, ver3, mat4  , you can select one point by shift + mousedown
					inputctrl_global->check_vertex_num = check_vertex_num(delegate.mNodes[graph_index]->return_vertex_pos()); 

					if (io.MouseClicked[0] && io.KeyShift)
					{
						if (inputctrl_global->check_vertex_num >= 0)
						{
							if (delegate.mNodes[graph_index]->select_vertex_num.size() == 0)
							{
								delegate.mNodes[graph_index]->select_vertex_num.push_back(inputctrl_global->check_vertex_num);
							}
							else
							{
								delegate.mNodes[graph_index]->select_vertex_num.clear();
								delegate.mNodes[graph_index]->select_vertex_num.push_back(inputctrl_global->check_vertex_num);
							}
						}
						gizmo.gizmo_matrix = glm::mat4(1);
						gizmo.origin_matrix.clear();
					}


					// if have select vertex, provide gizmo options
					if (delegate.mNodes[graph_index]->select_vertex_num.size() > 0)
						{
						if (gizmo.origin_matrix.size() == 0)
						{

							gizmo.origin_matrix.push_back(delegate.mNodes[graph_index]->return_transform(delegate.mNodes[graph_index]->select_vertex_num[0]));
							gizmo.gizmo_matrix = gizmo.origin_matrix[0];
							//Currently, the gizmo is provided for a single vertex only,
							//but to support future cases where multiple vertices are selected and manipulated simultaneously using the gizmo, 
							// the (vertex) origin matrix is stored as a vector.
						}

						//calcuate gizmo's translation matrix by user mouse interaction
						if (inputctrl_global->now_gizmo_state == Gizmo_state::TRANSLATE)
						{
							ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
								ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(gizmo.gizmo_matrix));
						}
						else if (inputctrl_global->now_gizmo_state == Gizmo_state::ROTATE)
						{
							ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
								ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(gizmo.gizmo_matrix));
						}
						glm::vec3 gizmo_result = gizmo.gizmo_matrix * glm::vec4(0, 0, 0, 1);
						debug.set_point_group("gizmo result", gizmo_result,glm::vec3(1,0,0),0.035); // draw point that selected vertex position


						
						if (io.MouseDown[0])
						{
							// Safety check: avoid referencing deleted vertex in Gizmo
							if(delegate.mNodes[graph_index]->select_vertex_num[0] < delegate.mNodes[graph_index]->value_vector_size)
							{
								delegate.mNodes[graph_index]->set_transform(gizmo.gizmo_matrix, delegate.mNodes[graph_index]->select_vertex_num[0]);
							}
							else
							{
								delegate.mNodes[graph_index]->select_vertex_num.clear();
								gizmo.origin_matrix.clear();
							}



						}


					}
				
				}
				else
				{
					delegate.mNodes[graph_index]->select_vertex_num.clear();

				}
			}

		}





		for (auto& graph_index : delegate.sorted_graph)
		{
			if (delegate.mNodes[graph_index]->mSelected)
				delegate.mNodes[graph_index]->imgui_render();

		}
		const ImGuiIO& io = ImGui::GetIO();


	}
	ImGui::End();


}



void Node_manager::save_all_node_by_json(std::string save_path)
{
	nlohmann::ordered_json j;

	const auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
#if defined(_WIN32)
	localtime_s(&tm, &now_c);
#else
	localtime_r(&now_c, &tm);
#endif

	std::stringstream time_string;
	time_string << std::put_time(&tm, "%Y%m%d_%H%M%S");

	j["saved_time"] = time_string.str();

	//// 1. get node imgui visualize option
	j["view_position"] = { viewState.mPosition.x, viewState.mPosition.y };
	j["view_factor"] = viewState.mFactor;
	ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
	j["view_offset"] = { offset.x, offset.y };


	//// 2. Check object nodes and save their paths to the save file

	for (auto object : pre_object_map)
	{
		bool check_used_node = false;

		// Checks if the object is actually used in a node, which also verifies if it has a valid path.
		// Note: Using the name for this check may pose potential risks in the future,
		// but since there are no plans for further expansion at this time, it is left as is.
		for (auto& node : delegate.mNodes)
		{
			if (node->name.rfind("object", 0) == 0 && node->name.length() > 6) 
			{
				if (node->name.substr(6) == object.first)
				{
					check_used_node = true;
				}
			}
		}

		if (check_used_node)
		{
			nlohmann::ordered_json j_object;

			j_object["name"] = object.first;
			std::filesystem::path relative_path = std::filesystem::relative(object.second->path, project_path);

			j_object["path"] = relative_path;

			j["object_list"].push_back(j_object);

		}



	}



	//// 3. get nodes option
	for (auto& node : delegate.mNodes)
	{

		nlohmann::ordered_json jnode;



		////classify object(imported model), gen object(are made by system), others
		if (node->name.rfind("object", 0) == 0 && node->name.length() > 6) 
		{
			jnode["name"] = "object";
			jnode["object_name"] = node->name.substr(6);
		}
		else if (node->name.rfind("gen_object", 0) == 0 && node->name.length() > 10) 
		{

			jnode["name"] = "gen_object";
			jnode["object_name"] = node->name.substr(10);
		}
		else if (node->name.rfind("shader", 0) == 0 && node->name.length() > 6)
		{
			jnode["name"] = "shader";
			jnode["shader_name"] = node->name.substr(6);

		}
		else if (node->name.find("setting", 0) == 0 && node->name.length() > 7)
		{
			jnode["name"] = "setting";
			jnode["setting_name"] = node->name.substr(8);
		}
		else if (node->name.find("animation", 0) == 0 && node->name.length() > 9)
		{
			jnode["name"] = "animation";
			jnode["animation_name"] = node->name.substr(9);
		}
		else {
			jnode["name"] = node->name;
		}

		jnode["position"] = { node->x, node->y };
		std::vector<ValueVariant> initial = node->get_initial_value();

		jnode["Draw"] = node->Draw_check;

		if (initial.size() > 0)
			jnode["initial_setting"] = serialize_initial_value(initial);

		if (node->have_keyframe_check())
		{
			std::vector<ValueVariant> key_frame_items = node->get_key_frame_item();
			jnode["key_frame_items"] = serialize_initial_value(key_frame_items);

			std::vector<ValueVariant> key_frame_ramp = node->get_key_frame_rampEdit();
			jnode["key_frame_ramp"] = serialize_initial_value(key_frame_ramp);

		}




		j["nodes"].push_back(jnode);

	}	

	for (auto& link : delegate.mLinks) {
		j["links"].push_back({
			link.mInputNodeIndex,
			link.mInputSlotIndex,
			link.mOutputNodeIndex,
			link.mOutputSlotIndex
			});
	}


	std::stringstream ss;

	if (save_path.size() < 5 || save_path.substr(save_path.size() - 5) != ".json")
	{
		save_path += ".json";
	}

	ss << save_path;

	std::ofstream(ss.str()) << j.dump(4);

}

void Node_manager::load_all_node_by_json(std::string json_file_to_read)
{



	if (json_file_to_read.empty()) {
		return;
	}

	std::ifstream file(json_file_to_read);
	if (!file.is_open()) {
		return;
	}
	nlohmann::json j;
	file >> j;

	pre_object_map.clear();

	delegate.mNodes.clear();
	delegate.mLinks.clear();



	this->object_count = 0;
	ImVec2 offset;
	
	if (j.contains("object_list"))
	{
		for (auto& j_object : j["object_list"])
		{
			std::string object_name = j_object["name"];
			std::string path = j_object["path"];

			std::filesystem::path file_path(path); // saved file

			if (!file_path.is_absolute())
			{
				file_path = project_path/file_path;
			}
			if (std::filesystem::exists(file_path))
			{

				set_pre_animation((file_path.string()).c_str(), (object_name).c_str());

			}
			else
			{
				//std::cout << "object file does not exist" << std::endl;
				return;
			}




		}

	}
	
	
	
	
	if (j.contains("view_position") && j.contains("view_factor") && j.contains("view_offset"))
	{
		viewState.mPosition = ImVec2(j["view_position"][0], j["view_position"][1]);
		viewState.mFactor = j["view_factor"];

		offset = ImVec2(j["view_offset"][0], j["view_offset"][1]);
	}
	ImVec2 nodeRectangleMin;
	for (auto& jnode : j["nodes"])
	{
		std::string name = jnode["name"];
		ImVec2 position = ImVec2(jnode["position"][0], jnode["position"][1]);

		nodeRectangleMin = position;

		if (name == "object" && jnode.contains("object_name")) {

			std::string object_name = jnode["object_name"];

			delegate.gen_object_node(nodeRectangleMin, "object", pre_object_map[object_name], this->object_count, pre_shader_map["texture shader"]);

		}
		else if (name == "gen_object" && jnode.contains("object_name"))
		{
			std::string object_name = jnode["object_name"];

			delegate.gen_object_node(nodeRectangleMin, (object_name).c_str(), nullptr, this->object_count, pre_shader_map["nomal shader"]);
		}
		else if (name == "animation")
		{
			std::string animation_name = jnode["animation_name"];
			delegate.gen_animation_node(nodeRectangleMin, (name).c_str(), pre_animation_map[animation_name]);
		}
		else if (name == "shader")
		{
			std::string shader_name = jnode["shader_name"];
			delegate.gen_shader_node(nodeRectangleMin, "shader", pre_shader_map[shader_name]);
		}
		else if (name == "setting")
		{
			std::string setting_name = jnode["setting_name"];
			delegate.gen_setting_node(nodeRectangleMin, (setting_name).c_str(), pre_shader_map["nomal shader"]);


		}
		else
		{
			delegate.gen_node(nodeRectangleMin, (name).c_str(), nullptr);



		}

		delegate.mNodes.back()->x = nodeRectangleMin.x;
		delegate.mNodes.back()->y = nodeRectangleMin.y;
		if (jnode.contains("Draw"))
			delegate.mNodes.back()->Draw_check = jnode["Draw"];
		if (jnode.contains("initial_setting")) 
		{
			auto values = deserialize_initial_value(jnode["initial_setting"]);
			delegate.mNodes.back()->initial_setting(values);

		}
		if (jnode.contains("key_frame_items") && jnode.contains("key_frame_ramp"))
		{

			auto key_frame_Items = deserialize_initial_value(jnode["key_frame_items"]);
			auto key_frame_ramp = deserialize_initial_value(jnode["key_frame_ramp"]);

			delegate.mNodes.back()->initial_setting_key_frame(key_frame_Items, key_frame_ramp);

		}
}

	// get link
	for (const auto& jlink : j["links"]) {
		int inNode = jlink[0];
		int inSlot = jlink[1];
		int outNode = jlink[2];
		int outSlot = jlink[3];

		delegate.AddLink(inNode, inSlot, outNode, outSlot);
	}

}



nlohmann::json Node_manager::serialize_initial_value(const std::vector<ValueVariant>& values) {
	nlohmann::json json_array = nlohmann::json::array();

	for (const auto& val : values) {
		nlohmann::json item;

		std::visit([&](const auto& v) {
			using T = std::decay_t<decltype(v)>;

			if constexpr (std::is_same_v<T, int>) {
				item["type"] = "int";
				item["value"] = v;
			}
			else if constexpr (std::is_same_v<T, float>) {
				item["type"] = "float";
				item["value"] = v;
			}
			else if constexpr (std::is_same_v<T, glm::vec2>) {
				item["type"] = "vec2";
				item["value"] = { v.x, v.y };
			}
			else if constexpr (std::is_same_v<T, glm::vec3>) {
				item["type"] = "vec3";
				item["value"] = { v.x, v.y, v.z };
			}
			else if constexpr (std::is_same_v<T, glm::vec4>) {
				item["type"] = "vec4";
				item["value"] = { v.x, v.y, v.z, v.w };
			}
			else if constexpr (std::is_same_v<T, glm::mat3>) {
				item["type"] = "mat3";
				for (int i = 0; i < 3; ++i)
					item["value"].push_back({ v[i][0], v[i][1], v[i][2] });
			}
			else if constexpr (std::is_same_v<T, glm::mat4>) {
				item["type"] = "mat4";
				for (int i = 0; i < 4; ++i)
					item["value"].push_back({ v[i][0], v[i][1], v[i][2], v[i][3] });
			}
			else if constexpr (std::is_same_v<T, Quaternion>) {
				item["type"] = "quat";
				item["value"] = { v.x, v.y, v.z, v.w };
			}
			}, val);

		json_array.push_back(item);
	}

	return json_array;
}

std::vector<ValueVariant> Node_manager::deserialize_initial_value(const nlohmann::json& jarray) {
	std::vector<ValueVariant> result;

	for (const auto& item : jarray) {
		std::string type = item["type"];
		const auto& value = item["value"];

		switch (hash_table((type).c_str()))
		{
		case hash_table("int"):
		{

			result.emplace_back(value.get<int>());
			break;
		}
		case hash_table("float"):
		{
			result.emplace_back(value.get<float>());
			break;
		}
		case hash_table("vec2"):
		{
			result.emplace_back(glm::vec2(value[0], value[1]));
			break;
		}
		case hash_table("vec3"):
		{
			result.emplace_back(glm::vec3(value[0], value[1], value[2]));
			break;
		}
		case hash_table("vec4"):
		{
			result.emplace_back(glm::vec4(value[0], value[1], value[2], value[3]));
			break;
		}
		case hash_table("mat3"):
		{
			glm::mat3 mat;
			for (int i = 0; i < 3; ++i)
				mat[i] = glm::vec3(value[i][0], value[i][1], value[i][2]);

			result.emplace_back(mat);
			break;
		}
		case hash_table("mat4"):
		{
			glm::mat4 mat;
			for (int i = 0; i < 4; ++i)
				mat[i] = glm::vec4(value[i][0], value[i][1], value[i][2], value[i][3]);
			result.emplace_back(mat);
			break;
		}
		case hash_table("quat"):
		{
			result.emplace_back(Quaternion(value[0], value[1], value[2], value[3]));
			break;
		}
		}

	}

	return result;
}