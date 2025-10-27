// This project uses parts of the "OpenCloth" codebase for simulation testing and learning purposes.
// The original work is licensed under the BSD License.
//
// Original Author comment
//A simple cloth using explicit Euler integration based on the SIGGRAPH course notes
//"Realtime Physics" http://www.matthiasmueller.info/realtimephysics/coursenotes.pdf.
//This code is intended for beginners  so that they may understand what is required 
//to implement a simple cloth simulation.
//
//This code is under BSD license. If you make some improvements,
//or are using this in your research, do let me know and I would appreciate
//if you acknowledge this in your code.
//
//Controls:
//left click on any empty region to rotate, middle click to zoom 
//left click and drag any point to drag it.
//
//Author: Movania Muhammad Mobeen
//        School of Computer Engineering,
//        Nanyang Technological University,
//        Singapore.
//Email : mova0002@e.ntu.edu.sg
//
//Changes: Added wind external force. Press 'w' to toggle wind on/off
//

#ifndef NODE_SIMULATION_H
#define NODE_SIMULATION_H
#include <windows.h>

#include <algorithm> 
#include <iostream>
#include "Node_template.h"
#include <ctime>

struct Simulation_property
{    
public:
    glm::vec3 gravity = glm::vec3(0.0f, -0.00981f, 0.0f);



};


struct Spring {
    int p1, p2;
    float rest_length;
    float Ks, Kd;
    int type;
};


class Node_func_ExplicitEuler_cloth_simulation : public Node_template_func
{
    float mass = 0.5f;
    float fullsize = 4.0f;
    float halfsize = fullsize / 2.0f;

    int oldX = 0, oldY = 0;

    float currentTime = 0;
    float timeStep = 1 / 60.0f;
    double accumulator = timeStep;

    float	KsStruct = 0.75f, KdStruct = -0.25f;
    float	KsShear = 0.75f, KdShear = -0.25f;
    float	KsBend = 0.95f, KdBend = -0.25f;
    glm::vec3 wind = glm::vec3(0, 0, 0);

    int spring_count = 0;

    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks

    int numX = 20, numY = 20;
    size_t total_points = (numX + 1) * (numY + 1);

    std::vector<glm::vec3> X;
    std::vector<glm::vec3> V;
    std::vector<glm::vec3> F;

    std::vector<glm::vec3> sumF; //for RK4
    std::vector<glm::vec3> sumV;

    std::vector<Spring> springs;

    const int STRUCTURAL_SPRING = 0;
    const int SHEAR_SPRING = 1;
    const int BEND_SPRING = 2;
    float fRadius = 1;
    glm::mat4 ellipsoid, inverse_ellipsoid;

