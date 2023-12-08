#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#include "Algebra.hpp"
#include "Color.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;

namespace atividades_cg_1::camera {
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
}

#endif