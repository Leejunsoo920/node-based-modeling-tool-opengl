#ifndef  IMGUI_FUNC_H
#define IMGUI_FUNC_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"


void button_list_math(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("func"))
		ImGui::OpenPopup("my_select_popup_math");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_math"))
	{
		for (int i = 0; i < names_count ; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}

			

		ImGui::EndPopup();
		
	}

}
void button_list_gen_object(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("Gen_object"))
		ImGui::OpenPopup("my_select_popup_gen_object");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_gen_object"))
	{
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}



		ImGui::EndPopup();

	}

}

void button_list_animation(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("animation"))
		ImGui::OpenPopup("my_select_popup_animation");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_animation"))
	{
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}



		ImGui::EndPopup();

	}

}

template <std::size_t N>
bool button_list(
	const std::string list_name,
	std::string& select_name,
	const char* (&select_list)[N]
	)
{
	int list_count = static_cast<int>(N);

	if (ImGui::Button((list_name).c_str()))
		ImGui::OpenPopup(("my_select_popup" + list_name).c_str());
	ImGui::SameLine();


	if (ImGui::BeginPopup(("my_select_popup" + list_name).c_str()))
	{
		for (std::size_t i = 0; i < list_count; ++i)
		{
			if (ImGui::Selectable(select_list[i]))
			{
				select_name = select_list[i];
				ImGui::EndPopup();
				return true;
			}
		}



		ImGui::EndPopup();

	}
	return false;
}



template <typename T>
bool button_list_map(
	const std::string list_name,
	std::string& select_name,
	const T& names_map
)
{
	

	if (ImGui::Button((list_name).c_str()))
		ImGui::OpenPopup(("my_select_popup" + list_name).c_str());
	ImGui::SameLine();

	std::vector<std::string> names_list;

	for (const auto& [name, val] : names_map)
	{
		names_list.push_back(name);
	}
	int names_count = names_list.size();


	if (ImGui::BeginPopup(("my_select_popup" + list_name).c_str()))
	{
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable((names_list[i]).c_str()))
			{

				select_name = names_list[i];

				ImGui::EndPopup();
				return true;
			}
		}


		ImGui::EndPopup();

	}

	return false;


}





#endif
