// maymath

#ifndef MYMATH_H
#define MYMATH_H

#include "glm/glm.hpp"
#include "Eigen/dense"
#include "Eigen/core"
#include "nlopt.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>


class Gizmo
{
public:

    static glm::mat4 gizmo_matrix;
    static std::vector<glm::mat4> origin_matrix;


    Gizmo() {}
    ~Gizmo() {}


};



class TrajectoryOptimizer 
{

private:
    struct OptimizationData 
    {
        int t_steps;
        double dt;
        std::vector<double> a;
        std::vector<double> b;
        int force_duration;
    } opt_data;



    static double trajectory_energy(
        const std::vector<double>& variables,
        std::vector<double>& grad,
        void* data) 
{
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        int t_steps = opt_data->t_steps;
        double dt = opt_data->dt;


        //set variables from parameter vector variables
        std::vector<double> x(variables.begin(), variables.begin() + t_steps);
        std::vector<double> y(variables.begin() + t_steps, variables.begin() + 2 * t_steps);
        std::vector<double> fx(variables.begin() + 2 * t_steps, variables.begin() + 3 * t_steps);
        std::vector<double> fy(variables.begin() + 3 * t_steps, variables.end());

        double total_energy = 0.0;
        
        for (int i = 0; i < t_steps; i++) 
        {
            // estimate velocity by x, y position, if i == 0->x,y[0], or not calculate [i] - [i-1]
            double vx = (i == 0 ? x[i] : (x[i] - x[i - 1]) / dt);
            double vy = (i == 0 ? y[i] : (y[i] - y[i - 1]) / dt);

            //kinetic energy 1/2 * v^2
            double kinetic_energy = 0.5 * (vx * vx + vy * vy);

            //potential energy by gravity
            double potential_energy = g * y[i];


            double force_energy = 0.5 * (fx[i] * fx[i] + fy[i] * fy[i]);

            total_energy += (kinetic_energy + potential_energy + force_energy) * dt;
        }

        return total_energy;
    }
    static double constraint_acceleration_x(
        const std::vector<double>& variables,
        std::vector<double>& grad,
        void* data
    ) 
    {
        auto* opt_data = static_cast<std::pair<OptimizationData*, int>*>(data);
        OptimizationData* main_data = opt_data->first;
        int i = opt_data->second;

        int t_steps = main_data->t_steps;
        double dt = main_data->dt;

        const std::vector<double> x(variables.begin(), variables.begin() + t_steps);
        const std::vector<double> fx(variables.begin() + 2 * t_steps, variables.begin() + 3 * t_steps);

        double ax = fx[i];
        double vx = (i == 0 ? x[i] : (x[i] - x[i - 1]) / dt);

        return (i == 0 ? vx : (vx - (x[i] - x[i - 1]) / dt)) - ax * dt;
    }


    static double constraint_initial_x(
        const std::vector<double>& variables,
        std::vector<double>& grad,
        void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> x(variables.begin(), variables.begin() + opt_data->t_steps);
        return x[0] - opt_data->a[0];
    }

    static double constraint_initial_y(const std::vector<double>& variables, std::vector<double>& grad, void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> y(variables.begin() + opt_data->t_steps, variables.begin() + 2 * opt_data->t_steps);
        return y[0] - opt_data->a[1];
    }

    static double constraint_final_x(const std::vector<double>& variables, std::vector<double>& grad, void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> x(variables.begin(), variables.begin() + opt_data->t_steps);
        return x[opt_data->t_steps - 1] - opt_data->b[0];
    }

    static double constraint_final_y(const std::vector<double>& variables, std::vector<double>& grad, void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> y(variables.begin() + opt_data->t_steps, variables.begin() + 2 * opt_data->t_steps);
        return y[opt_data->t_steps - 1] - opt_data->b[1];
    }

    static double constraint_force_x(unsigned i, const std::vector<double>& variables, std::vector<double>& grad, void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> fx(variables.begin() + 2 * opt_data->t_steps, variables.begin() + 3 * opt_data->t_steps);
        return (i < opt_data->force_duration) ? 0.0 : fx[i];
    }

