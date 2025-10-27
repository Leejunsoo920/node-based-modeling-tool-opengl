#ifndef INPUTCTRL_H
#define INPUTCTRL_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <vector>



enum class Gizmo_state
{
    TRANSLATE,
    ROTATE
};



class Input_ctrl
{

public:


    int check_vertex_num = -1;
    float deltaTime;


    Gizmo_state now_gizmo_state = Gizmo_state::TRANSLATE;

    Input_ctrl(){}
    ~Input_ctrl() {}

    glm::mat4 model;
    glm::vec3 neck_pos = glm::vec3(0.0,0.6,0.0);

    int mouse_pos[2];
    glm::vec3 mouse_pos_wolrd_pos;
    glm::vec2 t;

    glm::mat4 local_angle_plus;
    
    int now_frame = 0;

    glm::vec3 near_point;
    glm::vec3 far_point;
    glm::vec3 mouse_point_direction;

    float lastX = 800.0f / 2.0;
    float lastY = 800.0 / 2.0;
    std::vector<glm::vec3> local_axis;



    float camera_scroll = 0;

    float scene_pos_x;
    float scene_pos_y;

    float window_width;
    float window_height;

    float real_y = 0;


    float scene_width;
    float scene_height;

    glm::vec4 color = glm::vec4(0.20,0.20,0.20,1.0);
    glm::vec4 scene_color = glm::vec4(1.00, 1.0, 1.0, 1.0);


    bool first_mouse = true;
    bool push_butten_right = false;
    bool mesh_fill = false;

    //push butten check mouse&keyborad

    bool depth_test = false;

    bool bone_draw = true;
    bool normal_draw = false;
    
    bool one_object_draw = false;

    bool only_frame_mode = true;

    bool push_butten = false;
    bool Reset_triger = false;
    bool push_shift = false;

    bool push_N = false;
    bool push_N_before = false;

    bool push_B = false;
    bool push_B_before = false;

    bool push_P = false;

    bool push_Z = false;
    bool push_Z_before = false;



    bool push_tab = false;
    bool push_tab_before = true;

    bool push_space = false;
    bool push_space_before = true;

    bool push_1 = false;
    bool push_2 = false;
    bool push_3 = false;
    bool push_4 = false;
    bool push_5 = false;

    bool push_F = false;

    int push_c = 0;
    int push_t = 0;
    int push_g = 0;

    bool push_T = false;

    bool push_X = false;
    bool push_C = false;

    bool push_O_before = false;
    bool push_O = false;

    bool Window_push_W = false;
    bool Window_push_A = false;
    bool Window_push_S = false;
    bool Window_push_D = false;

    float model_float = 0.5f;
    float global_twist = 0.0f;
    //bool Reset_triger = false;

    bool antialiasing = false;


};





#endif


