#ifndef NODE_TEMPLATE_H
#define NODE_TEMPLATE_H

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm> // std::clamp
#include <iostream>
#include "GraphEditor.h"
#include "framemanager.h"
#include "Object_setting.h"
#include <glm/gtx/string_cast.hpp>

#include "MySequencer.h"


extern Frame_manager frame_manager;

template<typename T>
constexpr bool check_math_type =
std::is_arithmetic_v<T> ||
std::is_same_v<T, glm::vec2> ||
std::is_same_v<T, glm::vec3> ||
std::is_same_v<T, glm::vec4> ||
std::is_same_v<T, glm::mat3> ||
std::is_same_v<T, glm::mat4>;

template<typename T, typename U, typename = void>
struct is_math_compatible : std::false_type {};

template<typename T, typename U>
struct is_math_compatible<T, U, std::void_t<
    decltype(std::declval<T>() + std::declval<U>()),
    decltype(std::declval<T>() - std::declval<U>()),
    decltype(std::declval<T>()* std::declval<U>()),
    decltype(std::declval<T>() / std::declval<U>())
    >> : std::true_type {};

template<typename T>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

template <typename T2>
struct is_math_vector : std::false_type {};

template <typename U2>
struct is_math_vector<std::vector<U2>> : is_math_compatible<U2, U2> {};



class Node
{
public:
    std::string name;
    int ID_num;
    GraphEditor::TemplateIndex templateIndex;
    float x, y;
    bool mSelected;
    bool Draw_check = false;
    
    std::vector<std::shared_ptr<Node>> input_node;

    bool evaluated = false;

    std::shared_ptr<std::vector<ValueVariant>> value = std::make_shared<std::vector<ValueVariant>>();

    std::shared_ptr<std::vector<MySequenceItem>> key_frame_items = std::make_shared<std::vector<MySequenceItem>>();
    std::shared_ptr<std::vector<RampEdit>> rampEdit = std::make_shared<std::vector<RampEdit>>();



    int value_vector_size = 1;

    std::vector<int> select_vertex_num;
    int output_node_num = 0;
    int now_output_count = 0;

    Node(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) :name(name), ID_num(ID_num), templateIndex(templateIndex), x(x), y(y), mSelected(mSelected)
    {

    }

    virtual void evaluate() = 0;
    virtual void update() {};
    virtual void imgui_render();
    virtual void initial_setting(const std::vector<ValueVariant>& initial_value)
    {

    }

    virtual std::vector<ValueVariant> get_initial_value()
    {
        std::vector<ValueVariant> new_value;
        return new_value;
    }


