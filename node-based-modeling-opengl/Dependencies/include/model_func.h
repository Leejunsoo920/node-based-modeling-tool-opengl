#ifndef OPTFUNC_H
#define OPTFUNC_H

#include <vector>


class Drag
{
public:

    static std::vector<glm::vec3> drag_point;
    static glm::vec2 first_push_pixel_point;


    Drag()
    {

    }

    void set_drag(const glm::mat4& projection, const glm::mat4& view,
         std::vector<Vertex> vertices, std::vector<int> in_sq)
    {
        set_drag_point(projection, view);
        set_drag_select_vertex(projection, view, vertices,in_sq);

    }


    void set_drag_point(const glm::mat4& projection,const glm::mat4& view)
    {
        std::vector<float> temp;

        glm::vec3 temp_vec3;

        debug.set_text("near_point", inputctrl_global->near_point);
        debug.set_text("far_point", inputctrl_global->far_point);
        

        if (drag_point.size() == 0)
        {
            temp_vec3 = inputctrl_global->near_point;

            drag_point.push_back(temp_vec3);
            drag_point.push_back(temp_vec3); //// input all position, because drag must have four point
            drag_point.push_back(temp_vec3);
            drag_point.push_back(temp_vec3); //// when drag_point.size == 4 -> drag draw and check vertices position that in drag square
        
            first_push_pixel_point = glm::vec2(inputctrl_global->mouse_pos[0], inputctrl_global->mouse_pos[1]);
        }

        if (drag_point.size() > 0)
        {
            Set_ray set_ray;
            temp_vec3 = set_ray.calculate_ray_for_nearpoint_by_pixel(
                    projection,
                    view,
                    inputctrl_global->mouse_pos[0],
                    first_push_pixel_point[1],
                    inputctrl_global->scene_width,
                    inputctrl_global->scene_height

                );

            drag_point[1] = temp_vec3;
            
            temp_vec3 = set_ray.calculate_ray_for_nearpoint_by_pixel(
                projection,
                view,
                first_push_pixel_point[0],
                inputctrl_global->mouse_pos[1],
                inputctrl_global->scene_width,
                inputctrl_global->scene_height

            );
            
            drag_point[2] = temp_vec3;


            drag_point[3] = inputctrl_global->near_point;










        }

        




    }
    

    
    void set_drag_select_vertex(

        const glm::mat4& projection,
        const glm::mat4& view,
        std::vector<Vertex> vertices,
        std::vector<int> &in_sq)
    {
        float x_min;
        float x_max;
        float y_min;
        float y_max;


        if (drag_point[0][0] < drag_point[3][0])
        {
            if (drag_point[0][1] > drag_point[3][1])
            {
                x_min = first_push_pixel_point[0];
                x_max = inputctrl_global->mouse_pos[0];

                y_min = first_push_pixel_point[1];
                y_max = inputctrl_global->mouse_pos[1];

            }
            else
            {
                x_min = first_push_pixel_point[0];
                x_max = inputctrl_global->mouse_pos[0];


                y_min = inputctrl_global->mouse_pos[1];
                y_max = first_push_pixel_point[1];

            }

        }
        else
        {
            if (drag_point[0][1] > drag_point[3][1])
            {
                x_min = inputctrl_global->mouse_pos[0];
                x_max = first_push_pixel_point[0];

                y_min = first_push_pixel_point[1];
                y_max = inputctrl_global->mouse_pos[1];
            }
            else
            {
                x_min = inputctrl_global->mouse_pos[0];
                x_max = first_push_pixel_point[0];

                y_min = inputctrl_global->mouse_pos[1];
                y_max = first_push_pixel_point[1];



            }
        }

        for (unsigned int i = 0; i < vertices.size(); ++i)
        {

            glm::ivec4 viewport = glm::ivec4(0,0, inputctrl_global->scene_width, inputctrl_global->scene_height);
            glm::vec3 screen_pos = glm::project(vertices[i].Position, view, projection, viewport);
            screen_pos[1] = viewport[3] - screen_pos[1];

            if (screen_pos[0] > x_min &&
                screen_pos[1] > y_min &&
                screen_pos[0] < x_max &&
                screen_pos[1] < y_max)
            {
                if (std::find(in_sq.begin(), in_sq.end(), i) == in_sq.end())
                {
                    in_sq.push_back(i);
                }
            }

        }

    }

