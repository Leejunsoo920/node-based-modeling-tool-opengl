#ifndef NODE_GENERATE_OBJECT_H
#define NODE_GENERATE_OBJECT_H



#include <algorithm> 
#include <iostream>
#include "Node_template.h"




class Node_generate_circle_object : public Node
{
public:
    std::vector<std::shared_ptr<Object_setting>> initial_value;
    int initial_vertex_num = 12;
    float initial_radius = 0.02;

    Node_generate_circle_object(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Object_setting> object_setting,
        int object_count,
        std::shared_ptr<Shader> shader

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        Object_setting temp_Object_setting(object_count, shader);

        std::vector<int> new_indices;
        std::vector<Texture> new_texture;
        for (unsigned int i = 0; i < initial_vertex_num; i++)
        {
            new_indices.push_back(i);
        }
        Mesh new_mesh(generate_circle(initial_radius, initial_vertex_num), new_indices, new_texture);
        temp_Object_setting.object.meshes.push_back(new_mesh);
        initial_value.emplace_back(std::make_shared<Object_setting>(
            temp_Object_setting,
            object_count,
            shader
        ));

        this->value->emplace_back(initial_value[0]);


    }



    void evaluate() override
    {
        // if node have no input
        if (input_node.size() == 0)
        {
            initial_value[0]->object.meshes[0].vertices = generate_circle(initial_radius, initial_vertex_num);
            initial_value[0]->object.meshes[0].indices.clear();

            for (unsigned int i = 0; i < initial_vertex_num; i++)
            {
                initial_value[0]->object.meshes[0].indices.push_back(i);
            }
            value->clear();
            for (auto& v : initial_value)
            {
                value->emplace_back(v);
            }
            evaluated = true;
        }
        else if (input_node.size() > 0)
        {

            if ((*input_node[0]->value).size() < 2)
            {
                evaluated = false;
                return;
            }

            std::vector<glm::vec3> vertices;
            std::vector<Vertex> return_vertices;
            for (unsigned int i = 0; i < input_node[0]->value->size(); i++)
            {
                std::visit([&](auto&& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v<T1, glm::vec3>)
                        {
                            vertices.emplace_back(arg1);
                        }
                    }, (*input_node[0]->value)[i]);

            }

            for (int i = 0; i < vertices.size(); ++i)
            {
                Vertex this_vertex;
                this_vertex.Position = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
                return_vertices.emplace_back(this_vertex);
            }


            if (vertices.size() > 2)
            {
                initial_value[0]->object.meshes[0].vertices = return_vertices;
                initial_value[0]->object.meshes[0].indices.clear();

                value->clear();
                for (auto& v : initial_value)
                {
                    value->emplace_back(v);
                }
                evaluated = true;

            }



        }




    };


    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->initial_vertex_num = arg1;

                    }

                    if constexpr (std::is_same_v<T1, float>)
                    {

                        this->initial_radius = arg1;

                    }


                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.emplace_back(initial_vertex_num);
        this_value.emplace_back(initial_radius);


        return this_value;
    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {
        bool ret = false;
        if (this->value != nullptr)
        {


            std::visit([this, &ret](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;


                if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                    for (unsigned int i = 0; i < arg1->object.meshes[0].vertices.size() - 1; i++)
                    {
                        debug.set_line_group("circle", arg1->object.meshes[0].vertices[i].Position, arg1->object.meshes[0].vertices[i + 1].Position);
                    }
                    debug.set_line_group("circle", arg1->object.meshes[0].vertices[0].Position, arg1->object.meshes[0].vertices[arg1->object.meshes[0].vertices.size() - 1].Position);

                    ret = true;

                    return ret;

                }
                else
                {
                    ret = false;
                    return ret;


                }
                }, (*this->value)[0]);  // 

        }
        ret = false;
        return ret;

    }



private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();



            bool changed = false;
            changed |= ImGui::InputFloat((name + "x").c_str(), &initial_radius, 0.1f, 1.0f, "%.3f");
            changed |= ImGui::InputInt((name + "y").c_str(), &initial_vertex_num, 1, 10);
            if (initial_vertex_num < 3)
            {
                initial_vertex_num = 3;
            }

            if (changed)
            {
                evaluated = false;
            }




        }
        ImGui::EndChild();



    }


    std::vector<Vertex> generate_circle
    (
        float radius, int num_vertices
    )
    {
        std::vector<Vertex> return_vertices;

        std::vector<glm::vec3> points;
        points.reserve(num_vertices);

        const float PI = 3.14159265358979323846f;

        for (int i = 0; i < num_vertices; ++i) {
            float angle = 2.0f * PI * i / num_vertices; // 각도: 0 ~ 2π
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);


            Vertex this_vertex;
            this_vertex.Position = glm::vec3(x, 0.0f, z);


            return_vertices.emplace_back(this_vertex); // y=0: XZ 평면
        }



        return return_vertices;
    }





};





