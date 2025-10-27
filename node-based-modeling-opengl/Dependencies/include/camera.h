#ifndef CAMERA_H
#define CAMERA_H


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "object.h"
#include "inputctrl.h"

enum Camera_direction
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT

};

class Camera  : public Object
{
public:

    glm::mat4 view;

    float camera_z;
    glm::vec3 camera_Front;
    glm::vec3 camera_Up;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;

    float camera_Speed = 3;
    
    float lastX;
    float lastY;



    Camera(glm::vec3 position = glm::vec3(0.0,0.0,2.414213562f), float z_now = 2.414213562f) :
        Object(position, velocity, force)
    {
        this->camera_z = z_now;
        this->camera_Front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->camera_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = -90.0f;
        this->pitch = 0.0f;

    }

    /// when push button in base imgui camera position reset to initail position and direnction
    void camera_reset()
    {
        position = glm::vec3(0.0, 0.0, 2.414213562f);
        camera_Front = glm::vec3(0.0f, 0.0f, -1.0f);
        camera_Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = -90.0f;
        this->pitch = 0.0f;
    }

    void mouse_callback_cam(std::unique_ptr<Input_ctrl>& inputctrl_global)
    {
        float xpos = static_cast<float>(inputctrl_global->mouse_pos[0]);
        float ypos = static_cast<float>(inputctrl_global->mouse_pos[1]);
        float xoffset = xpos - inputctrl_global->lastX;
        float yoffset = inputctrl_global->lastY - ypos; // reversed since y-coordinates go from bottom to top
        inputctrl_global->lastX = xpos;
        inputctrl_global->lastY = ypos;

        if (inputctrl_global->push_butten_right)
        {
            if (inputctrl_global->first_mouse)
            {
                camera_reset();
                inputctrl_global->first_mouse = false;
            }

            float sensitivity = 0.1f; // change this value to your liking
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            camera_Front = glm::normalize(front);
        } 
    }

    void Camera_movement(std::unique_ptr<Input_ctrl>& inputctrl_global)
    {
        mouse_callback_cam(inputctrl_global);

        if (inputctrl_global->Window_push_W)
            position += camera_Front * camera_Speed * inputctrl_global->deltaTime ;
        if (inputctrl_global->Window_push_S)
            position -= camera_Front * camera_Speed * inputctrl_global->deltaTime ;
        if (inputctrl_global->Window_push_A)
            position -= glm::normalize(glm::cross(camera_Front, camera_Up)) * camera_Speed * inputctrl_global->deltaTime;
        if (inputctrl_global->Window_push_D)
            position += glm::normalize(glm::cross(camera_Front, camera_Up)) * camera_Speed * inputctrl_global->deltaTime;


        if (inputctrl_global->mouse_pos[0] > inputctrl_global->scene_pos_x && inputctrl_global->mouse_pos[1] > inputctrl_global->scene_pos_y)
        {
            if (inputctrl_global->mouse_pos[0] < inputctrl_global->scene_pos_x + inputctrl_global->window_width && inputctrl_global->mouse_pos[1] < inputctrl_global->scene_pos_y + inputctrl_global->window_height)
            {

                position += inputctrl_global->camera_scroll * camera_Front;
                inputctrl_global->camera_scroll = 0;
            }
        }


        camera_z = position[2];
        this->view = glm::lookAt(position, position + camera_Front, camera_Up);

        //this->view = glm::lookAt(position, glm::vec3(0.0, 0.0f, 0.0f), camera_Up);







    }

    void set_view()
    {

    }















};












#endif