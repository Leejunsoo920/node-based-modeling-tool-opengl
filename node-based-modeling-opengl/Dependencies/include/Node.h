#ifndef NODE_H
#define NODE_H

#include "grapheditordelegate.h"

//typedef std::vector<std::shared_ptr<Object_setting>> objects_t;
typedef std::map<std::string, std::shared_ptr<Object_setting>> objects_map_t;
typedef std::map<std::string, std::shared_ptr<Shader>> shader_map_t;


class Node_manager
{
public:
    Node_manager() 
    {
    }
    ~Node_manager() {}

    int draw_node_id = -1;
    int object_count = 0;

    std::filesystem::path project_path = std::filesystem::current_path();
    std::string current_file_path;




    MySequence mySequence = MySequence(0,200);
    //// resource list
    objects_map_t pre_object_map;
    objects_map_t object_map;
    objects_map_t pre_UI_object_map;
    shader_map_t pre_shader_map;
    std::map<std::string, std::shared_ptr<Animation>> pre_animation_map;

    void update(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& camera_position);
    void set_pre_UIobject(const char* path, const char* name, bool gamma = false)
    {
        std::shared_ptr<Object_setting> newobject = std::make_shared<Object_setting>(path, name, gamma);
        pre_UI_object_map.insert({ name, newobject });
    }
    void set_pre_object(const char* path,const  char* name, bool gamma = false)
    {
        std::shared_ptr<Object_setting> newobject = std::make_shared<Object_setting>(path, name, gamma);
        pre_object_map.insert({ name, newobject });
    }
    void set_pre_shader(const char* vertexPath, const char* fragmentPath, std::string name, const char* geometryPath = nullptr)
    {
        std::shared_ptr<Shader> new_shader = std::make_shared<Shader>(vertexPath, fragmentPath, name, geometryPath);
        pre_shader_map.insert({ name, new_shader });
    }
    void set_pre_animation(const char* path,const char* name, bool gamma = false)
    {
        //// get object
        std::shared_ptr<Object_setting> new_object = std::make_shared<Object_setting>(path, name, gamma);
        pre_object_map.insert({ name, new_object });
        //// if object have animation -> get animation
        if (new_object->object.m_BoneInfoMap.size() > 0)
        {
            std::shared_ptr<Animation> new_animation = std::make_shared<Animation>(path, name, new_object->object.m_BoneInfoMap, new_object->object.m_BoneCounter);
            pre_animation_map.insert({ name,new_animation });
        }


    }

    nlohmann::json serialize_initial_value(const std::vector<ValueVariant>& values);
    std::vector<ValueVariant> deserialize_initial_value(const nlohmann::json& jarray);


private:    
    GraphEditor::Options options;
    GraphEditorDelegate delegate;
    GraphEditor::ViewState viewState;
    GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;

    bool gen_node_click = false;
    bool showGraphEditor = true;
    bool first_run = true;

    void select_gen_node(objects_map_t& pre_object_map, const ImVec2& mouse_pos_when_click);
    void imgui_render();
    void imgui_node_property_render(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position);
    int select_node_count = 0;

    void save_all_node_by_json(std::string save_path);
    void load_all_node_by_json(std::string json_file_to_read);



    int check_vertex_num(const std::vector<glm::vec3> & vertices)
    {
        Vertex_Picking vertex_picking;
        vertex_picking.check_piking_distance(inputctrl_global->mouse_pos, vertices);
        return vertex_picking.picking_vertex_num;

    }
    void debug_UI_draw(
        const glm::mat4& projection,
        const glm::mat4& view)
    {

        /// draw UI object by instancing 
        if (debug.point_model_M.size() > 0)
        {
            // instancing draw object that can change color
            instancing_draw(
                pre_UI_object_map["UI_sphere"]->object.meshes[0],
                debug.point_instance_buffer,
                debug.point_color_buffer,
                debug.point_model_M,
                debug.point_color_M,
                projection,
                view
            );
        }
        if (debug.arrow_model_M.size() > 0)
        {
            // instancing draw object that can change color
            instancing_draw(
                pre_UI_object_map["yajirusi"]->object.meshes[0],
                debug.arrow_instance_buffer,
                debug.arrow_color_buffer,
                debug.arrow_model_M,
                debug.arrow_color_M,
                projection,
                view
            );
        }

        if (DebugUI::axis_model_M.size() > 0)
        {
            // instancing draw object that using their own texture
            instancing_draw(
                pre_UI_object_map["axis"]->object.meshes[0],
                debug.axis_instance_buffer,
                debug.axis_model_M,
                projection,
                view
            );
        }
        if (debug.line_model_M.size() > 0)
        {
            // instancing draw object that can change color

            instancing_draw(
                pre_UI_object_map["line"]->object.meshes[0],
                debug.line_instance_buffer,
                debug.line_color_buffer,
                debug.line_model_M,
                debug.line_color_M,
                projection,
                view
            );
        }
        
    }

    void instancing_draw(
        Mesh mesh,
        unsigned int& instance_buffer,
        unsigned int& color_buffer,
        std::vector<glm::mat4>& model_M, // to clear model_M in this function, call by reference
        std::vector<glm::vec3>& color_M, // to clear color_M, call by reference
        const glm::mat4& projection,
        const glm::mat4& view)
    {
        //set size by model matrix size
        int M_size = model_M.size();


        // bind model 4x4 matrix to buffer
        glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
        glBufferData(GL_ARRAY_BUFFER, M_size * sizeof(glm::mat4), &model_M[0], GL_STATIC_DRAW);

        glBindVertexArray(mesh.VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        // set color to buffer
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(GL_ARRAY_BUFFER, M_size * sizeof(glm::vec3), &color_M[0], GL_STATIC_DRAW);

        glBindVertexArray(mesh.VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glVertexAttribDivisor(2, 1);

        glBindVertexArray(0);

        pre_shader_map["instancingshader"]->use();
        pre_shader_map["instancingshader"]->setMat4("view", view);
        pre_shader_map["instancingshader"]->setMat4("projection", projection);

        glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0, M_size);
        glGetError();
        glBindVertexArray(0);


        // reset matrix
        model_M.clear();
        color_M.clear();

        model_M.shrink_to_fit();
        color_M.shrink_to_fit();




    }
    void instancing_draw(
        Mesh mesh,
        unsigned int& instance_buffer,
        std::vector<glm::mat4>& model_M, // to clear model_M in this function, call by reference
        const glm::mat4& projection,
        const glm::mat4& view
    )
    {
        //set size by model matrix size
        int M_size = model_M.size();
        // bind model 4x4 matrix to buffer
        glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
        glBufferData(GL_ARRAY_BUFFER, M_size * sizeof(glm::mat4), &model_M[0], GL_STATIC_DRAW);

        //
        glBindVertexArray(mesh.VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);


        pre_shader_map["instancing_texture shader"]->use();
        pre_shader_map["instancing_texture shader"]->setMat4("view", view);
        pre_shader_map["instancing_texture shader"]->setMat4("projection", projection);
        pre_shader_map["instancing_texture shader"]->setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.textures[0].id);


        glBindVertexArray(mesh.VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0, M_size);
        glBindVertexArray(0);


        // reset matrix
        model_M.clear();
        model_M.shrink_to_fit();
    }
};



#endif