    static double constraint_force_y(unsigned i, const std::vector<double>& variables, std::vector<double>& grad, void* data) {
        OptimizationData* opt_data = static_cast<OptimizationData*>(data);
        const std::vector<double> fy(variables.begin() + 3 * opt_data->t_steps, variables.end());
        return (i < opt_data->force_duration) ? 0.0 : fy[i];
    }


public:
    TrajectoryOptimizer(
        const std::vector<double>& start,
        const std::vector<double>& end,
        int t_steps,
        double dt,
        int force_duration
    )
    {
        opt_data.a = start;
        opt_data.b = end;
        opt_data.t_steps = t_steps;
        opt_data.dt = dt;
        opt_data.force_duration = force_duration;
    }

    void optimize() {
        int t_steps = opt_data.t_steps;
        int n_vars = 4 * t_steps;
        std::vector<double> initial_guess(n_vars, 0.0);

        for (int i = 0; i < t_steps; ++i) {
            initial_guess[i] = opt_data.a[0] + (opt_data.b[0] - opt_data.a[0]) * i / (t_steps - 1);
            initial_guess[i + t_steps] = opt_data.a[1] + (opt_data.b[1] - opt_data.a[1]) * i / (t_steps - 1);
            if (i < opt_data.force_duration) {
                initial_guess[i + 2 * t_steps] = 1.0;
                initial_guess[i + 3 * t_steps] = 5.0;
            }
        }

        nlopt::opt optimizer(nlopt::LD_SLSQP, n_vars);
        optimizer.set_min_objective(trajectory_energy, &opt_data);

        optimizer.add_equality_constraint(constraint_initial_x, &opt_data, 1e-8);
        optimizer.add_equality_constraint(constraint_initial_y, &opt_data, 1e-8);
        optimizer.add_equality_constraint(constraint_final_x, &opt_data, 1e-8);
        optimizer.add_equality_constraint(constraint_final_y, &opt_data, 1e-8);


        try {
            double minf;
            nlopt::result result = optimizer.optimize(initial_guess, minf);
            std::cout << "Optimization successful! Minimum energy: " << minf << std::endl;

            std::cout << "Optimized trajectory:\n";
            for (int i = 0; i < t_steps; ++i) {
                std::cout << "t=" << i * opt_data.dt << " x=" << initial_guess[i] << " y=" << initial_guess[i + t_steps] << std::endl;
            }
        }
        catch (std::exception& e) {
            std::cerr << "Optimization failed: " << e.what() << std::endl;
        }
    }

private:
    static constexpr double g = 9.81;
};



class Quaternion {
public:
    float w, x, y, z;

    
    Quaternion(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : w(w), x(x), y(y), z(z) 
    {}

    //// make quaternion by only one vector
    //// in this function assume that we are located origin and toward y axis (0,1,0)
    //// and calculate y axis vector and one vector that we input

    Quaternion(glm::vec3 target_point)
    {
        glm::vec3 reference_axis(0.0f, 1.0f, 0.0f);
        target_point[0] = target_point[0];
        target_point[1] = target_point[1];
        glm::vec3 direction = glm::normalize(target_point); // nomalize the target vector



        glm::vec3 rotation_axis = -glm::cross(reference_axis, direction);

        float axis_length = glm::length(rotation_axis);
        float cos_theta = glm::dot(reference_axis, direction);
        float angle = std::acos(glm::clamp(cos_theta, -1.0f, 1.0f)); // θ = arccos(cosθ)

        if (axis_length < 1e-6)
        {
            //cout << " error" << endl;
            rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);

        }
        else
        {
            rotation_axis = glm::normalize(rotation_axis);

        }

        rotation_axis = (axis_length > 1e-6) ? glm::normalize(rotation_axis) : glm::vec3(1.0f, 0.0f, 0.0f);

        w = std::cos(angle / 2.0f);
        float sin_half_angle = std::sin(angle / 2.0f);

        x = rotation_axis.x * sin_half_angle;
        y = rotation_axis.y * sin_half_angle;
        z = rotation_axis.z * sin_half_angle;
    }