class Node_generate_sphere_object : public Node
{
public:
    std::vector<std::shared_ptr<Object_setting>> initial_value;
    float initial_radius = 0.02;
    int sector_count = 3;
    int stack_count = 3;

    Node_generate_sphere_object(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Object_setting> object_setting,
        int object_count,
        std::shared_ptr<Shader> shader

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        Object_setting temp_Object_setting(object_count, shader);

        std::vector<int> new_indices;
        std::vector<Texture> new_texture;

        std::vector<Vertex> new_vertices = generate_sphere(initial_radius, sector_count, stack_count);

        new_indices = generate_sphere_indices(sector_count, stack_count);

        Mesh new_mesh(new_vertices, new_indices, new_texture);
        temp_Object_setting.object.meshes.push_back(new_mesh);
        initial_value.emplace_back(std::make_shared<Object_setting>(
            temp_Object_setting,
            object_count,
            shader
        ));

        this->value->emplace_back(initial_value[0]);


    }



    void evaluate() override
    {
        // if node have no input
        if (input_node.size() == 0)
        {
            initial_value[0]->object.meshes[0].vertices = generate_sphere(initial_radius, sector_count, stack_count);
            initial_value[0]->object.meshes[0].indices.clear();


            initial_value[0]->object.meshes[0].indices = generate_sphere_indices(sector_count, stack_count);


            value->clear();
            for (auto& v : initial_value)
            {
                value->emplace_back(v);
            }
            evaluated = true;
        }


    };


    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();


        for (unsigned int i = 0; i < initial_value.size(); i++)
        {
            std::visit([this, i](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, float>)
                    {
                        this->initial_radius = arg1;
                    }
                    if constexpr (std::is_same_v<T1, int>)
                    {
                        if(arg1 > 2 && i == 1)    // count must over 2 to make triangle
                            this->sector_count = arg1;
                        if(arg1 > 2 && i == 2)  // count must over 2 to make triangle
                            this->stack_count = arg1;
                    }

                }, initial_value[i]);

        }
    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.emplace_back(initial_radius);
        this_value.emplace_back(sector_count);
        this_value.emplace_back(stack_count);

        return this_value;
    }




private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();



            bool changed = false;
            changed |= ImGui::InputFloat((name + "radius").c_str(), &initial_radius, 0.1f, 1.0f, "%.3f");
            changed |= ImGui::InputInt((name + "sector").c_str(), &sector_count, 1, 10);
            changed |= ImGui::InputInt((name + "stack").c_str(), &stack_count, 1, 10);
            if (sector_count < 3)
            {
                sector_count = 3;
            }
            if (stack_count < 3)
            {
                stack_count = 3;
            }

            if (changed)
            {
                evaluated = false;
            }




        }
        ImGui::EndChild();



    }



    std::vector<Vertex> generate_sphere
    (
        float radius,
        int sector_count,
        int stack_count
    )
    {
        std::vector<Vertex> return_vertices;

        return_vertices.reserve((sector_count + 1) * (stack_count + 1));

        const float PI = 3.14159265358979323846f;

        // stack (-π/2 ~ +π/2)
        for (int i = 0; i <= stack_count; ++i) {
            float stackAngle = PI / 2.0f - (float)i * (PI / (float)stack_count);
            float xy = radius * std::cos(stackAngle);
            float y = radius * std::sin(stackAngle);

            // sector(0 ~ 2π)
            for (int j = 0; j <= sector_count; ++j) {
                float sectorAngle = (float)j * (2.0f * PI / (float)sector_count);

                float x = xy * std::cos(sectorAngle);
                float z = xy * std::sin(sectorAngle);

                Vertex this_vertex;
                this_vertex.Position = glm::vec3(x, y, z);

                return_vertices.emplace_back(this_vertex);
            }
        }




        return return_vertices;
    }

    std::vector<int> generate_sphere_indices
    (
        int sector_count,
        int stack_count

    )
    {
        std::vector<int> new_indices;

        for (int i = 0; i < stack_count; ++i) {
            int k1 = i * (sector_count + 1);
            int k2 = k1 + sector_count + 1;

            for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
                // 위쪽 삼각형
                if (i != 0) {
                    new_indices.push_back(k1);
                    new_indices.push_back(k2);
                    new_indices.push_back(k1 + 1);
                }
                // 아래쪽 삼각형
                if (i != (stack_count - 1)) {
                    new_indices.push_back(k1 + 1);
                    new_indices.push_back(k2);
                    new_indices.push_back(k2 + 1);
                }
            }
        }

        return new_indices;

    }



};














#endif