    virtual bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position)
    {
        if (value != nullptr && this->evaluated)
        {
            for (auto& it : *value)
            {

                std::visit([this, projection, view, camera_position](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) 
                    {
                        arg1->wolrd_object_Draw(projection, view, camera_position);
                        return true;
                    }
                    else
                        return false;
                    }, it);  // 
            }

        }
        else
            return false;



    }

    virtual std::vector<glm::vec3> return_vertex_pos()
    {
        if (value != nullptr)
        {
            auto& it = *value;
            if (it.size() > 0)
            {

                return std::visit([this](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                        std::vector<glm::vec3> new_pos;
                        for (unsigned int i = 0; i < arg1->object.meshes[0].vertices.size(); i++)
                        {
                            new_pos.emplace_back(arg1->object.meshes[0].vertices[i].Position);

                        }

                        return new_pos;

                    }
                    else
                        return std::vector<glm::vec3>(0);


                    }, it[0]);  // 
            }

        }
        else
            return std::vector<glm::vec3>(0);


    }

    virtual glm::mat4 return_transform
    (
        int vertex_num
    )
    {
        if (value != nullptr)
        {
            auto& it = *value;
            if (it.size() > 0)
            {

                return std::visit([this, vertex_num](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                        glm::mat4 model = glm::mat4(1);
                        return glm::translate(model, arg1->object.meshes[0].vertices[vertex_num].Position);

                    }
                    return glm::mat4(1);
                    }, it[0]);  // 
            }
            return glm::mat4(1);
        }
        else
            return glm::mat4(1);


    }

    // For code stability, this function checks whether the node contains keyframe data.
    virtual bool have_keyframe_check()
    {
        return false;
    }
    virtual std::shared_ptr<std::vector<MySequenceItem>> return_sequence_Items()
    {
        return key_frame_items;
    }

    virtual std::shared_ptr<std::vector<RampEdit>> return_sequence_rampEdit()
    {
        return rampEdit;
    }

    virtual std::vector<ValueVariant> get_key_frame_item()
    {
        std::vector<ValueVariant> new_value;

        for (unsigned int i = 0; i < key_frame_items->size(); i++)
        {
            new_value.emplace_back((*key_frame_items)[i].mType); //int 
            new_value.emplace_back((*key_frame_items)[i].mFrameStart); //int 
            new_value.emplace_back((*key_frame_items)[i].mFrameEnd); //int 

            new_value.emplace_back((*key_frame_items)[i].max_value); //float 
            new_value.emplace_back((*key_frame_items)[i].max_value); //float 

            int new_int = (*key_frame_items)[i].mExpanded; // bool ->int
            new_value.emplace_back(new_int); //bool

        }
        return new_value;
    }
    virtual std::vector<ValueVariant> get_key_frame_rampEdit()
    {
        std::vector<ValueVariant> new_value;

        for (unsigned int i = 0; i < key_frame_items->size(); i++)
        {

            glm::vec2 new_vec2 = glm::vec2((*rampEdit)[i].mMax[0], (*rampEdit)[i].mMax[1]);
            new_value.emplace_back(new_vec2); //ImVec2
            new_vec2 = glm::vec2((*rampEdit)[i].mMin[0], (*rampEdit)[i].mMin[1]);
            new_value.emplace_back(new_vec2); //ImVec2

            int curve_num = (*rampEdit)[i].mPts.size();

            new_value.emplace_back(curve_num); //int

            for (unsigned int j = 0; j < curve_num; j++)
            {
                int point_num = (*rampEdit)[i].mPts[j].size();
                new_value.emplace_back(point_num); //int
            }



            for (unsigned int j = 0; j < curve_num; j++)
            {
                for (unsigned int k = 0; k < (*rampEdit)[i].mPts[j].size(); k++)
                {
                    new_vec2 = glm::vec2((*rampEdit)[i].mPts[j][k][0], (*rampEdit)[i].mPts[j][k][1]);
                    new_value.emplace_back(new_vec2); // ImVec2
                }
            }

        }



        return new_value;
    }



    virtual void initial_setting_key_frame(
        const std::vector<ValueVariant>& items,
        const std::vector<ValueVariant>& ramp)
    {
        (*key_frame_items).clear();

            
        for (unsigned int i = 0; i < items.size(); i+=6)
        {
            int mType;
            int mFrameStart;
            int mFrameEnd;
            float max_value;
            float min_value;
            bool mExpanded;



            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, int>)
                    {
                        mType = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i]);
            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, int>)
                    {
                        mFrameStart = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i+1]);

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, int>)
                    {
                        mFrameEnd = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i+2]);

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, float> )
                    {
                        max_value = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i+3]);

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, float>)
                    {
                        min_value = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i+4]);
            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, int>)
                    {
                        mExpanded = arg1;
                    }
                    else
                    {
                        std::cout << "error : by key_frame item" << std::endl;
                        return;
                    }


                }, items[i + 5]);

            (*key_frame_items).push_back(MySequenceItem{ mType, mFrameStart,mFrameEnd,max_value, min_value, mExpanded });
        }

        (*rampEdit).resize((*key_frame_items).size(), RampEdit());
        int ramp_vector_index = 0;
        for (unsigned int ramp_loop = 0; ramp_loop < (*key_frame_items).size(); ramp_loop++)
        {


            (*rampEdit)[ramp_loop].mPts.clear();

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, glm::vec2>)
                    {

                        (*rampEdit)[ramp_loop].mMax = ImVec2(arg1[0], arg1[1]);
                        ramp_vector_index++;
                    }
                    else
                    {
                        std::cout << "error : by key_frame ramp" << std::endl;
                        return;
                    }


                }, ramp[ramp_vector_index]);



            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, glm::vec2>)
                    {

                        (*rampEdit)[ramp_loop].mMin = ImVec2(arg1[0], arg1[1]);
                        ramp_vector_index++;
                    }
                    else
                    {
                        std::cout << "error : by key_frame ramp" << std::endl;
                        return;
                    }


                }, ramp[ramp_vector_index]);



            int curve_num = 0;

            std::visit([&](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v <T1, int>)
                    {

                        curve_num = arg1;
                        ramp_vector_index++;
                    }
                    else
                    {
                        std::cout << "error : by key_frame ramp" << std::endl;
                        return;
                    }


                }, ramp[ramp_vector_index]);

            std::vector<int> curve_point_num;
            int start_index = ramp_vector_index;

            for (unsigned int i = start_index; i < start_index + curve_num; i++)
            {

                std::visit([&](auto&& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v <T1, int>)
                        {

                            curve_point_num.push_back(arg1);
                            ramp_vector_index++;
                        }
                        else
                        {
                            std::cout << "error : by key_frame ramp" << std::endl;
                            return;
                        }


                    }, ramp[ramp_vector_index]);
            }


            for (unsigned int i = 0; i < curve_point_num.size(); i++)
            {
                std::vector<ImVec2> new_Pts;
                for (unsigned int j = 0; j < curve_point_num[i]; j++)
                {
                    std::visit([&](auto&& arg1)
                        {
                            using T1 = std::decay_t<decltype(arg1)>;
                            if constexpr (std::is_same_v <T1, glm::vec2>)
                            {

                                new_Pts.push_back(ImVec2(arg1[0], arg1[1]));
                                ramp_vector_index++;
                            }
                            else
                            {
                                std::cout << "error : by key_frame ramp" << std::endl;
                                return;
                            }


                        }, ramp[ramp_vector_index]);


                }

                (*rampEdit)[ramp_loop].mPts.emplace_back(new_Pts);

            }

        }

       



    }




    virtual void set_transform
    (
        const glm::mat4& new_matrix,
        int vertex_num
    )
    {


    }
