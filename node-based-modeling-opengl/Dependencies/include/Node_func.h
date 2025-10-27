#ifndef NODE_FUNC_H
#define NODE_FUNC_H


#include <algorithm> 
#include <iostream>
#include "Node_template.h"
#include <cmath>


extern Frame_manager frame_manager;
class Node_func_line : public Node_template_func
{
public:
    Node_func_line(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }


    void evaluate() override
    {
        if (check_input_node(1, this->evaluated , this->value) != true)
        {

            return;
        }

        this->value = input_node[0]->value;
        this->value_vector_size = (*this->value).size();
        evaluated = true;


    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }


private:

    //glm::vec3 direction = glm::vec3(0.0);

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();



        }
        ImGui::EndChild();


    }
};



class Node_func_line_to_global_matrix : public Node_template_func
{
public:
    Node_func_line_to_global_matrix(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::vec3> line;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {


                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::vec3>) {

                            line.emplace_back(arg1);

                        }
                        }, (*this->value)[i]);  // 

                }

                if (line.size() > 0)
                {
                    std::vector<glm::mat4> local_dir(line.size());
                    std::vector<glm::mat4> global_dir(line.size());
                    local_transform_matrix_to_global_transform_matrix(line, local_dir, global_dir);

                    if (input_node[0]->now_output_count == 1)
                    {

                        (*this->value).clear();
                        for (auto& it : global_dir)
                        {

                            this->value->emplace_back(it);

                        }
                        this->value_vector_size = (*this->value).size();
                        evaluated = true;
                    }
                    else
                    {
                        this->value = std::make_shared<std::vector<ValueVariant>>();

                        for (auto& it : global_dir)
                        {

                            this->value->emplace_back(it);

                        }
                        this->value_vector_size = (*this->value).size();
                        evaluated = true;

                    }


                }
                


            }

            



        }



    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1, 0.05);

                    }
                    }, (*this->value)[i]);  // 

            }
            


        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

        }
        ImGui::EndChild();


    }



    void set_local_transform_matrix_by_line(
        const std::vector<glm::vec3> point_line,
        std::vector<glm::mat4>& local_bone_T
    )
    {
        glm::vec3 before_dir = glm::vec3(0, 1, 0);
        glm::vec3 next_dir = point_line[1] - point_line[0];

        next_dir = glm::normalize(next_dir);

        Quaternion bone_Q = Quaternion(next_dir);

        local_bone_T[0] = bone_Q.quaternion_to_r_matrix();

        before_dir = next_dir;




        for (unsigned int i = 1; i < point_line.size() - 1; i++)
        {
            next_dir = point_line[i + 1] - point_line[i];


            if (glm::length(next_dir) < 1e-6)
            {
                next_dir = glm::vec3(0, 1, 0);
            }
            else
            {

                next_dir = glm::normalize(next_dir);
            }

            bone_Q = Quaternion(before_dir, next_dir);
            local_bone_T[i] = bone_Q.quaternion_to_r_matrix();
            before_dir = next_dir;

            //before_dir = before_dir * glm::vec3(0, 1, 0);

        }


    }

    void local_transform_matrix_to_global_transform_matrix(
        const std::vector<glm::vec3>& point_line,
        std::vector<glm::mat4>& local_bone_T,
        std::vector<glm::mat4>& global_bone_T
    )
    {
        glm::vec3 before_dir = glm::vec3(0, 1, 0);
        glm::vec3 next_dir = point_line[1] - point_line[0];

        Quaternion bone_Q = Quaternion(next_dir);

        local_bone_T[0] = bone_Q.quaternion_to_r_matrix();
        global_bone_T[0] = glm::translate(glm::mat4(1.0), point_line[0]) * local_bone_T[0];

        before_dir = next_dir;
        float length_of_next_point = glm::distance(point_line[0], point_line[1]);
        global_bone_T[1] = glm::translate(global_bone_T[0], glm::vec3(0, length_of_next_point, 0));

        for (unsigned int i = 1; i < point_line.size() - 1; i++)
        {

            glm::vec3 next_point = glm::inverse(global_bone_T[i]) * glm::vec4(point_line[i + 1], 1);
            float length_of_next_point = glm::length(next_point);
            bone_Q = Quaternion(next_point);

            local_bone_T[i] = bone_Q.quaternion_to_r_matrix();
            global_bone_T[i] = global_bone_T[i] * local_bone_T[i];
            
            global_bone_T[i + 1] = glm::translate(global_bone_T[i], glm::vec3(0, length_of_next_point, 0));
        }



    }



};



class Node_func_line_to_chain_matrix : public Node_template_func
{
public:
    Node_func_line_to_chain_matrix(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {


        if (check_input_node(1, this->evaluated, this->value) != true)
        {
            return;
        }


        this->value = input_node[0]->value;


        std::vector<glm::vec3> line;
        for (unsigned int i = 0; i < (*this->value).size(); i++)
        {


            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;


                if constexpr (std::is_same_v<T1, glm::vec3>) {

                    line.emplace_back(arg1);

                }
                }, (*this->value)[i]);  // 

        }

        if (line.size() > 0)
        {
            std::vector<glm::mat4> local_dir(line.size());
            std::vector<glm::mat4> global_dir(line.size());
            std::vector<glm::mat4> real_dir(line.size());
            local_transform_matrix_to_global_transform_matrix(line, local_dir, global_dir, real_dir);
            rotate_global_transform_matrix(global_dir);

            if (input_node[0]->now_output_count == 1)
            {

                (*this->value).clear();
                for (auto& it : global_dir)
                {

                    this->value->emplace_back(it);

                }
                this->value_vector_size = (*this->value).size();
                evaluated = true;
            }
            else
            {
                this->value = std::make_shared<std::vector<ValueVariant>>();

                for (auto& it : global_dir)
                {

                    this->value->emplace_back(it);

                }
                this->value_vector_size = (*this->value).size();
                evaluated = true;

            }


        }

    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1, 0.05);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

        }
        ImGui::EndChild();


    }



    void set_local_transform_matrix_by_line(
        const std::vector<glm::vec3> point_line,
        std::vector<glm::mat4>& local_bone_T
    )
    {
        glm::vec3 before_dir = glm::vec3(0, 1, 0);
        glm::vec3 next_dir = point_line[1] - point_line[0];

        next_dir = glm::normalize(next_dir);

        Quaternion bone_Q = Quaternion(next_dir);

        local_bone_T[0] = bone_Q.quaternion_to_r_matrix();

        before_dir = next_dir;




        for (unsigned int i = 1; i < point_line.size() - 1; i++)
        {
            next_dir = point_line[i + 1] - point_line[i];


            if (glm::length(next_dir) < 1e-6)
            {
                next_dir = glm::vec3(0, 1, 0);
            }
            else
            {

                next_dir = glm::normalize(next_dir);
            }

            bone_Q = Quaternion(before_dir, next_dir);
            local_bone_T[i] = bone_Q.quaternion_to_r_matrix();
            before_dir = next_dir;

            //before_dir = before_dir * glm::vec3(0, 1, 0);

        }


    }

    void local_transform_matrix_to_global_transform_matrix(
        const std::vector<glm::vec3>& point_line,
        std::vector<glm::mat4>& local_bone_T,
        std::vector<glm::mat4>& global_bone_T,
        std::vector<glm::mat4>& real_bone_T
    )
    {
        if (point_line.size() < 2) return;

        local_bone_T.resize(point_line.size());
        global_bone_T.resize(point_line.size());
        real_bone_T.resize(point_line.size());

        for (size_t i = 0; i < point_line.size(); ++i)
        {
            glm::vec3 curr_point = point_line[i];

            glm::vec3 tangent;
            if (i == 0)
                tangent = point_line[1] - point_line[0];
            else if (i == point_line.size() - 1)
                tangent = point_line[i] - point_line[i - 1];
            else
                tangent = point_line[i + 1] - point_line[i - 1]; 

            tangent = glm::normalize(tangent);

            glm::vec3 up = glm::vec3(0, 1, 0);
            if (glm::abs(glm::dot(tangent, up)) > 0.999f) up = glm::vec3(1, 0, 0);

            glm::mat4 rot_matrix = glm::lookAt(glm::vec3(0.0f), tangent, up);
            rot_matrix = glm::inverse(rot_matrix); 

            local_bone_T[i] = rot_matrix;
            global_bone_T[i] = glm::translate(glm::mat4(1.0f), curr_point) * rot_matrix;
            real_bone_T[i] = global_bone_T[i];
        }

        



    }
    void rotate_global_transform_matrix
    (
            std::vector<glm::mat4>&global_bone_T
    )

    {
        if (global_bone_T.size() < 2) return;

        for (size_t i = 1; i < global_bone_T.size(); i+=2)
        {


            Quaternion rotation_90 = Quaternion(90.0f, glm::vec3(0, 0, 1));

            global_bone_T[i] = global_bone_T[i] * rotation_90.quaternion_to_r_matrix();


        }
    }



};




