#include <vector>

#include "Camera.hpp"

using namespace atividades_cg_1::camera;

Window::Window(float width, float height, int cols, int rows, float x, float y, float z)
{
    this->width = width;
    this->height = height;
    this->cols = cols;
    this->rows = rows;
    this->center = Vector3d(x, y, z);

    this->dx = width / (float)cols;
    this->dy = height / (float)rows;

    for (int i = 0; i < rows; i++)
    {
        std::vector<Color> row;
        for (int j = 0; j < cols; j++)
        {
            row.push_back(Color(100, 100, 100));
        }
        windows_colors.push_back(row);
    }
}