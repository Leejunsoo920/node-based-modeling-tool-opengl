#ifndef UI_H
#define UI_H
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_DEFINE_MATH_OPERATORS

#include <algorithm>
#include <vector>
#include <string.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_toggle.h"
#include "ImGuizmo.h"

#include "camera.h"
#include "window.h"
#include "Node.h"


// system logic run in all modes -> defined in the base class "Mode"
// vertex num check to vertex picking 
int Vertex_Picking::picking_vertex_num = -1;

class Mode
{
public:
    std::shared_ptr<Window> window;
    std::shared_ptr<Camera> camera;
    Mode(std::shared_ptr<Window> window, std::shared_ptr<Camera> camera)
    {
        this->window = window;
        this->camera = camera;
    }
    ~Mode() {}

    void inputctrl_update()
    {

        ImGui::Text("%.2f FPS", 1 / inputctrl_global->deltaTime);
        ImGui::Text("mouse xy(%d", inputctrl_global->mouse_pos[0]);
        ImGui::SameLine();
        ImGui::Text(",%d)", inputctrl_global->mouse_pos[1]);

        float win_x = 2 * inputctrl_global->mouse_pos[0] / inputctrl_global->scene_width - 1;
        float win_y = 1 - 2 * inputctrl_global->mouse_pos[1] / inputctrl_global->scene_height;

        glm::mat4 un_view_M = glm::inverse(window->projection * camera->view);
        glm::vec4 near_point_4 = un_view_M * glm::vec4(win_x, win_y, -1, 1);
        glm::vec4 far_point_4 = un_view_M * glm::vec4(win_x, win_y, 1, 1);

        glm::vec3 near_point;
        near_point.x = near_point_4.x / near_point_4.w;
        near_point.y = near_point_4.y / near_point_4.w;
        near_point.z = near_point_4.z / near_point_4.w;
        glm::vec3 far_point;
        far_point.x = far_point_4.x / far_point_4[3];
        far_point.y = far_point_4.y / far_point_4[3];
        far_point.z = far_point_4.z / far_point_4[3];

        inputctrl_global->near_point = near_point;
        inputctrl_global->far_point = far_point;
        inputctrl_global->mouse_point_direction = glm::normalize(far_point - near_point);
    }


    void imgui_base_render()
    {

        if (inputctrl_global->push_P != true)
            ImGui::Begin("BASE", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        else
            ImGui::Begin("BASE");

        ImGui::Text("%.2f FPS", 1 / inputctrl_global->deltaTime);
        ImGui::Text("mouse xy(%d", inputctrl_global->mouse_pos[0]);
        ImGui::SameLine();
        ImGui::Text(",%d)", inputctrl_global->mouse_pos[1]);

        Set_ray set_ray;

        inputctrl_global->near_point = set_ray.calculate_ray_for_nearpoint_by_pixel(
            window->projection,
            camera->view,
            inputctrl_global->mouse_pos[0],
            inputctrl_global->mouse_pos[1],
            inputctrl_global->scene_width,
            inputctrl_global->scene_height

        );
        inputctrl_global->far_point = set_ray.calculate_ray_for_farpoint_by_pixel(

            window->projection,
            camera->view,
            inputctrl_global->mouse_pos[0],
            inputctrl_global->mouse_pos[1],
            inputctrl_global->scene_width,
            inputctrl_global->scene_height

        );


        inputctrl_global->mouse_point_direction = glm::normalize(inputctrl_global->far_point - inputctrl_global->near_point);

        ImGui::Text("w_mouse(%.3f", inputctrl_global->mouse_pos_wolrd_pos[0]);
        ImGui::SameLine();
        ImGui::Text("%.3f %.3f)", inputctrl_global->mouse_pos_wolrd_pos[1], inputctrl_global->mouse_pos_wolrd_pos[2]);
        ImGui::Text("camera(%.2f %.2f %.2f)", camera->position[0], camera->position[1], camera->position[2]);
        ImGui::Text("camera ray (%.2f %.2f %.2f)", inputctrl_global->mouse_point_direction[0], inputctrl_global->mouse_point_direction[1], inputctrl_global->mouse_point_direction[2]);
        if (ImGui::Button("reset camera"))
            camera->camera_reset();
        ImGui::Text("scene pos %.1f %.1f", inputctrl_global->scene_pos_x, inputctrl_global->scene_pos_y);
        ImGui::Text("scene w_h %.1f %.1f", inputctrl_global->scene_width, inputctrl_global->scene_height);

        static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
        static bool saved_palette_init = true;
        static ImVec4 saved_palette[32] = {};
        if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }
        static bool alpha_preview = true;
        static bool alpha_half_preview = false;
        static bool drag_and_drop = true;
        static bool options_menu = true;
        static bool hdr = false;
        ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);


        static ImVec4 backup_color;
        bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        open_popup |= ImGui::Button("Palette");
        if (open_popup)
        {
            ImGui::OpenPopup("mypicker");
            backup_color = color;
        }
        if (ImGui::BeginPopup("mypicker"))
        {
            ImGui::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
            ImGui::Separator();
            ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
            ImGui::SameLine();

            ImGui::BeginGroup(); // Lock X position
            ImGui::Text("Current");
            ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            ImGui::Text("Previous");
            if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            ImGui::Separator();
            ImGui::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::PushID(n);
                if ((n % 8) != 0)
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

                // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                        memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
            }
            ImGui::EndGroup();
            ImGui::EndPopup();