class Node_func_bezier_curve : public Node_template_func
{
public:
    Node_func_bezier_curve(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {
        now_output_count = output_node_num;

        if (check_input_node(2, this->evaluated, this->value) == false)
        {
            return;
        }

        this->value = input_node[0]->value;



        if (input_node[1]->value->size() == 1)
        {
            int curve_point_num;
            std::vector<glm::vec3> curve_point_vec3;

            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, int>) {

                    curve_point_num = arg1;

                }
                }, (*input_node[1]->value)[0]);  // 



            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::vec3>) {

                        curve_point_vec3.emplace_back(arg1);

                    }
                    }, (*this->value)[i]);  // 

            }
            std::vector<glm::vec3> final_curve;

            if (curve_point_vec3.size() >= 3)
            {

                float gap = 1 / static_cast<float>(curve_point_num - 1);
                for (size_t i = 0; i + 2 < curve_point_vec3.size(); i += 2)
                {
                    glm::vec3 p0 = curve_point_vec3[i];
                    glm::vec3 p1 = curve_point_vec3[i + 1];
                    glm::vec3 p2 = curve_point_vec3[i + 2];


                    float t = 0.0f;

                    for (int j = 0; j < curve_point_num; ++j)
                    {
                        glm::vec3 pt = make_bezier_curve(p0, p1, p2, t);
                        final_curve.push_back(pt);
                        t += gap;


                    }
                    // Remove the last point to avoid duplication when connecting curve segments.
                    // The last point of the current segment will be the first point of the next segment,
                    // so without this, the shared point would be stored twice in final_curve.
                    if (i + 3 < curve_point_vec3.size()) {
                        final_curve.pop_back();
                    }


                }






                (*this->value).clear();
                for (auto& it : final_curve)
                {

                    this->value->emplace_back(it);

                }

                this->value_vector_size = (*this->value).size();
                evaluated = true;
            }

        }


    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }

    



private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);

        return interpolate.do_interpolate(A, B, t);
    }

    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, const T& P3, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);
        T C = interpolate.do_interpolate(P2, P3, t);

        T D = interpolate.do_interpolate(A, B, t);
        T E = interpolate.do_interpolate(B, C, t);


        return interpolate.do_interpolate(D, E, t);




    }




};



class Node_func_cubic_bezier_curve : public Node_template_func
{
    int curve_type = 0;
    const char* type_name_list[2] = { "by num", "by distance"};
public:
    Node_func_cubic_bezier_curve(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {
        now_output_count = output_node_num;

        if (check_input_node(2, this->evaluated, this->value) != true)
        {

            return;
        }
        this->evaluated = false;

        this->value = input_node[0]->value;


        int curve_point_num;
        float curve_point_distance;
        switch (curve_type)
        {
        case 0:
        {
            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, int>) {


                    curve_point_num = arg1;

                }
                }, (*input_node[1]->value)[0]);  // 

            if (curve_point_num == 0)
            {
                this->evaluated = false;
                return;
            }

            break;

        }
        case 1:
        {

            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, float>) {


                    curve_point_distance = arg1;

                }
                }, (*input_node[1]->value)[0]);  // 

            if (curve_point_distance <= 0.0001) // rimit
            {
                this->evaluated = false;
                return;
            }
            break;
        }
        }


        std::vector<glm::vec3> curve_point_vec3;


        for (unsigned int i = 0; i < (*this->value).size(); i++)
        {
            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, glm::vec3>) {

                    curve_point_vec3.emplace_back(arg1);

                }
                }, (*this->value)[i]);  // 

        }
        std::vector<glm::vec3> final_curve;

        if (curve_point_vec3.size() >= 4)
        {
            switch (curve_type)
            {
            case 0:
            {

                final_curve = curve_by_point_num(curve_point_vec3, curve_point_num);
                break;
            }
            case 1:
            {
                final_curve = curve_by_distance(curve_point_vec3, curve_point_distance);

            }
            }





            (*this->value).clear();
            for (auto& it : final_curve)
            {
                this->value->emplace_back(it);

            }

            this->value_vector_size = (*this->value).size();
            evaluated = true;



        }



    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->curve_type = arg1;

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.push_back(curve_type);

        return this_value;
    }