protected:

    void base_imgui()
    {
        ImGui::Text((name).c_str());
        if (evaluated)
        {
            ImGui::Text("evaluated!");
        }
        else
        {
            ImGui::Text("not evaluated!");
        }

        ImGui::Text("xy : %3f , %3f", x, y);
        ImGui::Text("vector size : %d", value_vector_size);
        ImGui::Checkbox("Draw", &Draw_check);

        ImGui::Text("output num %d", output_node_num);

    }


    bool Draw_vector_vec3(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            glm::vec3 temp_vec3_1 = glm::vec3(0.0);
            glm::vec3 temp_vec3_2;
            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {



                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, glm::vec3>) {


                        temp_vec3_2 = arg1;

                        debug.set_point_group((name+"point").c_str(), temp_vec3_2);
                        if (i > 0)
                        {
                            debug.set_line_group((name + "line").c_str(), temp_vec3_1, temp_vec3_2);
                        }


                        temp_vec3_1 = temp_vec3_2;


                        return true;

                    }
                    else
                    {
                        return false;


                    }
                    }, (*this->value)[i]);  // 
            }


        }
        return false;

    }

    bool check_input_node(int least_input, bool& evaluated, std::shared_ptr<std::vector<ValueVariant>> value)
    {
        evaluated = false;

        if (input_node.size() < least_input)
        {
            evaluated = false;
            value = nullptr;
            return false;
        }

        bool all_input_node_evaluated = true;

        for (auto it : input_node)
        {
            if (it == nullptr)
            {
                evaluated = false;
                value = nullptr;
                return false;
            }

            if (it->evaluated == false)
            {
                evaluated = false;
                value = nullptr;
                return false;
            }

            if (it->value->size() < 1)
            {
                evaluated = false;
                value = nullptr;
                return false;
            }
            
        }

        evaluated = true;
        return true;

    }




};

class Node_template_func : public Node
{
public:

    Node_template_func(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {


    }
private:

    void evaluate()
    {};


};

class Node_template_value : public Node
{
public:

    Node_template_value(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {

    }


    void evaluate() override
    {};

};

class Node_object : public Node
{
public:

    std::shared_ptr<Object_setting> initial_object;
    std::shared_ptr<Object_setting> now_object;
    std::string object_path;