    const float DEFAULT_DAMPING = -0.0125f;
    glm::vec3 center = glm::vec3(0, 0, 0);
    float radius = 1;
    float frameTime = 0;
    double frameTimeQP = 0;
public:
    Node_func_ExplicitEuler_cloth_simulation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {



        // get ticks per second
        QueryPerformanceFrequency(&frequency);

        // start timer
        QueryPerformanceCounter(&t1);




        //indices.resize(numX * numY * 2 * 3);



        X.resize(total_points);
        V.resize(total_points);
        F.resize(total_points);


        //for RK4
        sumF.resize(total_points);
        sumV.resize(total_points);


        int i = 0, j = 0, count = 0;
        int l1 = 0, l2 = 0;
        float ypos = 7.0f;
        int v = numY + 1;
        int u = numX + 1;

        //fill in X
        for (j = 0; j < v; j++) {
            for (i = 0; i < u; i++) {
                X[count++] = glm::vec3(((float(i) / (u - 1)) * 2 - 1) * halfsize, fullsize + 1, ((float(j) / (v - 1)) * fullsize));
            }
        }

        //fill in V
        memset(&(V[0].x), 0, total_points * sizeof(glm::vec3));

        /*
        //fill in indices
        GLushort* id = &indices[0];
        for (i = 0; i < numY; i++) {
            for (j = 0; j < numX; j++) {
                int i0 = i * (numX + 1) + j;
                int i1 = i0 + 1;
                int i2 = i0 + (numX + 1);
                int i3 = i2 + 1;
                if ((j + i) % 2) {
                    *id++ = i0; *id++ = i2; *id++ = i1;
                    *id++ = i1; *id++ = i2; *id++ = i3;
                }
                else {
                    *id++ = i0; *id++ = i2; *id++ = i3;
                    *id++ = i0; *id++ = i3; *id++ = i1;
                }
            }
        }
        */



        //setup springs
        // Horizontal
        for (l1 = 0; l1 < v; l1++)	// v
            for (l2 = 0; l2 < (u - 1); l2++) {
                AddSpring((l1 * u) + l2, (l1 * u) + l2 + 1, KsStruct, KdStruct, STRUCTURAL_SPRING);
            }

        // Vertical
        for (l1 = 0; l1 < (u); l1++)
            for (l2 = 0; l2 < (v - 1); l2++) {
                AddSpring((l2 * u) + l1, ((l2 + 1) * u) + l1, KsStruct, KdStruct, STRUCTURAL_SPRING);
            }


        // Shearing Springs
        for (l1 = 0; l1 < (v - 1); l1++)
            for (l2 = 0; l2 < (u - 1); l2++) {
                AddSpring((l1 * u) + l2, ((l1 + 1) * u) + l2 + 1, KsShear, KdShear, SHEAR_SPRING);
                AddSpring(((l1 + 1) * u) + l2, (l1 * u) + l2 + 1, KsShear, KdShear, SHEAR_SPRING);
            }


        // Bend Springs
        for (l1 = 0; l1 < (v); l1++) {
            for (l2 = 0; l2 < (u - 2); l2++) {
                AddSpring((l1 * u) + l2, (l1 * u) + l2 + 2, KsBend, KdBend, BEND_SPRING);
            }
            AddSpring((l1 * u) + (u - 3), (l1 * u) + (u - 1), KsBend, KdBend, BEND_SPRING);
        }
        for (l1 = 0; l1 < (u); l1++) {
            for (l2 = 0; l2 < (v - 2); l2++) {
                AddSpring((l2 * u) + l1, ((l2 + 2) * u) + l1, KsBend, KdBend, BEND_SPRING);
            }
            AddSpring(((v - 3) * u) + l1, ((v - 1) * u) + l1, KsBend, KdBend, BEND_SPRING);
        }

        ellipsoid = glm::translate(glm::mat4(1), glm::vec3(0, 2, 0));
        ellipsoid = glm::rotate(ellipsoid, 45.0f, glm::vec3(1, 0, 0));
        ellipsoid = glm::scale(ellipsoid, glm::vec3(fRadius, fRadius, fRadius / 2));
        inverse_ellipsoid = glm::inverse(ellipsoid);



    }
    void AddSpring(int a, int b, float ks, float kd, int type) 
    {
        Spring spring;
        spring.p1 = a;
        spring.p2 = b;
        spring.Ks = ks;
        spring.Kd = kd;
        spring.type = type;
        glm::vec3 deltaP = X[a] - X[b];
        spring.rest_length = sqrt(glm::dot(deltaP, deltaP));
        springs.push_back(spring);
    }
    void evaluate() override
    {

        size_t i = 0;
        float newTime = glfwGetTime();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        //accumulator += frameTime;

        //Using high res. counter
        QueryPerformanceCounter(&t2);
        // compute and print the elapsed time in millisec
        frameTimeQP = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
        t1 = t2;
        accumulator += frameTimeQP;


        //Fixed time stepping + rendering at different fps	
        if (accumulator >= timeStep)
        {
            StepPhysics(timeStep);
            accumulator -= timeStep;
        }



        evaluated = true;


    }

    void StepPhysics(float dt) {
        srand((unsigned int)time(NULL));
        float rx = (float(rand()) / RAND_MAX) * 2 - 1.0f;
        float ry = (float(rand()) / RAND_MAX) * 2 - 1.0f;
        float rz = (float(rand()) / RAND_MAX) * 2 - 1.0f;
        wind += glm::vec3(rx, ry, rz) * 0.00001f;

        ComputeForces();

        //for Explicit/Midpoint Euler 
        IntegrateEuler(dt);

        //for mid-point Euler
        //IntegrateMidpointEuler(timeStep);

        //for RK4
        //IntegrateRK4(timeStep);
        EllipsoidCollision();
        ApplyProvotDynamicInverse();
    }

    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (!X.empty())
        {
            for (auto element : X)
            {
                debug.set_point_group("x test", element);
            }

        }
        return false;

    }