private:

    // make point by making point number 
    std::vector<glm::vec3> curve_by_point_num
    (
        const std::vector<glm::vec3>& curve_point_vec3,
        const int& curve_point_num
        
    )
    {
        std::vector<glm::vec3> final_curve;


        int segment_count = (curve_point_vec3.size() - 1) / 3;
        float step = 1.0f / static_cast<float>(curve_point_num - 1);


        for (int j = 0; j < curve_point_num; ++j)
        {
            float global_t = j * step; // 0 ~ 1 
            float seg_t = global_t * segment_count; 

            int seg_index = static_cast<int>(seg_t);
            float local_t = seg_t - seg_index;

            
            if (seg_index >= segment_count)
                seg_index = segment_count - 1;

            glm::vec3 p0 = curve_point_vec3[seg_index * 3 + 0];
            glm::vec3 p1 = curve_point_vec3[seg_index * 3 + 1];
            glm::vec3 p2 = curve_point_vec3[seg_index * 3 + 2];
            glm::vec3 p3 = curve_point_vec3[seg_index * 3 + 3];

            glm::vec3 pt = make_bezier_curve(p0, p1, p2, p3, local_t);
            final_curve.emplace_back(pt);


        }
        return final_curve;
    }

    std::vector<glm::vec3> curve_by_distance(
        const std::vector<glm::vec3>& curve_point_vec3,
        float distance
    )
    {
        std::vector<glm::vec3> final_curve;
        if (curve_point_vec3.size() < 4) return final_curve;

        int segment_count = (curve_point_vec3.size() - 1) / 3;
        const int resolution = 200; 

   
        std::vector<glm::vec3> samples;
        std::vector<float> cumulative_length;
        samples.reserve(segment_count * resolution);
        cumulative_length.reserve(segment_count * resolution);

        float total_length = 0.0f;

        for (int s = 0; s < segment_count; ++s)
        {
            glm::vec3 prev = curve_point_vec3[s * 3];
            for (int i = 0; i <= resolution; ++i)
            {
                float t = static_cast<float>(i) / resolution;
                glm::vec3 cur = make_bezier_curve(
                    curve_point_vec3[s * 3 + 0],
                    curve_point_vec3[s * 3 + 1],
                    curve_point_vec3[s * 3 + 2],
                    curve_point_vec3[s * 3 + 3],
                    t
                );
                if (!(s == 0 && i == 0)) total_length += glm::distance(prev, cur);

                samples.push_back(cur);
                cumulative_length.push_back(total_length);
                prev = cur;
            }
        }


        final_curve.emplace_back(samples.front());
        float target_length = distance;

        while (target_length < total_length)
        {
            auto it = std::lower_bound(cumulative_length.begin(), cumulative_length.end(), target_length);
            if (it == cumulative_length.end()) break;

            int idx = std::distance(cumulative_length.begin(), it);

            if (idx == 0)
            {
                final_curve.emplace_back(samples[0]);
            }
            else
            {

                float prev_len = cumulative_length[idx - 1];
                float next_len = cumulative_length[idx];
                float ratio = (target_length - prev_len) / (next_len - prev_len);

                glm::vec3 p = glm::mix(samples[idx - 1], samples[idx], ratio);
                final_curve.emplace_back(p);
            }

            target_length += distance;
        }

        final_curve.emplace_back(samples.back());
        return final_curve;
    }



    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {


            base_imgui();

            bool changed = false;
            changed = ImGui::ListBox("type", &curve_type, type_name_list, IM_ARRAYSIZE(type_name_list));


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);

        return interpolate.do_interpolate(A, B, t);
    }

    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, const T& P3, float t)
    
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);
        T C = interpolate.do_interpolate(P2, P3, t);

        T D = interpolate.do_interpolate(A, B, t);
        T E = interpolate.do_interpolate(B, C, t);


        return interpolate.do_interpolate(D, E, t);




    }




};




class Node_func_catenary_curve : public Node_template_func
{
    int curve_type = 0;
    const char* type_name_list[2] = { "by num", "by distance" };
public:
    Node_func_catenary_curve(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {
        now_output_count = output_node_num;

        if (check_input_node(2, this->evaluated, this->value) != true)
        {

            return;
        }
        this->evaluated = false;

        this->value = input_node[0]->value;


        int curve_point_num;
        float curve_point_distance;
        switch (curve_type)
        {
        case 0:
        {
            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, int>) {


                    curve_point_num = arg1;

                }
                }, (*input_node[1]->value)[0]);  // 

            if (curve_point_num == 0)
            {
                this->evaluated = false;
                return;
            }



        }
        case 1:
        {

            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, float>) {


                    curve_point_distance = arg1;

                }
                }, (*input_node[1]->value)[0]);  // 

            if (curve_point_distance <= 0.0001) // rimit
            {
                this->evaluated = false;
                return;
            }

        }
        }


        std::vector<glm::vec3> curve_point_vec3;


        for (unsigned int i = 0; i < (*this->value).size(); i++)
        {
            std::visit([&](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, glm::vec3>) {

                    curve_point_vec3.emplace_back(arg1);

                }
                }, (*this->value)[i]);  // 

        }
        std::vector<glm::vec3> final_curve;

        if (curve_point_vec3.size() >= 2)
        {
            switch (curve_type)
            {
            case 0:
            {
                final_curve = curve_by_distance(curve_point_vec3, 0.5, curve_point_distance);
                break;
            }
            case 1:
            {


            }
            }

            (*this->value).clear();
            for (auto& it : final_curve)
            {
                this->value->emplace_back(it);

            }

            this->value_vector_size = (*this->value).size();
            evaluated = true;



        }



    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {
        return Draw_vector_vec3(projection, view, camera_position);
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->curve_type = arg1;

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.push_back(curve_type);

        return this_value;
    }




private:

    
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {


            base_imgui();

            bool changed = false;
            changed = ImGui::ListBox("type", &curve_type, type_name_list, IM_ARRAYSIZE(type_name_list));


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    inline float cosh_f(float x) {
        x = std::min(std::abs(x), 20.0f); // exp 폭발 방지
        return static_cast<float>(std::cosh(static_cast<double>(x)));
    }

    inline float sinh_f(float x) {
        x = std::min(std::abs(x), 20.0f); // exp 폭발 방지
        return static_cast<float>(std::sinh(static_cast<double>(x)));
    }
    inline glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t) {
        return a * (1.0f - t) + b * t;
    }
    std::vector<glm::vec3> curve_by_distance(
        const std::vector<glm::vec3>& control_points,

        float sag_ratio,    
        float distance
    )
    {
        std::vector<glm::vec3> high_res_points;
        if (control_points.size() < 2) return high_res_points;
        if (distance <= 0.0f) distance = 0.1f;

        // ----------------- 각 구간 해상도 높은 샘플 생성 -----------------
        for (size_t i = 0; i + 1 < control_points.size(); ++i) {
            const glm::vec3& p0 = control_points[i];
            const glm::vec3& p1 = control_points[i + 1];

            glm::vec3 dir = p1 - p0;
            float L = glm::length(glm::vec2(dir.x, dir.z));
            if (L < 1e-5f) {
                if (high_res_points.empty() || high_res_points.back() != p0)
                    high_res_points.push_back(p0);
                continue;
            }

            float dy = p1.y - p0.y;
            float sagAmount = L * sag_ratio;
            float a = (L * L) / (8.0f * sagAmount + 1e-5f);

            for (int j = 0; j <= 200; ++j) {
                float t = float(j) / 200;
                glm::vec3 pos = lerp(p0, p1, t);

                float x = t * L - L / 2.0f;
                float y_catenary = a * (cosh(x / a) - cosh(L / (2 * a)));
                pos.y = p0.y * (1.0f - t) + p1.y * t + y_catenary;

                if (i > 0 && j == 0) continue; // 중복 제거
                high_res_points.push_back(pos);
            }
        }

        high_res_points.push_back(control_points.back());

        // ----------------- 3D 거리 기반 정확 샘플링 -----------------
        std::vector<glm::vec3> sampled_points;
        sampled_points.push_back(high_res_points.front());

        float accumulated = distance;

        for (size_t i = 1; i < high_res_points.size(); ++i) {
            glm::vec3 p_start = high_res_points[i - 1];
            glm::vec3 p_end = high_res_points[i];

            glm::vec3 segment = p_end - p_start;
            float seg_len = glm::length(segment);
            if (seg_len < 1e-5f) continue;

            while (accumulated <= seg_len) {
                float ratio = accumulated / seg_len;
                glm::vec3 new_point = p_start + segment * ratio;
                sampled_points.push_back(new_point);
                accumulated += distance;
            }

            accumulated -= seg_len; // 남은 거리 carry
        }

        // 마지막 점 보정
        if (sampled_points.back() != control_points.back())
            sampled_points.push_back(control_points.back());

        return sampled_points;
    }
};

class Node_func_sweep : public Node_template_func
{
public:

    int sweep_type = 0;
    const char* type_name_list[2] = { "basic", "loop" };

    Node_func_sweep(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected){}