    Node_object(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Object_setting> object_setting,
        int object_count,
        std::shared_ptr<Shader> shader



    ) : Node(name, ID_num, templateIndex, x, y, mSelected),
        initial_object(std::make_shared<Object_setting>(
            *object_setting,
            object_count,
            shader
        )),
        now_object(std::make_shared<Object_setting>(
            *object_setting,
            object_count,
            shader
        )),
        object_path(initial_object->path)
    {

        auto& it = *value;
        it.push_back(now_object);
    }

    void evaluate() override
    {
        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true ) 
            {
                if (input_node[0]->value->size() == 1)
                {
                    int curve_point_num;
                    std::vector<glm::vec3> curve_point_vec3;

                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v<T1, glm::mat4>) {
                            initial_object->object.position =  glm::vec3(arg1[3]);

                            //initial_object->object.scale.x = glm::length(glm::vec3(arg1[0]));  // X axis
                            //initial_object->object.scale.y = glm::length(glm::vec3(arg1[1]));  // Y axis
                            //initial_object->object.scale.z = glm::length(glm::vec3(arg1[2]));  // Z axis

                            glm::mat3 rotation_matrix;
                            rotation_matrix[0] = glm::vec3(arg1[0]) / initial_object->object.scale.x;
                            rotation_matrix[1] = glm::vec3(arg1[1]) / initial_object->object.scale.y;
                            rotation_matrix[2] = glm::vec3(arg1[2]) / initial_object->object.scale.z;
                            glm::quat rotation = glm::quat_cast(rotation_matrix);

                            initial_object->object.quaternion.w = -rotation.w;
                            initial_object->object.quaternion.x = rotation.x;
                            initial_object->object.quaternion.y = rotation.y;
                            initial_object->object.quaternion.z = rotation.z;


                        }
                        }, (*input_node[0]->value)[0]);  

                }
            }
        }





        *now_object = *initial_object;
        evaluated = true;
    };

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            ImGui::Checkbox("Draw", &Draw_check);



        }
        ImGui::EndChild();



    }


    void set_transform
    (
        const glm::mat4& new_matrix,
        int vertex_num
    ) override
    {

        glm::vec3 new_pos = new_matrix * glm::vec4(0, 0, 0, 1);
        now_object->object.meshes[0].vertices[vertex_num].Position = new_pos;

        evaluated = false;


    }



};

class Node_animation : public Node
{
public:
    std::shared_ptr<Animation> initial_value;
    std::vector<glm::mat4> bone_matrices;

    int frame = 0;
    float float_t = 0;
    Node_animation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Animation> animation



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {

        bone_matrices.reserve(animation->m_BoneInfoMap.size());
        for (int i = 0; i < animation->m_BoneInfoMap.size(); i++)
            bone_matrices.push_back(glm::mat4(1.0f));


 
        initial_value = std::make_shared<Animation>(
            *animation
        );


    }

    void evaluate() override
    {
        if (check_input_node(1, this->evaluated, this->value) == false)
        {
            this->value = nullptr;
            return;
        }
        this->value = input_node[0]->value;
        frame = frame_manager.now_frame;
        std::visit([this](auto&& arg1) {
            using T1 = std::decay_t<decltype(arg1)>;
            if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>)
            {
                real_act(arg1->object);
                arg1->object.final_bone_matrix.clear();

                for (auto& it : bone_matrices)
                {
                    arg1->object.final_bone_matrix.emplace_back(it);
                }
                evaluated = true;
            }

            }, (*this->value)[0]);  // 
    }

    void real_act(Model& model)
    {
        if (this->initial_value == nullptr)
        {
            debug.set_text("error : don't have animation in this model", 0.1);
        }
        else
        {
            CalculateBoneTransform(this->initial_value, bone_matrices, &this->initial_value->GetRootNode(), glm::mat4(1.0f));
        }

    }

    void CalculateBoneTransform(
        std::shared_ptr<Animation> animation,
        std::vector<glm::mat4>& m_FinalBoneMatrices,
        const AssimpNodeData* node,
        glm::mat4 parentTransform
    )
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone2* Bone = animation->FindBone(nodeName);

        float animation_time = frame * 2.0;


        static float animation_dt_temp = 5.50;


        animation_time *= animation_dt_temp;
        animation_time = std::fmod(animation_time, animation->GetDuration());



        if (Bone)
        {

            Bone->Update(animation_time);
            nodeTransform = Bone->GetLocalTransform();
        }


        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            static float temp_scale = 1;
            glm::mat4 offset = boneInfoMap[nodeName].offset * temp_scale;


            static float arm_angle_x = 90;
            static float arm_angle_y = 0;
            static float arm_angle_z = 90;

            offset = glm::scale(offset, glm::vec3(temp_scale));

            m_FinalBoneMatrices[index] = globalTransformation * offset;



        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(animation, m_FinalBoneMatrices, &node->children[i], globalTransformation);
    }


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();
            if (ImGui::SliderInt("frame", &frame, 0, 200))
            {
                evaluated = false;
            }

            ImGui::SliderFloat("0~1 t", &float_t, 0, 1);



        }
        ImGui::EndChild();



    }

};