    void clear_drag()
    {
        drag_point.clear();
    }
    


};

class Vertex_Picking
{
public:

    static int picking_vertex_num;

    float mim_distance = 0.2f;

    void picking()
    {



    }
    glm::vec2 check_by_ray(
        const glm::vec3& target,
        const float& min_distance)
    {
        
        //// A_minus C does not have a physical meaning
        ////however, since the final equation takes the form a = dir^2, b = 2 * dir * A_minus_C, and c = A_minus_C^2 - r^2 by sphere equation
        //// so we define this variable for convenience
        glm::vec3 A_minus_C = inputctrl_global->near_point - target;


        glm::vec3 dir = inputctrl_global->mouse_point_direction;

        float a = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        float b = 2 * (A_minus_C[0] * dir[0] + A_minus_C[1] * dir[1] + A_minus_C[2] * dir[2]);
        float c = A_minus_C[0] * A_minus_C[0] + A_minus_C[1] * A_minus_C[1] + A_minus_C[2] * A_minus_C[2] - min_distance * min_distance;

        float t1 = -b + sqrt(b * b - 4 * a * c) / (2 * a);
        float t2 = -b - sqrt(b * b - 4 * a * c) / (2 * a);

        return glm::vec2(t1 ,t2);
    }
    void hold_pick(
        std::vector<Vertex> vertices
    )
    {
        check_by_ray(vertices[picking_vertex_num].Position, mim_distance);
        glm::vec3 dir = inputctrl_global->mouse_point_direction;
        dir[0] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        dir[1] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        dir[2] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        glm::vec3 cen = inputctrl_global->near_point + dir;

        inputctrl_global->mouse_pos_wolrd_pos = cen;




    }
    void hold_pick(
        const glm::vec3& position,
        const float& min_distance)
    {
        Vertex_Picking object_picking;
        object_picking.check_by_ray(position, min_distance);
        glm::vec3 dir = inputctrl_global->mouse_point_direction;
        dir[0] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        dir[1] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        dir[2] *= (inputctrl_global->t[0] + 0.05) / 2 + 0.05;
        glm::vec3 cen = inputctrl_global->near_point + dir;

        inputctrl_global->mouse_pos_wolrd_pos = cen;

    }

    void check_piking_distance(int mouse_pos[2], std::vector<glm::vec3> vertices)
    {


        glm::vec2 t;
        float dis_t;
        float min_t = 0;



        int mim_distance_num = -1;


        for (unsigned int i = 0; i < vertices.size(); i++)
        {

            t = check_by_ray(vertices[i], mim_distance);
            //dis_t = check_by_ray(glm::vec3(0.0), mim_distance);


            dis_t = t[0] - t[1];
            if (dis_t > 0 && dis_t > min_t)
            {

                mim_distance_num = i;
                min_t = dis_t;

                inputctrl_global->t = t;
            }




        }
        picking_vertex_num = mim_distance_num;


        if(mim_distance_num >= 0)
            debug.set_point_group("predict pick vertex", vertices[mim_distance_num], glm::vec3(1,0.45,0.1));





    }
    
    void set_pick_select_vertex(
        std::vector<Vertex> vertices,
        std::vector<int>& in_select_vertex)
    {
        if (std::find(in_select_vertex.begin(), in_select_vertex.end(), picking_vertex_num) == in_select_vertex.end())
        {
            in_select_vertex.push_back(picking_vertex_num);
        }

    }



};

class Ray_picking
{
public:




};



#endif