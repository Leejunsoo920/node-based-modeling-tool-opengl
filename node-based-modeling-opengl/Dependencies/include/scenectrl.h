#ifndef SCENECTRL_H
#define SCENECTRL_H

#include "opt.h"

class Scenectrl
{
public:
    Scenectrl(){}
    Operation operation;

    void set_object(const char* path, const char* name, bool gamma = false){operation.set_object(path,name, gamma);}
    void set_UIobject(const char* path, const char* name, bool gamma = false){operation.set_UIobject(path,name, gamma);}
    void set_shader(const char* vertexPath, const char* fragmentPath, std::string name, const char* geometryPath = nullptr){operation.set_shader(vertexPath, fragmentPath,name,geometryPath);}
    void set_animation(const char* path, const char* name, bool gamma = false){operation.set_animation(path, name, gamma);}
    void shutdown_imgui(){operation.shutdown_imgui();}


private:
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

public:
    void imgui_set(){operation.imgui_set();}
 
    ////render loop func
    //// set scene and make frame buffer finaliy change framebuffer to texture and Attach a texture to an ImGui window
	void render(
        ImGuiContext* main_context,
        Framebuffer& framebuffer)
	{
        //// time check
        float currentFrame = static_cast<float>(glfwGetTime());
        inputctrl_global->deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //// set Imgui Context
        ImGui::SetCurrentContext(main_context);
        glfwPollEvents();
        operation.render_setting(framebuffer);
        operation.render(framebuffer);
        operation.camera_move();
        framebuffer.unbind_framebuffer();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(operation.show_display.window->window);

	}



};






#endif