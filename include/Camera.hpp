#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "Algebra.hpp"
#include "Color.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;

namespace atividades_cg_1::camera {
    const int CHANGE_FROM_WORLD_TO_CAMERA = 1;
    const int CHANGE_FROM_CAMERA_TO_WORLD = 2;

    class Window
    {
    public:
        Vector3d center;
        float width;
        float height;

        int cols;
        int rows;

        // dx and dy of rectangles
        float dx;
        float dy;

        std::vector<std::vector<Color>> windows_colors;
        bool should_update = true;

        Window() {}
        Window(float width, float height, int cols, int rows, float x, float y, float z);
    };


    class Camera {
        protected:
            Matrix* world_to_camera;
            Matrix* camera_to_world;
        public:
            Vector3d look_at;
            Vector3d eye;
            Vector3d view_up;
            Vector3d kc;
            Vector3d jc;
            Vector3d ic;

            float focal_distance;
            Window window;

            Camera(){}
            Camera(Vector3d look_at, Vector3d eye, Vector3d view_up, float d, Window window);
            Camera(Vector3d look_at, Vector3d eye, Vector3d view_up, float d, float width, 
            float height, int cols, int rows);

            Vector3d transform_vector_from_world_to_camera(Vector3d v);
            Vector3d transform_vector_from_camera_to_world(Vector3d v);
            void destroy();
            void calculate_everything();
            void set_eye(Vector3d eye);
            void set_look_at(Vector3d look_at);

    };
}

#endif