class Node_shader : public Node
{
public:

    glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.31f,1);



    std::shared_ptr<Shader> initial_shader;
    //std::shared_ptr<Shader> now_shader;

    Node_shader(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Shader> shader



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        //this->initial_shader = shader;
        this->initial_shader = std::make_shared<Shader>(*shader);

    }

    void evaluate() override
    {
        this->value->clear();

        if (initial_shader != nullptr)
        {


            this->initial_shader->use();
            
            this->initial_shader->setVec3("objectColor", color);
            this->value->emplace_back(initial_shader);



            evaluated = true;
        }
        else
            evaluated = false;
    }


    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();


        for (unsigned int i = 0; i < initial_value.size(); i++)
        {
            std::visit([this, i](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, glm::vec4>)
                    {
                        color = arg1;

                    }


                }, initial_value[i]);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.emplace_back(color);


        return this_value;
    }


private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());
            
            //ImGui::Checkbox("Draw", &Draw_check);



            if (ImGui::Button("change color"))
            {
                this->color = inputctrl_global->color;
            }



        }
        ImGui::EndChild();



    }

};




class Node_object_setting : public Node
{
public:

    std::shared_ptr<Shader> now_shader;


    Node_object_setting(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Shader> shader



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        now_shader = shader; // when node generate, the first shader is nomal shader

    }

    void evaluate() override
    {

        if (check_input_node(1, this->evaluated, this->value) == false)
        {
            return;
        }

        this->value->clear();

        if (input_node.size() >= 2)
        {
            if (input_node[1] != nullptr)
            {

                std::visit([&](auto&& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v <T1, std::shared_ptr<Shader>>)
                        {
                            now_shader = arg1;



                        }


                    }, (*input_node[1]->value)[0]);





            }

        }

        this->evaluated = true;


        std::visit([&](auto&& arg1)
            {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v <T1, std::shared_ptr<Object_setting>>)
                {

                    arg1->shader = now_shader;
                    this->value->emplace_back(arg1);




                }


            }, (*input_node[0]->value)[0]);





        evaluated = true;
    };

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();
        }
        ImGui::EndChild();



    }




};


class Node_keyframe_sequence : public Node
{
public:
    int data_type = 0;
    const char* type_name_list[3] = { "float", "vec3", "mat4" };

    Node_keyframe_sequence(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {
        
        (*this->key_frame_items).push_back(MySequenceItem{ 0, 0, 200,-1,1, false });
        
        RampEdit new_ramp = RampEdit();
        
        (*rampEdit).push_back(new_ramp);
    
    }

    void evaluate() override
    {
        if (check_input_node(0, this->evaluated, this->value) == false)
        {
            return;
        }



        this->value->clear();
        switch (data_type)
        {
        case 0:

            for (unsigned int i = 0; i < (*rampEdit).size(); i++)
            {

                if ((*rampEdit)[i].now_value.size() > 0)
                {
                    //std::cout << rampEdit->value[0] << std::endl;
                    this->value->push_back((*rampEdit)[i].now_value[0]);

                }
            }
 

            break;


        }




        


        if (input_node.size() >= 2)
        {
            if (input_node[1] != nullptr)
            {

                std::visit([&](auto&& arg1)
                    {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v <T1, std::shared_ptr<Shader>>)
                        {



                        }


                    }, (*input_node[1]->value)[0]);





            }

        }



        evaluated = true;
    };

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            bool changed = false;