    //// set quaternion by two vector
    //// this quaternion is rotate from first vector to second vector
    Quaternion(const glm::vec3& origin_v1, const glm::vec3& goal_v2) {
        //// calculate inner product and find angle of two vector
        float cos_theta = glm::dot(origin_v1, goal_v2);
        float angle = std::acos(glm::clamp(cos_theta, -1.0f, 1.0f)); // θ = arccos(cosθ)

        //// calculate cross product
        glm::vec3 rotation_axis = glm::cross(origin_v1, goal_v2);
        float axis_length = glm::length(rotation_axis);
        if (axis_length < 1e-6)
        {

            rotation_axis = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else
        {
            rotation_axis = glm::normalize(rotation_axis);

        }

        //rotation_axis = (axis_length > 1e-6f) ? glm::normalize(rotation_axis) : glm::vec3(1.0f, 0.0f, 0.0f);
        //// we can make quatenrion by two vector
        float sin_half_angle = std::sin(angle / 2.0f);

        
        w = std::cos(angle / 2.0f);
        x = rotation_axis.x * sin_half_angle;
        y = rotation_axis.y * sin_half_angle;
        z = rotation_axis.z * sin_half_angle;

 
    }
    Quaternion(
        const float& angle,
        glm::vec3 rotation_axis
    )
    {
        rotation_axis = glm::normalize(rotation_axis);
        float sin_half_angle = sin(glm::radians(angle) / 2.0f);

        w = cos(glm::radians(angle) / 2.0f);
        x = rotation_axis.x * sin_half_angle;
        y = rotation_axis.y * sin_half_angle;
        z = rotation_axis.z * sin_half_angle;

    }


    Quaternion(const glm::mat3& m)
    {
        Quaternion q;
        float trace = m[0][0] + m[1][1] + m[2][2];

        if (trace > 0.0f)
        {
            float s = std::sqrt(trace + 1.0f) * 2.0f; // s=4*qw
            w = 0.25f * s;
            x = (m[2][1] - m[1][2]) / s;
            y = (m[0][2] - m[2][0]) / s;
            z = (m[1][0] - m[0][1]) / s;
        }
        else
        {
            if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
            {
                float s = std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f; // s=4*qx
                w = (m[2][1] - m[1][2]) / s;
                x = 0.25f * s;
                y = (m[0][1] + m[1][0]) / s;
                z = (m[0][2] + m[2][0]) / s;
            }
            else if (m[1][1] > m[2][2])
            {
                float s = std::sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f; // s=4*qy
                w = (m[0][2] - m[2][0]) / s;
                x = (m[0][1] + m[1][0]) / s;
                y = 0.25f * s;
                z = (m[1][2] + m[2][1]) / s;
            }
            else
            {
                float s = std::sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f; // s=4*qz
                w = (m[1][0] - m[0][1]) / s;
                x = (m[0][2] + m[2][0]) / s;
                y = (m[1][2] + m[2][1]) / s;
                z = 0.25f * s;
            }
        }
    }

    float norm() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }


    Quaternion normalize() const {
        float n = norm();
        return Quaternion(w / n, x / n, y / n, z / n);
    }

    std::tuple<float, float, float> toEulerAngles() const {
        float yaw = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
        float sinp = 2.0f * (w * y - z * x);
        float pitch = 0.0f;

        float roll = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

        return std::make_tuple(roll, pitch, yaw);
    }

    static Quaternion quaternion_fromeuler(float input_roll, float input_pitch, float input_yaw) {

        float yaw = glm::radians(input_roll);
        float roll = glm::radians(-input_pitch);
        float pitch = glm::radians(-input_yaw);

        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);

