 #include <iostream>
 
 #include "Vector3d.hpp"

using namespace atividades_cg_1::algebra;

Vector3d Vector3d::multiply(float value) {
    return Vector3d(this->x * value, this->y * value, this->z * value);
}

Vector3d Vector3d::divide(float value) {
    return this->multiply(1 / value);
}

Vector3d Vector3d::sum(Vector3d v)
{
    return Vector3d(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3d Vector3d::minus(Vector3d v)
{
    Vector3d new_v = v.multiply(-1);
    return this->sum(new_v);
}

float Vector3d::size()
{
    return sqrt(std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2));
}

float Vector3d::scalar_product(Vector3d v)
{
    return (this->x * v.x + this->y * v.y + this->z * v.z);
}

Vector3d Vector3d::vectorial_product(Vector3d other) {
    float new_x = this->y * other.z - this->z * other.y;
    float new_y = this->z * other.x - this->x * other.z;
    float new_z = this->x * other.y - this->y * other.x;
    return Vector3d(new_x, new_y, new_z);
}

Vector3d Vector3d::get_vector_normalized()
{
    return this->divide(this->size());
}

bool Vector3d::equals(Vector3d other) {
    return this->x == other.x && this->y == other.y && this->z == other.z;
}