            ImGui::Checkbox("Draw", &Draw_check);
            changed = ImGui::ListBox("type", &data_type, type_name_list, IM_ARRAYSIZE(type_name_list));


            if (changed)
            {

                switch (data_type)
                {
                case 0:
                {
                    (*key_frame_items).resize(1, MySequenceItem{ 0, 0, 200, -1, 1, false });
                    (*key_frame_items)[0].mExpanded = 1;

                    (*rampEdit).resize(1, RampEdit());
                    (*rampEdit)[0].mPts.resize(1);
                    break;
                }
                case 1:
                {
                    (*key_frame_items).resize(1, MySequenceItem{ 0, 0, 200, -1, 1, false });
                    (*rampEdit).resize(1, RampEdit());
                    if ((*rampEdit)[0].mPts.size() < 3)
                        (*rampEdit)[0].mPts.resize(3);

                    if ((*rampEdit)[0].mPts[0].empty()) 
                        (*rampEdit)[0].mPts[0] = {ImVec2(0.f, 0.5f), ImVec2(50.f, 0.5f), ImVec2(100.f, 0.5f)};
                    if ((*rampEdit)[0].mPts[1].empty()) 
                        (*rampEdit)[0].mPts[1] = {ImVec2(0.f, 0.3f), ImVec2(100.f, 0.7f)};
                    if ((*rampEdit)[0].mPts[2].empty()) 
                        (*rampEdit)[0].mPts[2] = {ImVec2(0.f, 1.0f), ImVec2(100.f, 0.0f)};



                    break;

                }
                case 2:
                {
                    (*key_frame_items).resize(3, MySequenceItem{ 0, 0, 200, -1, 1, false });
                    (*rampEdit).resize(3, RampEdit());

                    for (unsigned int i = 0; i < 3; i++)
                    {
                        if ((*rampEdit)[i].mPts.size() < 3)
                            (*rampEdit)[i].mPts.resize(3);

                        if ((*rampEdit)[i].mPts[0].empty())
                            (*rampEdit)[i].mPts[0] = { ImVec2(0.f, 0.5f), ImVec2(50.f, 0.5f), ImVec2(100.f, 0.5f) };
                        if ((*rampEdit)[i].mPts[1].empty())
                            (*rampEdit)[i].mPts[1] = { ImVec2(0.f, 0.3f), ImVec2(100.f, 0.7f) };
                        if ((*rampEdit)[i].mPts[2].empty())
                            (*rampEdit)[i].mPts[2] = { ImVec2(0.f, 1.0f), ImVec2(100.f, 0.0f) };
                    }

                    break;
                }

                    



                }




            }






        }
        ImGui::EndChild();



    }

    bool have_keyframe_check() override
    {
        return true;
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

                        this->data_type = arg1;

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        this_value.push_back(data_type);

        return this_value;
    }


private:





};





class Node_value_float : public Node_template_value
{
public:

    std::vector<float> initial_value = std::vector<float>(1,0.0f);

    Node_value_float(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {

        value->push_back(initial_value[0]);
    }

    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<float>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, float>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

            }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }



    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }

        evaluated = true;

    }

private:
    //void imgui_node();

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            base_imgui();
            bool changed = false;
            
            
            
            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }


            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed = ImGui::InputFloat((name + std::to_string(i)).c_str(), &initial_value[i], 0.1f, 1.0f, "%.3f");
            }

            if (changed)
            {
                evaluated = false;
            }

        }
        ImGui::EndChild();


    }


};




class Node_value_int : public Node_template_value
{
public:
    std::vector<int> initial_value = std::vector<int>(1,0);
    Node_value_int(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }

    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<int>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }

        evaluated = true;

    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }



            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed = ImGui::InputInt((name + std::to_string(i)).c_str(), &initial_value[i], 1,10);
            }

            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};