    void evaluate() override
    {

        if (check_input_node(2, this->evaluated, this->value) == false)
        {
            evaluated = false;
            return;
        }
        std::vector<glm::mat4> temp_mat4;

        for (auto& it : *input_node[1]->value)
        {
            std::visit([&](auto&& arg2) {
                using T2 = std::decay_t<decltype(arg2)>;

                if constexpr
                    (
                        std::is_same_v<T2, glm::mat4>)
                {
                    temp_mat4.emplace_back(arg2);

                }
                else
                {

                    return;
                }

                }, it);  // 

        }



        std::visit([&](auto& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;

                if constexpr (std::is_same_v < T1, std::shared_ptr<Object_setting>>)
                {
                    int bone_size = temp_mat4.size();

                    int vertex_num = arg1->object.meshes[0].vertices.size();

                    std::vector<Vertex> origin_vertices;
                    for (auto&& it : arg1->object.meshes[0].vertices)
                    {
                        origin_vertices.push_back(it);

                    }



                    std::vector<Vertex> new_vertices;
                    std::vector<int> new_indices;

                    for (unsigned int i = 0; i < bone_size; i++)
                    {
                        int vertex_size = origin_vertices.size();

                        for (unsigned int j = 0; j < vertex_size; j++)
                        {
                            new_vertices.push_back(origin_vertices[j]);

                            new_vertices.back().Position = temp_mat4[i] * glm::vec4(origin_vertices[j].Position, 1);
                        }



                        auto vertex_index = new_vertices.end() - new_vertices.begin() - 2 * vertex_size;

                        if (bone_size > 1)
                        {
                            if (i > 0)
                            {

                                for (unsigned int k = 0; k < vertex_size - 1; k++)
                                {

                                    new_indices.emplace_back(vertex_index + k);
                                    new_indices.emplace_back(vertex_index + vertex_size + k);
                                    new_indices.emplace_back(vertex_index + vertex_size + 1 + k);

                                    new_indices.emplace_back(vertex_index + k);
                                    new_indices.emplace_back(vertex_index + 1 + vertex_size + k);
                                    new_indices.emplace_back(vertex_index + 1 + k);


                                }

                                //// the last primitive of sweep 
                                //// making primitive by first vertex and last vertex 
                                new_indices.emplace_back(vertex_index + vertex_size - 1);
                                new_indices.emplace_back(vertex_index + vertex_size - 1 + vertex_size);
                                new_indices.emplace_back(vertex_index + vertex_size);

                                new_indices.emplace_back(vertex_index + vertex_size - 1);
                                new_indices.emplace_back(vertex_index + vertex_size);
                                new_indices.emplace_back(vertex_index);


                            }
                            if (i == bone_size - 1 && sweep_type == 1) // loop mesh
                            {
                                int first_vertex_index = 0; 
                                int last_vertex_index = new_vertices.size() - vertex_size; 

                                for (unsigned int k = 0; k < vertex_size - 1; k++)
                                {
                                    new_indices.emplace_back(last_vertex_index + k);
                                    new_indices.emplace_back(first_vertex_index + k);
                                    new_indices.emplace_back(first_vertex_index + k + 1);

                                    new_indices.emplace_back(last_vertex_index + k);
                                    new_indices.emplace_back(first_vertex_index + k + 1);
                                    new_indices.emplace_back(last_vertex_index + k + 1);
                                }


                                new_indices.emplace_back(last_vertex_index + vertex_size - 1);
                                new_indices.emplace_back(first_vertex_index + vertex_size - 1);
                                new_indices.emplace_back(first_vertex_index);

                                new_indices.emplace_back(last_vertex_index + vertex_size - 1);
                                new_indices.emplace_back(first_vertex_index);
                                new_indices.emplace_back(last_vertex_index);
                            }

                        }
                        else
                        {

                            for (unsigned int k = 0; k < vertex_size; k++)
                            {

                                new_indices.emplace_back(vertex_index - k);


                            }



                        }




                    }

                    //if(input_node)
                    //arg1->object.meshes[0].indices = new_indices;
                    //arg1->object.meshes[0].vertices = new_vertices;

                    //auto obj_ptr = std::make_shared<Object_setting>(arg1->shader);
                    arg1->object.meshes[0].vertices = new_vertices;
                    arg1->object.meshes[0].indices = new_indices;
                    this->value->clear();
                    this->value->emplace_back(arg1);


                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    return;
                }



            }, (*input_node[0]->value)[0]);





    }

    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->sweep_type = arg1;

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.push_back(sweep_type);

        return this_value;
    }

private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

            bool changed = false;
            changed = ImGui::ListBox("type", &sweep_type, type_name_list, IM_ARRAYSIZE(type_name_list));


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }






};



class Node_func_select_one_value : public Node_template_func
{
public:
    int initial_value = 0;

    Node_func_select_one_value(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected) {}

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) == false)
        {
            return;
        }

        std::vector<glm::mat4> temp_mat4;
        auto& it = (*input_node[1]->value)[0];

        std::visit([&](auto&& arg2) {
            using T2 = std::decay_t<decltype(arg2)>;

            if constexpr
                (
                    std::is_same_v<T2, int>)
            {


                initial_value = arg2;

            }
            else
            {

                return;
            }

            }, it);  // 

        int temp = (*input_node[0]->value).size();
        if (initial_value >= temp)
        {
            initial_value = temp - 1;
        }


        (*this->value).clear();
        (*this->value).emplace_back((*input_node[0]->value)[initial_value]);

        this->value_vector_size = (*this->value).size();
        evaluated = true;


    }



private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();
    }
};





class Node_func_matrix_rotation : public Node_template_func
{
public:
    glm::mat4 initial_value = glm::mat4(1.0f);
    Node_func_matrix_rotation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;

                if (input_node.size() >= 2)
                {

                    std::visit([&](auto&& arg0) {
                        using T1 = std::decay_t<decltype(arg0)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            glm::mat4 rotation_mat4 = glm::mat4(1.0f);

                            rotation_mat4[0] = glm::vec4(glm::normalize(glm::vec3(arg0[0])), 0.0f); 
                            rotation_mat4[1] = glm::vec4(glm::normalize(glm::vec3(arg0[1])), 0.0f);  
                            rotation_mat4[2] = glm::vec4(glm::normalize(glm::vec3(arg0[2])), 0.0f);  
                            rotation_mat4[3] = glm::vec4(0, 0, 0, 1);  




                            initial_value = rotation_mat4;

                        }
                        }, (*input_node[1]->value)[0]);  


                }

                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {




                    
                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>) 
                        {


                            glm::vec3 position = glm::vec3(arg1[3]);


                            glm::mat4 rotationOnly = glm::mat4(1.0f);
                            rotationOnly[0] = glm::vec4(glm::normalize(glm::vec3(arg1[0])), 0.0f);
                            rotationOnly[1] = glm::vec4(glm::normalize(glm::vec3(arg1[1])), 0.0f);
                            rotationOnly[2] = glm::vec4(glm::normalize(glm::vec3(arg1[2])), 0.0f);



                            glm::mat4 rotated = initial_value * rotationOnly;


                            rotated[3] = glm::vec4(position, 1.0f);

                            global_dir.emplace_back(rotated);
                        }
                        }, (*this->value)[i]);  

                }











                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }





    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = false;

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};




class Node_func_now_frame : public Node_template_func
{
public:

    int before_frame = 0;

    Node_func_now_frame(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {


        (this->value)->clear();
        (this->value)->push_back(frame_manager.now_frame);

        evaluated = true;

    }

private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            ImGui::Text("now frame : %d", frame_manager.now_frame);



        }
        ImGui::EndChild();


    }
};