            inputctrl_global->color[0] = color.x;
            inputctrl_global->color[1] = color.y;
            inputctrl_global->color[2] = color.z;
            inputctrl_global->color[3] = color.w;


        }
        if (ImGui::Button("scene color change"))
            inputctrl_global->scene_color = inputctrl_global->color;
        ImGui::Checkbox("mesh_fill", &inputctrl_global->mesh_fill);
        if (inputctrl_global->check_vertex_num >= 0)
            ImGui::Text("%d select vertex", inputctrl_global->check_vertex_num);




        ImGui::End();
    }

protected:
    virtual void update() = 0;
private:


};

class Nodemode : public Mode
{
public:

    std::shared_ptr<Window> window;
    std::shared_ptr<Camera> camera;

    Node_manager node_manager = Node_manager();

    Nodemode(
        std::shared_ptr<Window> window,
        std::shared_ptr<Camera> camera
    ) : Mode(window, camera)
    {
        this->window = window;
        this->camera = camera;



    }
    ~Nodemode() {}

    void update()
    {
        this->node_manager.update(window->projection, camera->view, camera->position);
        imgui_base_render();
    }

    //// set all resources to use // object, UIobject, shader, animation
    void set_pre_object(const char* path, const char* name, bool gamma = false)
    {
        node_manager.set_pre_object(path, name, gamma);
    }
    void set_UIobject(const char* path, const char* name, bool gamma = false)
    {
        node_manager.set_pre_UIobject(path, name, gamma);
    }
    void set_shader(const char* vertexPath, const char* fragmentPath, std::string name, const char* geometryPath = nullptr)
    {
        node_manager.set_pre_shader(vertexPath, fragmentPath, name, geometryPath);
    }
    void set_animation(const char* path, const char* name, bool gamma = false)
    {
        node_manager.set_pre_animation(path, name, gamma);
    }



private:

    void imgui_render() {}

};

//// Render loop section
class Show_Display
{
public:
    std::shared_ptr<Window> window;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Nodemode> nodemode;


    Show_Display() :
        window(std::make_shared<Window>("node-based-modeling-opengl", 1920, 1080)),
        camera(std::make_shared<Camera>()),
        nodemode(std::make_shared<Nodemode>(window, camera))
    {}

    //// set all resources to use // object, UIobject, shader, animation
    void set_object(const char* path, const char* name, bool gamma = false)
    {
        nodemode->set_pre_object(path, name, gamma);
    }
    void set_UIobject(const char* path, const char* name, bool gamma = false)
    {
        nodemode->set_UIobject(path, name, gamma);
    }
    void set_shader(const char* vertexPath, const char* fragmentPath, std::string name, const char* geometryPath = nullptr)
    {
        nodemode->set_shader(vertexPath, fragmentPath, name, geometryPath);
    }
    void set_animation(const char* path, const char* name, bool gamma = false)
    {
        nodemode->set_animation(path, name, gamma);
    }



    void render_mode(
        Framebuffer& framebuffer)
    {
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        window->processInput();
        glfwMakeContextCurrent(window->window);
        imgui_newframe();
        ImGuizmo::BeginFrame();
        if (inputctrl_global->push_P != true)
            ImGui::Begin("My Scene", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        else
            ImGui::Begin("My Scene");



        //// Create a framebuffer to render the scene and attach it as a texture to an ImGui window.
        ImVec2 pos = ImGui::GetCursorScreenPos();
        const float window_width = ImGui::GetContentRegionAvail().x;
        const float window_height = ImGui::GetContentRegionAvail().y;
        glViewport(0, 0, window_width, window_height);
        framebuffer.bind_framebuffer(window_width, window_height);
        framebuffer.rescale_framebuffer(window_width, window_height);
        ImGui::GetWindowDrawList()->AddImage(
            (void*)framebuffer.texture_id,
            ImVec2(0, 0),
            ImVec2(window_width, window_height),
            ImVec2(0, 1),
            ImVec2(1, 0));

        ////guizmo setting
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);
        ImGui::End();

        inputctrl_global->scene_pos_x = pos.x;
        inputctrl_global->scene_pos_y = pos.y;
        inputctrl_global->window_width = window_width;
        inputctrl_global->window_height = window_height;
        inputctrl_global->scene_width = window_width;
        inputctrl_global->scene_height = window_height;

        glClearColor(inputctrl_global->scene_color[0], inputctrl_global->scene_color[1], inputctrl_global->scene_color[2], inputctrl_global->scene_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        switch (mode_list)
        {
        case NODE_MODE:
            mode_list = NODE_MODE;
            this->nodemode->update();
            break;
        }

        debug.debug_imgui_render(inputctrl_global->color);
        ImGui::Render();
    }

    //global func
    void imgui_set()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        (void)io;
        ImGui_ImplGlfw_InitForOpenGL(window->window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        ImGui::StyleColorsDark();
    }
    void camera_move()
    {
        camera->Camera_movement(inputctrl_global);
    }
    void imgui_newframe()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void shutdown_imgui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
private:

};

#endif
