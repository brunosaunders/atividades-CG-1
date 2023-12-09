#include <vector>

#include "Camera.hpp"

using namespace atividades_cg_1::camera;

Camera::Camera(Vector3d look_at, Vector3d eye, Vector3d view_up, float d, float width,
               float height, int cols, int rows) : look_at(look_at), eye(eye), focal_distance(d)
{
    this->view_up = view_up;
    this->window = Window(width, height, cols, rows, eye.x, eye.y, eye.z - d);

    this->kc = eye.minus(look_at).get_vector_normalized();
    this->ic = view_up.vectorial_product(kc).get_vector_normalized();
    this->jc = kc.vectorial_product(ic);

    this->camera_to_world = new Matrix(vector<vector<float>>{
        vector<float>{this->ic.x, this->ic.y, this->ic.z, - this->ic.scalar_product(eye)},
        vector<float>{this->jc.x, this->jc.y, this->jc.z, - this->jc.scalar_product(eye)},
        vector<float>{this->kc.x, this->kc.y, this->kc.z, - this->kc.scalar_product(eye)},
        vector<float>{0,0,0,1.0}
        });

    this->world_to_camera = new Matrix(vector<vector<float>>{
        vector<float>{this->ic.x, this->jc.x, this->kc.x, eye.x},
        vector<float>{this->ic.y, this->jc.y, this->kc.y, eye.y},
        vector<float>{this->ic.z, this->jc.z, this->kc.z, eye.z},
        vector<float>{0,0,0,1.0}
    });
}


Vector3d Camera::transform_vector_from_world_to_camera(Vector3d v) {
    return this->camera_to_world->multiply(v.as_matrix()).as_vector();
}


Vector3d Camera::transform_vector_from_camera_to_world(Vector3d v) {
    return this->world_to_camera->multiply(v.as_matrix()).as_vector();
}


void Camera::destroy() {
    delete this->camera_to_world;
    delete this->world_to_camera;
}

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
        this->windows_colors.push_back(row);
    }
}