class Node_func_matrix_z_look_up : public Node_template_func
{
public:
    glm::mat4 initial_value = glm::mat4(1.0f);
    Node_func_matrix_z_look_up(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }




    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {

                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;    

                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            glm::vec3 position = glm::vec3(arg1[3]);

                            glm::vec3 up_dir = glm::inverse(arg1) *glm::vec4(0, 1, 0,0);
                            


                            glm::vec3 y_dir = arg1 * glm::vec4(0, 1, 0, 0.0f);

                            y_dir = glm::normalize(y_dir);


                            static float test_angle = 0;
                            Quaternion quaternion = Quaternion(up_dir, glm::vec3(0, 1, 0));


                            glm::mat4 rotated = arg1 * quaternion.quaternion_to_r_matrix();


                            glm::vec3 z_dir = rotated * glm::vec4(0, 0, 1, 0.0f);
                            float rotation_angle = glm::dot(z_dir, y_dir);
                            Quaternion set_z = Quaternion(glm::degrees(acos(rotation_angle)), glm::vec3(0, -1, 0));
                            rotated = rotated * set_z.quaternion_to_r_matrix();

                            global_dir.emplace_back(rotated);
                        }
                        }, (*this->value)[i]);

                }











                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }





    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = false;

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};







class Node_func_mesh_projection : public Node_template_func
{
public:
    
    std::shared_ptr<Object_setting> initial_value;
    
    Node_func_mesh_projection(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected) 
    {
    }



    void evaluate() override
    {

        if (input_node.size() >= 2) 
        {
            if (initial_value != nullptr)
            {
                this->value_vector_size = (*this->value).size();
                evaluated = true;
                return;
            }



            if ((*input_node[0]->value).size() > 0 && (*input_node[1]->value).size() > 0 )
            {


                std::vector<int> vertex_num;
                std::vector<glm::vec3> vertex_list;

                auto& it0 = (*input_node[0]->value)[0];
                auto& it1 = (*input_node[1]->value)[0];

                std::visit([&](auto&& arg1, auto&& arg2) 
                    {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;


                    if constexpr (
                        std::is_same_v<T1, std::shared_ptr<Object_setting>> &&
                        std::is_same_v<T2, std::shared_ptr<Object_setting>>
                        )
                    {
                        initial_value = std::make_shared<Object_setting>(*arg2, -1, arg2->shader);

                        std::vector<glm::vec3> target_object_vertex;
                        std::vector<int> target_object_indices;
                        for (auto& vertex : arg1->object.meshes[0].vertices)
                        {
                            target_object_vertex.emplace_back(vertex.Position);
                        }
                        for (auto& index : arg1->object.meshes[0].indices)
                        {
                            target_object_indices.emplace_back(index);
                        }

                        std::vector<Vertex> new_pos;
                        std::vector<int> new_index = initial_value->object.meshes[0].indices;
                        std::vector<int> minus_index = std::vector<int>(new_index.size());


                        act_projection(target_object_vertex, target_object_indices, initial_value->object.meshes[0].vertices, new_pos, new_index);
                        


                        initial_value->object.meshes[0].vertices = new_pos;
                        initial_value->object.meshes[0].indices = new_index;


                        (this->value)->emplace_back(initial_value);



                    }
                    else
                    {
                        return;
                    }

                    }, it0, it1);  // 

                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
            }
        }
        else
        {
            initial_value = nullptr;

        }



    }

    
private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();
            bool changed = false;
            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }


    void act_projection(
        const std::vector<glm::vec3>& vertices,
        const std::vector<int>& indices,
        std::vector<Vertex>& point,
        std::vector<Vertex>& output_vertex,
        std::vector<int>& output_index
    )
    {
        int minus_count = 0;

        std::map<int, int> set_new_index;



        for (unsigned int i = 0; i < point.size(); i++)
        {
            int closest_triangle = -1;
            glm::vec3 closest_position;

            if (findClosestPointOnMesh(vertices, indices, point[i].Position, closest_triangle, closest_position))
            {
                Vertex new_vertex = point[i];
                new_vertex.Position = closest_position;
                output_vertex.emplace_back(new_vertex);
                set_new_index[i] = output_vertex.size() - 1 ;

            }
            else
            {
                remove_faces_with_vertex(output_index, i);
                minus_count++;

                set_new_index[i] = -1;
            }



        }

        for (unsigned int i = 0; i < output_index.size(); i++)
        {
            output_index[i] = set_new_index[output_index[i]];

        }





    }

    void remove_faces_with_vertex(std::vector<int>& indices, unsigned int target) 
    {
        for (size_t i = 0; i < indices.size()-2;) {
            if (indices[i] == target || indices[i + 1] == target || indices[i + 2] == target) {
                indices.erase(indices.begin() + i, indices.begin() + i + 3);

                /*
                
                for (unsigned int j = 0; j < indices.size(); j++)
                {
                    if (indices[j] > target)
                        indices[j]--;


                }
                */
            }
            else {
                i += 3;
            }
        }

    }


    bool findClosestPointOnMesh(
        const std::vector<glm::vec3>& vertices,
        const std::vector<int>& indices,
        glm::vec3& point,
        int& outTriangleIndex,
        glm::vec3& outClosestPoint)
    {
        float minDistSq = std::numeric_limits<float>::max();
        glm::vec3 closest;

        bool have_q = false;

        for (size_t i = 0; i < indices.size()-2; i += 3) 
        {
            const glm::vec3& a = vertices[indices[i]];
            const glm::vec3& b = vertices[indices[i + 1]];
            const glm::vec3& c = vertices[indices[i + 2]];

            glm::vec3 q = closestPointOnTriangle(point, a, b, c);
            
            float distSq = glm::length(point - q);

            if (distSq < 0.05)
            {
                if (distSq < minDistSq) 
                {
                    minDistSq = distSq;
                    closest = q;
                    outTriangleIndex = static_cast<int>(i / 3);
                    outClosestPoint = q;
                    have_q = true;
                }
            
            }
        }
        return have_q;


    }

    glm::vec3 closestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ap = p - a;

        float d1 = glm::dot(ab, ap);
        float d2 = glm::dot(ac, ap);

        if (d1 <= 0.0f && d2 <= 0.0f) return a; // Bary(1,0,0)

        glm::vec3 bp = p - b;
        float d3 = glm::dot(ab, bp);
        float d4 = glm::dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return b; // Bary(0,1,0)

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + v * ab; // Bary(1 - v, v, 0)
        }

        glm::vec3 cp = p - c;
        float d5 = glm::dot(ab, cp);
        float d6 = glm::dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return c; // Bary(0,0,1)

        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + w * ac; // Bary(1 - w, 0, w)
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b); // Bary(0, 1 - w, w)
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w; // 내부
    }




};


























/*

class Node_func_vertex_translate : public Node_template_func
{
public:
    std::vector<glm::mat4> translate_matrix;

    Node_func_translate(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true)
            {

                this->value = input_node[0]->value;


                std::vector<glm::mat4> global_dir;
                for (unsigned int i = 0; i < (*this->value).size(); i++)
                {


                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;


                        if constexpr (std::is_same_v<T1, glm::mat4>)
                        {


                            global_dir.emplace_back(glm::rotate(arg1, glm::radians(initial_value), glm::vec3(0, 1, 0)));
                        }
                        }, (*this->value)[i]);  // 

                }

                if (input_node[0]->now_output_count == 1)
                {

                    (*this->value).clear();
                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;
                }
                else
                {
                    this->value = std::make_shared<std::vector<ValueVariant>>();

                    for (auto& it : global_dir)
                    {

                        this->value->emplace_back(it);

                    }
                    this->value_vector_size = (*this->value).size();
                    evaluated = true;

                }


            }





        }



    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }




private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();
            bool changed = ImGui::SliderFloat("angle", &initial_value, 0, 360);

            if (changed)
                evaluated = false;

        }
        ImGui::EndChild();


    }





};
*/

