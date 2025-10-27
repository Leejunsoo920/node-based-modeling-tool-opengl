#ifndef OPT_H
#define OPT_H

#include <algorithm>
#include <vector>
#include <cmath>
#include "show_display.h"


class Operation
{
public:
    Show_Display show_display;
    Operation() {}
    ~Operation() {}

    //when start, setting option
    void render_setting(Framebuffer& framebuffer){}

    //camera option 
    void camera_move() { show_display.camera_move(); }

    //setting of ingui
    void imgui_set() { show_display.imgui_set(); }
    void shutdown_imgui() { show_display.shutdown_imgui(); }

    // render 
    void render(Framebuffer& framebuffer) { show_display.render_mode(framebuffer); }

    //set model
    void set_object(const char* path, const char* name, bool gamma = false) { show_display.set_object(path, name, gamma); }
    void set_UIobject(const char* path, const char* name, bool gamma = false) { show_display.set_UIobject(path, name, gamma); }
    void set_shader(const char* vertexPath, const char* fragmentPath, std::string name, const char* geometryPath = nullptr) { show_display.set_shader(vertexPath, fragmentPath, name, geometryPath); }
    void set_animation(const char* path, const char* name, bool gamma) { show_display.set_animation(path, name, gamma); }

private:
};

#endif