class Node_value_vec3 : public Node_template_value
{
public:
    std::vector<glm::vec3> initial_value = std::vector<glm::vec3>(1, glm::vec3(0.0));
    Node_value_vec3(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }
        evaluated = true;
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<glm::vec3>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, glm::vec3>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {

        for (unsigned int i = 0; i < value_vector_size; i++)
        {
            debug.set_point_group("vec3", initial_value[i]);
        }
        

        return true;

    }
    std::vector<glm::vec3> return_vertex_pos() override
    {

        return initial_value;


    }
    glm::mat4 return_transform(int vertex_num) override
    {
        glm::mat4 new_mat = glm::mat4(1);
        new_mat = glm::translate(new_mat, initial_value[vertex_num]);

        return new_mat;


    }
    void set_transform(const glm::mat4& new_matrix, int vertex_num) override
    {

        glm::vec3 new_pos = new_matrix * glm::vec4(0, 0, 0, 1);
        initial_value[vertex_num] = new_pos;

        evaluated = false;


    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();



            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }

            

            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed |= ImGui::InputFloat(("x" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][0], 0.01f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat(("y" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][1], 0.01f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat(("z" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][2], 0.01f, 1.0f, "%.3f");
                ImGui::Spacing();
                ImGui::Spacing();
            }

            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};



class Node_value_mat4 : public Node_template_value
{
public:
    std::vector<glm::mat4> initial_value = std::vector<glm::mat4>(1, glm::mat4(1.0));
    Node_value_mat4(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }
        evaluated = true;
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {
        this->value_vector_size = initial_value.size();
        std::vector<glm::mat4>().swap(this->initial_value);
        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, glm::mat4>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {

        for (unsigned int i = 0; i < value_vector_size; i++)
        {
            debug.set_axis("mat4", initial_value[i]);
        }


        return true;

    }
    std::vector<glm::vec3> return_vertex_pos() override
    {
        std::vector<glm::vec3> now_pos_list;
        for (auto& it : initial_value)
        {
            glm::vec3 now_pos = glm::vec3(it[3]);
            now_pos_list.emplace_back(now_pos);
        }

        return now_pos_list;


    }
    glm::mat4 return_transform(int vertex_num) override
    {
        return initial_value[vertex_num];
    }
    void set_transform(const glm::mat4& new_matrix, int vertex_num) override
    {

        initial_value[vertex_num] = new_matrix;

        evaluated = false;


    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();



            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if(ImGui::Button("matrix reset"))
            {
                for (unsigned int i = 0; i < initial_value.size(); i++)
                {
                    initial_value[i] = glm::mat4(1.0);

                }

            }

            if (value_vector_size <= 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
                initial_value.back() = glm::mat4(1.0f);
            }



            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};

/*
class Node_value_vector_vec3 : public Node_template_value
{
public:
    std::vector<glm::vec3> initial_value = std::vector<glm::vec3>(1,glm::vec3(0.0));
    int vector_size = 1;
    
    Node_value_vector_vec3(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {

        this->now_value = initial_value;
        value = std::make_shared<ValueVariant>(this->now_value);
    }

    void initial_setting(const ValueVariant& initial_value) override
    {

        std::visit([this](auto&& arg1) {
            using T1 = std::decay_t<decltype(arg1)>;

            //
            if constexpr (std::is_same_v<T1, std::vector<glm::vec3>>) {

                vector_size = arg1.size();
                this->initial_value = arg1;
            }

        }, initial_value);  // 
        
       

    }
    void update() override
    {

    }

    void evaluate() override
    {

        //initial_value.assign(vector_size, initial_vec3);

        *value = initial_value;
        evaluated = true;

    }

private:
    //void imgui_node();

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());


            bool changed = false;
            
            changed |= ImGui::SliderInt((name + "size").c_str(), &vector_size, 1, 100);

            if (changed)
            {
                initial_value.assign(vector_size, glm::vec3(0.0));
            }



            for (unsigned int i = 0; i < vector_size; i++)
            {
                ImGui::BeginChild((std::to_string(i)).c_str(), ImVec2(450, 100));
                {

                    changed |= ImGui::InputFloat((name + "x##" +std::to_string(i)).c_str(), &initial_value[i][0], 0.1f, 10.0f, "%.3f");
                    changed |= ImGui::InputFloat((name + "y##" + std::to_string(i)).c_str(), &initial_value[i][1], 0.1f, 10.0f, "%.3f");
                    changed |= ImGui::InputFloat((name + "z##" + std::to_string(i)).c_str(), &initial_value[i][2], 0.1f, 10.0f, "%.3f");
                }
                ImGui::EndChild();
            }


            if (changed)
            {
                evaluated = false;
            }

            /*
            for (int i = 0; i < vector_size; i++)
            {

                changed |= ImGui::InputFloat((name + "x").c_str(), &initial_value[i][0], 0.1f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat((name + "y").c_str(), &initial_value[i][1], 0.1f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat((name + "z").c_str(), &initial_value[i][2], 0.1f, 1.0f, "%.3f");



            }
            

            if (changed)
            {
                evaluated = false;
            }

        }
        ImGui::EndChild();


    }

};
*/




class Node_func_add : public Node_template_func
{
public:
    Node_func_add(
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

        auto& vec1 = *input_node[0]->value;
        auto& vec2 = *input_node[1]->value;

        if (vec2.size() == 1)
        {

            for (size_t i = 0; i < vec1.size(); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 + arg2;  // 
                    }
                    }, vec1[i], vec2[0]);

            }
        }
        else
        {

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 + arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }
        }