class Node_func_rotation : public Node_template_func
{
public:

    float angle = 0;
    int twisting = 0;

    Node_func_rotation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) != true)
        {

            return;
        }
        std::vector<glm::vec3> rotation_axis_list;

        if (input_node.size() > 2)
        {
            for (unsigned int i = 0; i < input_node[2]->value->size(); i++)
            {

                std::visit([&](auto&& arg0) {
                    using T1 = std::decay_t<decltype(arg0)>;
                    if constexpr (std::is_same_v<T1, float>)
                    {
                        angle = arg0;

                    }
                    }, (*input_node[2]->value)[0]);
            }
        }



        // find rotation axis by input 2
        for (unsigned int i = 0; i < 2; i++)
        {

            std::visit([&](auto&& arg0) {
                using T1 = std::decay_t<decltype(arg0)>;
                if constexpr (std::is_same_v<T1, glm::vec3>)
                {
                    rotation_axis_list.emplace_back(arg0);

                }
                }, (*input_node[1]->value)[i]);


        }
        std::vector<glm::vec3> after_rotation_point_list;
        (this->value)->clear();
        for (unsigned int i = 0; i < input_node[0]->value->size(); i++)
        {

            glm::vec3 rotation_axis = rotation_axis_list[1] - rotation_axis_list[0];
            std::visit([&](auto&& arg0) {
                using T1 = std::decay_t<decltype(arg0)>;
                if constexpr (std::is_same_v<T1, glm::vec3>)
                {
                    glm::mat4 rotation_mat4 = glm::mat4(1.0f);

                    glm::vec3 origin_dir = arg0 - rotation_axis_list[0];

                    float dir_length = glm::distance(arg0, rotation_axis_list[0]);

                    Quaternion rotation_q;
                    if (twisting)
                    {
                        float real_angle = ((*input_node[0]->value).size() - i) * angle / (*input_node[0]->value).size();

                        rotation_q = Quaternion(real_angle, rotation_axis);
                    }
                    else
                    {
                        rotation_q = Quaternion(angle, rotation_axis);
                    }


                    glm::vec3 new_point = rotation_q.quaternion_to_r_matrix() * glm::vec4(origin_dir, 1);

                    new_point = rotation_axis_list[0] + new_point;

                    this->value->push_back(new_point);

                }
                }, (*input_node[0]->value)[i]);
        }
        evaluated = true;

    }


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
                        if (arg1 < 2)
                        {
                             this->twisting = arg1;
                        }


                    }

                    if constexpr (std::is_same_v<T1, float>)
                    {

                        this->angle = arg1;

                    }


                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.emplace_back(angle);
        this_value.emplace_back(twisting);


        return this_value;
    }





private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            bool changed = false;

            changed |= ImGui::SliderFloat((name + "angle").c_str(), &angle, 0, 360);
            changed |= ImGui::SliderInt((name + "twist").c_str(), &twisting, 0, 1);

            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }
};



class Node_func_array_all_rotate : public Node_template_func
{
public:

    float angle = 0;
    int twisting = 0;

    Node_func_array_all_rotate(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) != true)
        {

            return;
        }

        if (input_node[1]->value->size() < 1)
        {
            evaluated = false;
            return;
        }

        std::vector<glm::vec3> rotation_axis_list;



        // find rotation axis by input 2
        for (unsigned int i = 0; i < 2; i++)
        {

            std::visit([&](auto&& arg0) {
                using T1 = std::decay_t<decltype(arg0)>;
                if constexpr (std::is_same_v<T1, glm::vec3>)
                {
                    rotation_axis_list.emplace_back(arg0);

                }
                }, (*input_node[1]->value)[i]);


        }
        std::vector<glm::mat4> after_rotation_point_list;
        (this->value)->clear();

        float befor_last_angle = 0;
        glm::mat4 befor_last_mat = glm::mat4(1.0f);
        for (unsigned int i = 0; i < input_node[0]->value->size(); i++)
        {

            glm::vec3 rotation_axis = rotation_axis_list[1] - rotation_axis_list[0];
            std::visit([&](auto&& arg0) {
                using T1 = std::decay_t<decltype(arg0)>;
                if constexpr (std::is_same_v<T1, glm::mat4>)
                {
                    glm::vec3 p = glm::vec3(arg0[3]); // the point that we find rotate
                    glm::vec3 a = rotation_axis_list[0]; // the point on the axis line
                    glm::vec3 rotation_dir = glm::normalize(rotation_axis);


                    glm::vec3 pa = p - a;
                    glm::vec3 q = a + glm::dot(pa, rotation_dir) * rotation_dir; //	the foot of the perpendicular from point P to line L
                    glm::vec3 dir_q_to_p = glm::normalize(q - p);
                    glm::vec3 x_axis = glm::normalize(-arg0[0]);
                    glm::vec3 z_axis = glm::normalize(arg0[2]);

                    glm::vec3 y_axis = glm::normalize(arg0[1]); // also plane nomal dir



                    float denom = glm::dot(rotation_dir, y_axis);
                    if (glm::abs(denom) > 1e-6f) {
                        
                        float t = glm::dot(p - rotation_axis_list[0], y_axis) / denom;
                        glm::vec3 intersection_point = rotation_axis_list[0] + t * rotation_dir;
                        glm::vec3 collect_dir = glm::normalize(p - intersection_point);
                        glm::vec3 cross_vec = glm::cross(x_axis, collect_dir);

                        float sign = glm::dot(cross_vec, y_axis) < 0.0f ? -1.0f : 1.0f;


                        float cos_theta = glm::clamp(glm::dot(x_axis, collect_dir), -1.0f, 1.0f);
                        float angle_between = glm::degrees(acos(cos_theta)) * sign;
                        static int new_angle = 0;


                        // have bug at final matrix -> so apply same angle that before final angle at fianal matrix 
                        if (i == input_node[0]->value->size() - 2)
                        {
                            befor_last_angle = angle_between;
                        }

                        if (i == input_node[0]->value->size() - 1)
                        {

                            angle_between = befor_last_angle;
                        }

                        Quaternion rotation_q = Quaternion(-angle_between, y_axis);


                        glm::mat4 T_to_origin = glm::translate(glm::mat4(1.0f), -p);  
                        glm::mat4 T_back = glm::translate(glm::mat4(1.0f), p);        
                        glm::mat4 R = rotation_q.quaternion_to_r_matrix();            

                        glm::mat4 rotation_around_p = T_back * R * T_to_origin;
                        glm::mat4 new_mat = rotation_around_p * arg0;

                        if (i == input_node[0]->value->size() - 2)
                        {
                            befor_last_mat = new_mat;
                        }

                        if (i == input_node[0]->value->size() - 1)
                        {

                            new_mat = befor_last_mat;
                        }

                        after_rotation_point_list.push_back(new_mat);
                    }

 

                }
                }, (*input_node[0]->value)[i]);





        }


        if (input_node.size() > 2)
        {
            for (unsigned int i = 0; i < input_node[2]->value->size(); i++)
            {

                std::visit([&](auto&& arg0) {
                    using T1 = std::decay_t<decltype(arg0)>;
                    if constexpr (std::is_same_v<T1, float>)
                    {
                        angle = arg0;

                    }
                    }, (*input_node[2]->value)[0]);
            }
        }

        if (after_rotation_point_list.size() > 0)
        {

            if (input_node[0]->now_output_count != output_node_num-1)
            {

                (*this->value).clear();
                for (auto& it : after_rotation_point_list)
                {

                    this->value->emplace_back(it);

                }
                this->value_vector_size = (*this->value).size();
                evaluated = true;
            }
            else
            {
                this->value = std::make_shared<std::vector<ValueVariant>>();

                for (auto& it : after_rotation_point_list)
                {

                    this->value->emplace_back(it);

                }
                this->value_vector_size = (*this->value).size();
                evaluated = true;

            }
        }
        evaluated = true;

    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }


private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            bool changed = false;

            changed |= ImGui::SliderFloat((name + "angle").c_str(), &angle, 0, 360);
            changed |= ImGui::SliderInt((name + "twist").c_str(), &twisting, 0, 1);

            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }
};





class Node_func_rotate_control : public Node_template_func
{
public:

    float angle = 0;
    int control_count = 1;

    Node_func_rotate_control(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) != true)
        {
            return;
        }

        this->value = input_node[0]->value;

        unsigned int N = this->value->size() - 1;
        for (unsigned int i = this->value->size()-1; i > this->value->size() - control_count - 1; i--)
        { 
            std::visit([&](auto&& arg0, auto&& arg1) {
                using T1 = std::decay_t<decltype(arg0)>;
                using T2 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, glm::mat4> && std::is_same_v<T2, glm::mat4>)
                {
                    glm::vec4 origin_pos = arg0[3];
                    glm::mat3 rot_mat0 = glm::mat3(arg0);
                    glm::mat3 rot_mat1 = glm::mat3(arg1);

                    Quaternion rot1 = Quaternion(rot_mat0);
                    Quaternion rot2 = Quaternion(rot_mat1);

                    float temp_diff = control_count - (N - i);
                    float t = temp_diff / control_count;
                    //t = 1;


                    Quaternion interpolation_rot = rot1.force_rotation_dir_slerp(rot2, t);

                    glm::mat4 result_rotation_matrix = interpolation_rot.quaternion_to_r_matrix();

                    arg0 = result_rotation_matrix;
                    arg0[3] = origin_pos;

                }
                }, (*this->value)[i], (*input_node[1]->value)[0]);
        }
        this->value_vector_size = (*this->value).size();
        evaluated = true;

    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {


                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (std::is_same_v<T1, glm::mat4>) {

                        debug.set_axis("global matrix", arg1);

                    }
                    }, (*this->value)[i]);  // 

            }



        }
        return false;

    }
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
                        this->control_count = arg1;   
                    }
                }, it);
        }
    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;
        this_value.emplace_back(this->control_count);
        return this_value;
    }


private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            bool changed = false;
            
            changed |= ImGui::SliderInt((name + "twist").c_str(), &control_count, 0, 10);


            if (evaluated)
            {
                if (control_count < 1)
                {
                    control_count = 1;
                }
                if (control_count > (*input_node[0]->value).size())
                {
                    control_count = (*input_node[0]->value).size();
                }
            }


            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }
};

class Node_func_translation : public Node_template_func
{
public:

    Node_func_translation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) != true)
        {
            return;
        }

        glm::mat4 translation_matrix = glm::mat4(1.0);
        std::visit([&](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;

                if constexpr (
                    std::is_same_v<T1, glm::mat4>
                    )
                {
                    translation_matrix = arg1;
                }


            }, (*input_node[1]->value)[0]);


        for (unsigned int i = 0; i < (*input_node[0]->value).size(); i++)
        {
            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;


                    if constexpr (
                        std::is_same_v<T1, std::shared_ptr<Object_setting>>
                        )
                    {
                        for (unsigned int i = 0; i < arg1->object.meshes[0].vertices.size(); i++)
                        {
                            arg1->object.meshes[0].vertices[i].Position = translation_matrix * glm::vec4(arg1->object.meshes[0].vertices[i].Position, 1);

                        }
                    }
                    if constexpr (
                        std::is_same_v<T1, glm::vec3>
                        )
                    {
                        arg1 = translation_matrix * glm::vec4(arg1, 1);



                    }




                }, (*input_node[0]->value)[i]);
        }



        this->value = input_node[0]->value;
        this->value_vector_size = (*this->value).size();
        evaluated = true;

    }



private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            bool changed = false;



            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }
};


class Node_func_object_arrange : public Node_template_func
{
public:

    Node_func_object_arrange(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {
        if (check_input_node(2, this->evaluated, this->value) != true)
        {
            return;
        }

        std::vector<glm::mat4> translation_matrix_list;
        
        for (unsigned int i = 0; i < (*input_node[1]->value).size(); i++)
        {

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (
                        std::is_same_v<T1, glm::mat4>
                        )
                    {
                        translation_matrix_list.push_back(arg1);
                    }


                }, (*input_node[1]->value)[i]);

        }


        for (unsigned int i = 0; i < (*input_node[0]->value).size(); i++)
        {
            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (
                        std::is_same_v<T1, std::shared_ptr<Object_setting>>
                        )
                    {
                        std::vector<Vertex> new_vertices;
                        std::vector<int> new_indices;

                        for (unsigned int i = 0; i < translation_matrix_list.size(); i++)
                        {
                            // copy vertices
                            for (unsigned int j = 0; j < arg1->object.meshes[0].vertices.size(); j++)
                            {
                                Vertex new_vertex = arg1->object.meshes[0].vertices[j];
                                new_vertex.Position = translation_matrix_list[i] * glm::vec4(arg1->object.meshes[0].vertices[j].Position, 1);

                                new_vertices.emplace_back(new_vertex);
                                



                            }

                            // copy indices 
                            for (unsigned int j = 0; j < arg1->object.meshes[0].indices.size(); j++)
                            {
                                new_indices.push_back(arg1->object.meshes[0].indices[j] + i * arg1->object.meshes[0].vertices.size());



                            }




                        }
                        arg1->object.meshes[0].indices.clear();
                        arg1->object.meshes[0].vertices.clear();
                        arg1->object.meshes[0].indices.insert(arg1->object.meshes[0].indices.begin(), new_indices.begin(), new_indices.end());
                        arg1->object.meshes[0].vertices.insert(arg1->object.meshes[0].vertices.begin(), new_vertices.begin(), new_vertices.end());


                    }



                }, (*input_node[0]->value)[i]);
        }



        this->value = input_node[0]->value;
        this->value_vector_size = (*this->value).size();
        evaluated = true;

    }



private:


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();

            bool changed = false;



            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }
};


class Node_func_half_two_circles : public Node_template_func
{
public:

    std::vector<glm::vec3> initial_vertices;

    float radius = 0.1;
    int vertices_num = 10;
    float distance = 0.1;