        return Quaternion(
            cr * cp * cy + sr * sp * sy,  // w
            sr * cp * cy - cr * sp * sy,  // x
            cr * sp * cy + sr * cp * sy,  // y
            cr * cp * sy - sr * sp * cy   // z
        );
    }

    glm::vec3 euler_fromquaternion()
    {
        glm::vec3 angle_xyz;

        float t0 = sqrt(1 + 2 * (w * y - x * z));
        float t1 = sqrt(1 - 2 * (w * y - x * z));

        angle_xyz[2] = glm::degrees(2 * atan2(t0, t1) - 3.141592 / 2);

        float sincosp = 2.0f * (w * x + y * z);
        float coscosp = 1 - 2 * (x * x + y * y);
        float pitch = atan2(sincosp, coscosp);

        angle_xyz[1] = glm::degrees(pitch);


        float t2 = 2 * (w * z + x * y);
        float t3 = 1 - 2 * (y * y + z * z);

        float roll = std::atan2(t2, t3);
        
        angle_xyz[0] = glm::degrees(roll);

        return angle_xyz;
    }





    Quaternion multiply(const Quaternion& q) const {
        return Quaternion(
            w * q.w - x * q.x - y * q.y - z * q.z,  // w
            w * q.x + x * q.w + y * q.z - z * q.y,  // x
            w * q.y - x * q.z + y * q.w + z * q.x,  // y
            w * q.z + x * q.y - y * q.x + z * q.w   // z
        );
    }
    glm::mat4 quaternion_to_r_matrix() 
    {
        glm::mat4 matrix;

        matrix[0][0] = 1 - 2 * (y * y + z * z);
        matrix[0][1] = 2 * (x * y - w * z);
        matrix[0][2] = 2 * (x * z + w * y);
        matrix[0][3] = 0;

        matrix[1][0] = 2 * (x * y + w * z);
        matrix[1][1] = 1 - 2 * (x * x + z * z);
        matrix[1][2] = 2 * (y * z - w * x);
        matrix[1][3] = 0;

        matrix[2][0] = 2 * (x * z - w * y);
        matrix[2][1] = 2 * (y * z + w * x);
        matrix[2][2] = 1 - 2 * (x * x + y * y);
        matrix[2][3] = 0;

        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;
        matrix[3][3] = 1;

        return matrix;
    }

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& q) {
        os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
        return os;
    }

    glm::mat4 quaternion_to_r_matrix_x_inverse()
    {
        glm::mat4 matrix;

        matrix[0][0] = 1 - 2 * (y * y + z * z);
        matrix[0][1] = -2 * (x * y - w * z); 
        matrix[0][2] = -2 * (x * z + w * y); 
        matrix[0][3] = 0;

        matrix[1][0] = -2 * (x * y + w * z); 
        matrix[1][1] = 1 - 2 * (x * x + z * z);
        matrix[1][2] = 2 * (y * z - w * x);
        matrix[1][3] = 0;

        matrix[2][0] = -2 * (x * z - w * y); 
        matrix[2][1] = 2 * (y * z + w * x);
        matrix[2][2] = 1 - 2 * (x * x + y * y);
        matrix[2][3] = 0;

        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;
        matrix[3][3] = 1;

        return matrix;
    }

    glm::mat4 quaternion_to_r_matrix_y_inverse()
    {
        glm::mat4 matrix;

        matrix[0][0] = 1 - 2 * (y * y + z * z);
        matrix[0][1] = 2 * (x * y - w * z);
        matrix[0][2] = -2 * (x * z + w * y); 
        matrix[0][3] = 0;

        matrix[1][0] = 2 * (x * y + w * z);
        matrix[1][1] = 1 - 2 * (x * x + z * z);
        matrix[1][2] = -2 * (y * z - w * x); 
        matrix[1][3] = 0;

        matrix[2][0] = 2 * (x * z - w * y);
        matrix[2][1] = 2 * (y * z + w * x);
        matrix[2][2] = 1 - 2 * (x * x + y * y);
        matrix[2][3] = 0;

        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;
        matrix[3][3] = 1;

        return matrix;
    }
    glm::mat4 quaternion_to_r_matrix_z_inverse()
    {
        glm::mat4 matrix;
        matrix[0][0] = 1 - 2 * (y * y + z * z);
        matrix[0][1] = 2 * (x * y - w * z);
        matrix[0][2] = 2 * (x * z + w * y);
        matrix[0][3] = 0;

        matrix[1][0] = 2 * (x * y + w * z);
        matrix[1][1] = 1 - 2 * (x * x + z * z);
        matrix[1][2] = 2 * (y * z - w * x);
        matrix[1][3] = 0;

        matrix[2][0] = -2 * (x * z - w * y); 
        matrix[2][1] = -2 * (y * z + w * x); 
        matrix[2][2] = 1 - 2 * (x * x + y * y);
        matrix[2][3] = 0;

        matrix[3][0] = 0;
        matrix[3][1] = 0;
        matrix[3][2] = 0;
        matrix[3][3] = 1;

        return matrix;
    }
    Quaternion slerp(const Quaternion& q2, float t)
    {
        // Compute the cosine of the angle between the two quaternions
        float dot = x * q2.x + y * q2.y + z * q2.z + w * q2.w;

        Quaternion q2_copy = q2;

        // If dot is negative, negate q2 to take the shorter path
        if (dot < 0.0f)
        {
            dot = -dot;
            q2_copy.x = -q2.x;
            q2_copy.y = -q2.y;
            q2_copy.z = -q2.z;
            q2_copy.w = -q2.w;
        }

        const float EPSILON = 1e-6f;

        // If the quaternions are close, use linear interpolation
        if (dot > 1.0f - EPSILON)
        {
            Quaternion result;
            result.x = x + t * (q2_copy.x - x);
            result.y = y + t * (q2_copy.y - y);
            result.z = z + t * (q2_copy.z - z);
            result.w = w + t * (q2_copy.w - w);
            result.Normalize();
            return result;
        }

        // Use SLERP
        float theta = std::acos(dot);


        float sin_theta = std::sin(theta);

        float weight1 = std::sin((1.0f - t) * theta) / sin_theta;
        float weight2 = std::sin(t * theta) / sin_theta;

        Quaternion result;
        result.x = weight1 * x + weight2 * q2_copy.x;
        result.y = weight1 * y + weight2 * q2_copy.y;
        result.z = weight1 * z + weight2 * q2_copy.z;
        result.w = weight1 * w + weight2 * q2_copy.w;

        return result;
    }

    // force rotation direction when interpolate quaternion
    Quaternion force_rotation_dir_slerp(const Quaternion& q2, float t, bool force_given_direction = false)
    {

        float dot = x * q2.x + y * q2.y + z * q2.z + w * q2.w;

        Quaternion q2_copy = q2;
        std::cout << dot << std::endl;
        std::cout << glm::degrees(std::acos(dot)) << std::endl;

        if (!force_given_direction && dot <  0.0f)
        {
            dot = -dot;
            q2_copy.x = -q2.x;
            q2_copy.y = -q2.y;
            q2_copy.z = -q2.z;
            q2_copy.w = -q2.w;

        }

        const float EPSILON = 1e-6f;

        if (dot > 1.0f - EPSILON)
        {
            Quaternion result;
            result.x = x + t * (q2_copy.x - x);
            result.y = y + t * (q2_copy.y - y);
            result.z = z + t * (q2_copy.z - z);
            result.w = w + t * (q2_copy.w - w);
            result.Normalize();
            return result;
        }

        float theta = std::acos(dot);


        float sin_theta = std::sin(theta);

        float weight1 = std::sin((1.0f - t) * theta) / sin_theta;
        float weight2 = std::sin(t * theta) / sin_theta;

        Quaternion result;
        result.x = weight1 * x + weight2 * q2_copy.x;
        result.y = weight1 * y + weight2 * q2_copy.y;
        result.z = weight1 * z + weight2 * q2_copy.z;
        result.w = weight1 * w + weight2 * q2_copy.w;

        result.Normalize();
        return result;


        
    }

    void Normalize()
    {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len > 1e-6f)
        {
            x /= len;
            y /= len;
            z /= len;
            w /= len;
        }
    }
};