        evaluated = true;
        this->value_vector_size = (*this->value).size();
 

    }

private:
};

class Node_func_add_all : public Node_template_func
{
public:
    Node_func_add_all(
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

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 + arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }



    }

private:
};



class Node_func_subtract : public Node_template_func
{
public:
    Node_func_subtract(
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


        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 - arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }


    }




private:


};




class Node_func_divide : public Node_template_func
{
public:
    Node_func_divide(
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

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 / arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }



    }




private:


};




class Node_func_multiply : public Node_template_func
{
public:
    Node_func_multiply(
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

        auto& vec1 = *input_node[0]->value;
        auto& vec2 = *input_node[1]->value;

        for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
        {

            std::visit([this, i](auto&& arg1, auto&& arg2) {
                using T1 = std::decay_t<decltype(arg1)>;
                using T2 = std::decay_t<decltype(arg2)>;

                
                if constexpr (is_math_compatible<T1, T2>::value) {
                    (*input_node[0]->value)[i] = arg1 * arg2;
                }
                }, vec1[i], vec2[i]);


        }
        this->value = input_node[0]->value;
        evaluated = true;


    }




private:


};

/*
class Node_func_vector_multiply : public Node_template_func
{
public:
    Node_func_vector_multiply(
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

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            std::visit([this](auto&& arg1, auto&& arg2) {
                using T1 = std::decay_t<decltype(arg1)>;
                using T2 = std::decay_t<decltype(arg2)>;

                //
                if constexpr (is_math_compatible<T1, T2>::value) {
                    *this->value = arg1 * arg2;  // 
                }
                else if constexpr ()
                {


                }
                }, *input_node[0]->value, *input_node[1]->value);  // 
        }

        evaluated = true;
    }




private:


};

*/


class Node_func_result : public Node_template_func
{
public:
    Node_func_result(
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
        if (input_node.size() > 0)
            this->value = input_node[0]->value;

        evaluated = true;
    }

private:
    void imgui_render() override
    {
        ImGui::Text((name).c_str());
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            for (unsigned int i = 0; i < input_node.size(); i++)
            {
                ImGui::Text((input_node[i]->name).c_str());
            }           


            if (this->value != nullptr)
            {
                for (auto& it : *this->value)
                {
                    std::visit([](auto&& val)
                        {
                            using T = std::decay_t<decltype(val)>;
                            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
                            {
                                ImGui::Text("value : %.3f", val);
                            }
                            else if constexpr (std::is_same_v<T, int>)
                            {
                                ImGui::Text("value : %d", val);
                            }
                            else if constexpr (std::is_same_v<T, std::string>)
                            {
                                ImGui::Text("value : %s", val.c_str());
                            }
                            else if constexpr (std::is_same_v<T, glm::vec3>)
                            {
                                ImGui::Text("value: %s", glm::to_string(val).c_str());
                            }

                            else
                            {
                                ImGui::Text("value : (unsupported type)");
                            }
                        }, it);

                }


            }




        }
        ImGui::EndChild();

    }


};






class end_node : public Node
{
public:



private:


};
















#endif 