    Node_func_half_two_circles(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {}

    void evaluate() override
    {

        if (check_input_node(3, this->evaluated, this->value) != true)
        {
            return;
        }

        std::visit([this](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;

                if constexpr (std::is_same_v<T1, float>)
                {
                    this->radius = arg1; // set radius
                }

            }, (*input_node[0]->value)[0]);

        std::visit([this](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;

                if constexpr (std::is_same_v<T1, int>)
                {
                    this->vertices_num = arg1; // set verticse num
                }

            }, (*input_node[1]->value)[0]);

        std::visit([this](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;

                if constexpr (std::is_same_v<T1, float>)
                {
                    this->distance = arg1; // half circles distance
                }

            }, (*input_node[2]->value)[0]);


        this->initial_vertices = generate_two_half_circles(radius, vertices_num, distance);



        value->clear();
        for (auto& v : initial_vertices)
        {

            value->emplace_back(v);
        }
        evaluated = true;
        this->value_vector_size = initial_vertices.size();


    }
    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {

        if (this->value != nullptr)
        {

            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {
                std::visit([this](auto&& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;

                        if constexpr (std::is_same_v<T1, glm::vec3>)
                        {

                            debug.set_point_group("vec3", arg1);

                        }

                    }, (*this->value)[i]);

            }

        }



        return true;

    }



private:
    std::vector<glm::vec3> generate_two_half_circles(float radius, int num_vertices, float distance)
    {
        std::vector<glm::vec3> return_vertices;
        return_vertices.reserve(num_vertices);

        const float PI = 3.14159265358979323846f;

        int half_count = num_vertices / 2;

        for (int i = 0; i <= half_count; ++i)
        {
            float angle = PI * i / half_count; // 0 ~ π
            float x = radius * std::cos(angle);
            float z = distance * 0.5f + radius * std::sin(angle);

            return_vertices.emplace_back(x, 0.0f, z);
        }

        for (int i = 0; i <= half_count; ++i)
        {
            float angle = PI * i / half_count; // 0 ~ π
            float x = -radius * std::cos(angle);
            float z = -distance * 0.5f - radius * std::sin(angle);

            return_vertices.emplace_back(x, 0.0f, z);
        }

        return return_vertices;
    }
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {


            base_imgui();

            bool changed = false;


            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }
    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, float t)
    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);

        return interpolate.do_interpolate(A, B, t);
    }

    template <typename T>
    T make_bezier_curve(const T& P0, const T& P1, const T& P2, const T& P3, float t)

    {
        Interpolate interpolate;

        T A = interpolate.do_interpolate(P0, P1, t);
        T B = interpolate.do_interpolate(P1, P2, t);
        T C = interpolate.do_interpolate(P2, P3, t);

        T D = interpolate.do_interpolate(A, B, t);
        T E = interpolate.do_interpolate(B, C, t);


        return interpolate.do_interpolate(D, E, t);




    }




};
class Node_func_find_projection_point : public Node_template_func
{
public:


    Node_func_find_projection_point(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {
    }
    void evaluate() override
    {

        if (check_input_node(2, this->evaluated, this->value) != true)
        {
            evaluated = false;
            return;
        }


        std::vector<int> vertex_num;
        std::vector<glm::vec3> vertex_list;

        auto& it0 = (*input_node[0]->value)[0]; // projected object

        for (unsigned int i = 0; i < input_node[1]->value->size(); i++)
        {
            std::visit([&](auto&& arg0) {
                using T1 = std::decay_t<decltype(arg0)>;
                if constexpr (std::is_same_v<T1, glm::vec3>)
                {
                    vertex_list.push_back(arg0);

                }
                }, (*input_node[1]->value)[i]);
        }



        std::visit([&](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;



                if constexpr (
                    std::is_same_v<T1, std::shared_ptr<Object_setting>> 
                    )
                {

                    std::vector<glm::vec3> target_object_vertex;
                    std::vector<int> target_object_indices;
                    for (auto& vertex : arg1->object.meshes[0].vertices)
                    {
                        target_object_vertex.emplace_back(vertex.Position);
                    }
                    for (auto& index : arg1->object.meshes[0].indices)
                    {
                        target_object_indices.emplace_back(index);
                    }

                    std::vector<glm::vec3> new_vec3_list;


                    new_vec3_list = act_projection(target_object_vertex, target_object_indices, vertex_list);


                    value->clear();
                    for (unsigned int i = 0; i < new_vec3_list.size(); i++)
                    {

                        if (Draw_check)
                        {
                            for (unsigned int j = 1; j < 6; j++)
                            {
                                float t = 0.2 * j;

                                glm::vec3 dir = new_vec3_list[i] - vertex_list[i];
                                glm::vec3 draw_point = vertex_list[i] + dir * t;
                                debug.set_arrow_group("projection point", vertex_list[i], new_vec3_list[i]);

                            }

                        }
                        value->push_back(new_vec3_list[i]);
                    }


                }
                else
                {
                    return;
                }

            }, it0);  // 

        this->value_vector_size = (*this->value).size();
        evaluated = true;



    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();
            bool changed = false;
            if (changed)
            {
                evaluated = false;
            }
        }
        ImGui::EndChild();


    }


    std::vector<glm::vec3> act_projection(
        const std::vector<glm::vec3>& input_mesh_vertices,
        const std::vector<int>& input_mesh_indices,
        std::vector<glm::vec3> project_vec3
     
    )
    {
        int minus_count = 0;

        std::vector<glm::vec3> new_output_vec3_list;

        for (unsigned int i = 0; i < project_vec3.size(); i++)
        {
            int closest_triangle = -1;
            glm::vec3 closest_position;

            if (findClosestPointOnMesh(input_mesh_vertices, input_mesh_indices, project_vec3[i], closest_triangle, closest_position))
            {
                new_output_vec3_list.emplace_back(closest_position);

            }
            else
            {



                new_output_vec3_list.emplace_back(project_vec3[i]);


            }



        }


        return new_output_vec3_list;

    }


    bool findClosestPointOnMesh(
        const std::vector<glm::vec3>& vertices,
        const std::vector<int>& indices,
        glm::vec3& point,
        int& outTriangleIndex,
        glm::vec3& outClosestPoint)
    {
        float minDistSq = std::numeric_limits<float>::max();
        glm::vec3 closest;

        bool have_q = false;
        // i increase 3, 
        // when i > indices size  crush,
        for (size_t i = 0; i < indices.size() - 2; i += 3)
        {
            const glm::vec3& a = vertices[indices[i]];
            const glm::vec3& b = vertices[indices[i + 1]];
            const glm::vec3& c = vertices[indices[i + 2]];

            glm::vec3 q = closestPointOnTriangle(point, a, b, c);
            float distSq = glm::length(point - q);

            if (distSq < 0.3)
            {
                if (distSq < minDistSq)
                {
                    minDistSq = distSq;
                    closest = q;
                    outTriangleIndex = static_cast<int>(i / 3);
                    outClosestPoint = q;
                    have_q = true;
                }

            }
        }
        return have_q;


    }

    glm::vec3 closestPointOnTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        const glm::vec3 ab = b - a;
        const glm::vec3 ac = c - a;
        const glm::vec3 ap = p - a;

        float d1 = glm::dot(ab, ap);
        float d2 = glm::dot(ac, ap);

        if (d1 <= 0.0f && d2 <= 0.0f) return a; // Bary(1,0,0)

        glm::vec3 bp = p - b;
        float d3 = glm::dot(ab, bp);
        float d4 = glm::dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return b; // Bary(0,1,0)

        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + v * ab; // Bary(1 - v, v, 0)
        }

        glm::vec3 cp = p - c;
        float d5 = glm::dot(ab, cp);
        float d6 = glm::dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return c; // Bary(0,0,1)

        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + w * ac; // Bary(1 - w, 0, w)
        }

        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b); // Bary(0, 1 - w, w)
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w; // 내부
    }




};




#endif // ! NODE_FUNC_H
