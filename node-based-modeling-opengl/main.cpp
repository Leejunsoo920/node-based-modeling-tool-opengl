#include <glad\glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "scenectrl.h"

Scenectrl scenectrl = Scenectrl();

int main()
{
    glfwSwapInterval(1);

    //// shader
    scenectrl.set_shader("shader/light.vs", "shader/transparent.fs", "transparent shader");
    scenectrl.set_shader("shader/UIshader.vs", "shader/shapematchingshader.fs", "color shader");
    scenectrl.set_shader("shader/instancing.vs", "shader/UIshader.fs", "instancingshader");
    scenectrl.set_shader("shader/texture.vs", "shader/texture.fs", "texture shader");
    scenectrl.set_shader("shader/instancing_texture.vs", "shader/instancing_texture.fs", "instancing_texture shader");
    scenectrl.set_shader("shader/skin_animation.vs", "shader/skin_animation.fs", "skin");
    scenectrl.set_shader("shader/texture.vs", "shader/texture.fs", "texture shader");
    scenectrl.set_shader("shader/light.vs", "shader/light.fs", "nomal shader");

    //// ui object load  
    scenectrl.set_UIobject("resources/objects/ui_sphere.obj", "UI_sphere");
    scenectrl.set_UIobject("resources/objects/bone.obj", "bone");
    scenectrl.set_UIobject("resources/objects/cone.obj", "cone");
    scenectrl.set_UIobject("resources/objects/yajirusi.obj", "yajirusi");
    scenectrl.set_UIobject("resources/objects/gizumo.obj", "gizumo");
    scenectrl.set_UIobject("resources/objects/axis.obj", "axis");
    scenectrl.set_UIobject("resources/objects/line.obj", "line");

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(1.0f, 0.0f, 1.0f, 1.00f);
    IMGUI_CHECKVERSION();
    ImGuiContext* main_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(scenectrl.operation.show_display.window->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    Framebuffer framebuffer = Framebuffer(60, 775);


    // set debugUI buffer that using instance -> make buffer and save them in their class
    debug.set_debugUI_buffer();


    std::vector<double> start = { 0, 0 };
    std::vector<double> end = { 10, 0 };
    int t_steps = 5;
    double dt = 0.1;
    int force_duration = 5;

    while (!glfwWindowShouldClose(scenectrl.operation.show_display.window->window))
    {
        scenectrl.render(main_context, framebuffer);
    }

    scenectrl.shutdown_imgui();
    glfwTerminate();
    return 0;
}