class Mymath
{
public:





};

class Interpolate
{
public:

    template <typename T>
    T do_interpolate(
        const T& start,
        const T& end,
        const float& t
    )
    {
        return (1.0f - t) * start + t * end;
    }






};


class Crossing_check
{
public:


    Crossing_check(){}
    ~Crossing_check() {}

    glm::vec3 ray_plane_check(
        const glm::vec3 plane_nomal,
        const glm::vec3 plane_point, // the point on plane 
        const glm::vec3 near_point,
        const glm::vec3 far_point,
        const glm::vec3 ray
    )
    {

        float  dot_check = glm::dot(plane_nomal, ray);

        float d = -(plane_nomal[0] * plane_point[0] + plane_nomal[1] * plane_point[1] + plane_nomal[2] * plane_point[2]);

        float t = (-d - glm::dot(plane_nomal, near_point)) / glm::dot(plane_nomal, ray);
        
        glm::vec3 crossing_point = near_point + t * ray;



        return crossing_point;
    }
    


};
class Set_ray
{
public:
    Set_ray() {};
    ~Set_ray() {};

    glm::vec3 calculate_ray_for_nearpoint_by_pixel(
        glm::mat4 projection,
        glm::mat4 view,
        float mouse_pos_x,
        float mouse_pos_y,
        float scene_width,
        float scene_height
    )
    {


        float win_x = 2 * mouse_pos_x / scene_width - 1;
        float win_y = 1 - 2 * mouse_pos_y / scene_height;

        glm::mat4 un_view_M = glm::inverse(projection * view);

        glm::vec4 near_point_4 = un_view_M * glm::vec4(win_x, win_y, -1, 1);


        glm::vec3 near_point;
        near_point.x = near_point_4.x / near_point_4.w;
        near_point.y = near_point_4.y / near_point_4.w;
        near_point.z = near_point_4.z / near_point_4.w;

        return near_point;



    }
    glm::vec3 calculate_ray_for_farpoint_by_pixel(
        glm::mat4 projection,
        glm::mat4 view,
        float mouse_pos_x,
        float mouse_pos_y,
        float scene_width,
        float scene_height
    )
    {


        float win_x = 2 * mouse_pos_x / scene_width - 1;
        float win_y = 1 - 2 * mouse_pos_y / scene_height;

        glm::mat4 un_view_M = glm::inverse(projection * view);


        glm::vec4 far_point_4 = un_view_M * glm::vec4(win_x, win_y, 1, 1);


        glm::vec3 far_point;
        far_point.x = far_point_4.x / far_point_4[3];
        far_point.y = far_point_4.y / far_point_4[3];
        far_point.z = far_point_4.z / far_point_4[3];

        return far_point;


    }





};

