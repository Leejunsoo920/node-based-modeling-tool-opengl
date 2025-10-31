// This code is adapted from LearnOpenGL by Joey de Vries
// https://learnopengl.com
// Licensed under CC BY 4.0: https://creativecommons.org/licenses/by/4.0/
// Twitter: https://twitter.com/JoeyDeVriez

#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "inputctrl.h"

std::unique_ptr<Input_ctrl> inputctrl_global = std::make_unique<Input_ctrl>();


enum Mode_list
{
    NODE_MODE


};

static Mode_list mode_list = NODE_MODE;





// make Scene Frame buffer and change to texture
class Framebuffer
{
public:

    unsigned int FBO;
    unsigned int texture_id;
    unsigned int RBO;

    Framebuffer(float WIDTH, float HEIGHT)
    {
        
        if (inputctrl_global->antialiasing != true)
        {
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        else
        {
            glGenFramebuffers(1, &FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_id, 0);

            glGenRenderbuffers(1, &RBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }


    }
    ~Framebuffer()
    {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture_id);
        glDeleteRenderbuffers(1, &RBO);
    }
    void bind_framebuffer(float width, float height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        if (inputctrl_global->antialiasing != true)
        {

        }
        else
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
    void unbind_framebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void rescale_framebuffer(float width, float height)
    {
        if (inputctrl_global->antialiasing != true)
        {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        }
        else
        {


            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
            //glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_id, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);



        }


    }


private:


};

class Window
{

public:
    // settings
    int SCR_WIDTH;
    int SCR_HEIGHT;



    glm::mat4 projection;

    //now your mouse position

    


    // window 
    GLFWwindow* window;




    Window(const char* windowname,int width, int height)
    {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;




        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_SAMPLES, 4);


#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowname, NULL, NULL);
        // glfw window creation
        // --------------------

        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            //return -1;
        }
        //GLFWcharmodsfun *test = operation1->mouse_callback;

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);

        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


        // glad 초기화
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
           // return -1;
        }

    }


    void processInput()
    {
        //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         //   glfwSetWindowShouldClose(window, true);

        float cameraSpeed = static_cast<float>(4 * 0.0);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            inputctrl_global->Window_push_W = 1;
        else
            inputctrl_global->Window_push_W = 0;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            inputctrl_global->Window_push_S = 1;
        else
            inputctrl_global->Window_push_S = 0;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            inputctrl_global->Window_push_A = 1;
        else
            inputctrl_global->Window_push_A = 0;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            inputctrl_global->Window_push_D = 1;
        else
            inputctrl_global->Window_push_D = 0;


        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            inputctrl_global->push_F = true;

        }
        else
        {
            inputctrl_global->push_F = false;
        }


        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            inputctrl_global->now_gizmo_state = Gizmo_state::ROTATE;

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            inputctrl_global->push_c += 1;

        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            inputctrl_global->now_gizmo_state = Gizmo_state::TRANSLATE;

        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            inputctrl_global->push_g = 1;

        }
        else
        {
            inputctrl_global->push_g = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {

            inputctrl_global->push_Z = true;
        }
        else
        {
            inputctrl_global->push_Z = false;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            inputctrl_global->push_shift = true;
        }
        else
        {
            inputctrl_global->push_shift = false;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            inputctrl_global->push_X = true;
        }
        else
        {
            inputctrl_global->push_X = false;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            inputctrl_global->push_C = true;
        }
        else
        {
            inputctrl_global->push_C = false;
        }
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            if (inputctrl_global->push_N == false && inputctrl_global->push_N_before == false)
            {
                inputctrl_global->push_N = true;

            }
            else
            {
                inputctrl_global->push_N = false;
            }
            inputctrl_global->push_N_before = true;


        }
        else
        {
            inputctrl_global->push_N = false;
            inputctrl_global->push_N_before = false;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            /*
            if (inputctrl_global->push_space_before == false)
            {
                if (mode_list == OBJECTMODE)
                {
                    mode_list = PLAYMODE;
                    inputctrl_global->push_space_before = true;
                }
                else
                {
                    mode_list = OBJECTMODE;
                    inputctrl_global->push_space_before = true;

                }
            }
            */
        }
        else
            inputctrl_global->push_space_before = false;


        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
        {
                /*
            if (inputctrl_global->push_tab_before == false)
            {

                if (mode_list != EDITMODE)
                {
                    mode_list = EDITMODE;

                }
                else
                {
                    mode_list = OBJECTMODE;


                }

            }
            inputctrl_global->push_tab_before = true;
            */
        }
        else
            inputctrl_global->push_tab_before = false;

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            /*
            if (inputctrl_global->push_O_before == false)
            {
                if (mode_list != NODE_MODE)
                {
                    mode_list = NODE_MODE;

                }
                else
                {
                    mode_list = OBJECTMODE;


                }

            }
            inputctrl_global->push_O_before = true;
            */

        }
        else
            inputctrl_global->push_O_before = false;


        push_ctrl(GLFW_KEY_B, inputctrl_global->push_B);
        push_ctrl(GLFW_KEY_P, inputctrl_global->push_P);

        push_ctrl(GLFW_KEY_1, inputctrl_global->push_1);
        push_ctrl(GLFW_KEY_2, inputctrl_global->push_2);
        push_ctrl(GLFW_KEY_3, inputctrl_global->push_3);
        push_ctrl(GLFW_KEY_4, inputctrl_global->push_4);
        push_ctrl(GLFW_KEY_5, inputctrl_global->push_5);
    }

private:

    void push_ctrl(int n, bool& push)
    {
        if (glfwGetKey(window, n) == GLFW_PRESS)
        {
            push = 1;

        }
        else
        {
            push = 0;
        }
    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {

        glViewport(0, 0, width, height);

    }



    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {

        inputctrl_global->camera_scroll = (float)yoffset / 3;

    }


    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        if (inputctrl_global->first_mouse)
        {
            inputctrl_global->lastX = xposIn;
            inputctrl_global->lastX = yposIn;
            inputctrl_global->first_mouse = false;
        }
        
        inputctrl_global->mouse_pos[0] = static_cast<int>(xposIn);// -inputctrl_global->scene_pos_x;
        inputctrl_global->mouse_pos[1] = static_cast<int>(yposIn);// -inputctrl_global->scene_pos_y;


        

    }


    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {

            inputctrl_global->push_butten = true;



        }
        else
        {
            inputctrl_global->push_butten = false;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {

            inputctrl_global->push_butten_right = true;



        }
        else
        {
            inputctrl_global->push_butten_right = false;
        }
        


    }




};





#endif