private:

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {

            base_imgui();

        }
        ImGui::EndChild();


    }

    void ComputeForces
    (
    ) 
    {
        Simulation_property simulation_property;


        size_t i = 0;
        for (i = 0; i < total_points; i++) {
            F[i] = glm::vec3(0);

            //add gravity force only for non-fixed points
            if (i != 0 && i != numX)
                F[i] += simulation_property.gravity;

            //add force due to damping of velocity
            F[i] += DEFAULT_DAMPING * V[i];
        }

        //add spring forces
        for (i = 0; i < springs.size(); i++) {
            glm::vec3 p1 = X[springs[i].p1];
            glm::vec3 p2 = X[springs[i].p2];
            glm::vec3 v1 = V[springs[i].p1];
            glm::vec3 v2 = V[springs[i].p2];
            glm::vec3 deltaP = p1 - p2;
            glm::vec3 deltaV = v1 - v2;
            float dist = glm::length(deltaP);

            float leftTerm = -springs[i].Ks * (dist - springs[i].rest_length);
            float rightTerm = springs[i].Kd * (glm::dot(deltaV, deltaP) / dist);
            glm::vec3 springForce = (leftTerm + rightTerm) * glm::normalize(deltaP);

            if (springs[i].p1 != 0 && springs[i].p1 != numX)
                F[springs[i].p1] += springForce;
            if (springs[i].p2 != 0 && springs[i].p2 != numX)
                F[springs[i].p2] -= springForce;
        }
    }

    void IntegrateEuler
    (
        float deltaTime
    ) 
    {
        float deltaTimeMass = deltaTime / mass;
        size_t i = 0;

        for (i = 0; i < total_points; i++) {
            glm::vec3 oldV = V[i];
            V[i] += (F[i] * deltaTimeMass);
            X[i] += deltaTime * oldV;

            if (X[i].y < 0) {
                X[i].y = 0;
            }
        }
    }

    void EllipsoidCollision
    (
    )
    {
        for (size_t i = 0; i < total_points; i++) {
            glm::vec4 X_0 = (inverse_ellipsoid * glm::vec4(X[i], 1));
            glm::vec3 delta0 = glm::vec3(X_0.x, X_0.y, X_0.z) - center;
            float distance = glm::length(delta0);
            if (distance < 1.0f) {
                delta0 = (radius - distance) * delta0 / distance;

                // Transform the delta back to original space
                glm::vec3 delta;
                glm::vec3 transformInv;
                transformInv = glm::vec3(ellipsoid[0].x, ellipsoid[1].x, ellipsoid[2].x);
                transformInv /= glm::dot(transformInv, transformInv);
                delta.x = glm::dot(delta0, transformInv);
                transformInv = glm::vec3(ellipsoid[0].y, ellipsoid[1].y, ellipsoid[2].y);
                transformInv /= glm::dot(transformInv, transformInv);
                delta.y = glm::dot(delta0, transformInv);
                transformInv = glm::vec3(ellipsoid[0].z, ellipsoid[1].z, ellipsoid[2].z);
                transformInv /= glm::dot(transformInv, transformInv);
                delta.z = glm::dot(delta0, transformInv);
                X[i] += delta;
                V[i] = glm::vec3(0);
            }
        }
    }


    void ApplyProvotDynamicInverse
    (
    )
    {

        for (size_t i = 0; i < springs.size(); i++) {
            //check the current lengths of all springs
            glm::vec3 p1 = X[springs[i].p1];
            glm::vec3 p2 = X[springs[i].p2];
            glm::vec3 deltaP = p1 - p2;
            float dist = glm::length(deltaP);
            if (dist > springs[i].rest_length) {
                dist -= (springs[i].rest_length);
                dist /= 2.0f;
                deltaP = glm::normalize(deltaP);
                deltaP *= dist;
                if (springs[i].p1 == 0 || springs[i].p1 == numX) {
                    V[springs[i].p2] += deltaP;
                }
                else if (springs[i].p2 == 0 || springs[i].p2 == numX) {
                    V[springs[i].p1] -= deltaP;
                }
                else {
                    V[springs[i].p1] -= deltaP;
                    V[springs[i].p2] += deltaP;
                }
            }
        }
    }


};





















#endif // ! NODE_SIMULATION_H