class Ray_check
{
public:
    Ray_check(){}
    ~Ray_check() {}

    glm::vec2 check_by_ray(
        const glm::vec3& near_point,
        const glm::vec3& dir,
        const glm::vec3& target,
        const float& r //// radius of sphere
        

        )
    {
        //// A_minus C does not have a physical meaning
        ////however, since the final equation takes the form a = dir^2, b = 2 * dir * A_minus_C, and c = A_minus_C^2 - r^2 by sphere equation
        //// so we define this variable for "convenience"
        glm::vec3 A_minus_C = near_point - target;

        float a = dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2];
        float b = 2 * (A_minus_C[0] * dir[0] + A_minus_C[1] * dir[1] + A_minus_C[2] * dir[2]);
        float c = A_minus_C[0] * A_minus_C[0] + A_minus_C[1] * A_minus_C[1] + A_minus_C[2] * A_minus_C[2] - r * r;

        float t1 = -b + sqrt(b * b - 4 * a * c) / (2 * a);
        float t2 = -b - sqrt(b * b - 4 * a * c) / (2 * a);

        return glm::vec2(t1, t2);


    }


    void check_closest_num(
        float& min_t,
        int& min_distance_num,
        glm::vec2& t12,
        const int& num
    )
    {
        float dis_t = t12[0] - t12[1];

        if (dis_t > 0 && dis_t > min_t && abs(t12[0]) > abs(t12[1]))
        {

            min_distance_num = num;
            min_t = dis_t;


        }

    }



};

class Ray_aabb_check
{
public:
    bool RayIntersectsAABB(
        const glm::vec3& ray_dir,
        const glm::vec3& ray_origin,
        const glm::vec3& box_min,
        const glm::vec3& box_max,
        float* outT 
    )
    {
        float tNear = -std::numeric_limits<float>::infinity();
        float tFar = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; ++i) {
            float origin = ray_origin[i];
            float dir = ray_dir[i];
            float minB = box_min[i];
            float maxB = box_max[i];

            if (fabs(dir) < 1e-8f) {
                if (origin < minB || origin > maxB)
                    return false;
            }
            else {
                float t1 = (minB - origin) / dir;
                float t2 = (maxB - origin) / dir;
                if (t1 > t2) std::swap(t1, t2);

                if (t1 > tNear) tNear = t1;
                if (t2 < tFar) tFar = t2;

                if (tNear > tFar) return false;
                if (tFar < 0) return false;
            }
        }

        if (outT) *outT = tNear; 
        return true;
    }


};













#endif