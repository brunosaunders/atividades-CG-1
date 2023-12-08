#include "Algebra.hpp"

 #include <iostream>
 #include <stdexcept>


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


void Vector3d::print() {
    std::cout << "v(x:" << this->x << ",y:" << this->y << ",z:" << this->z << ") ";
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
    float factor = 1e-12;
    return std::abs(this->x - other.x) <= factor 
                && std::abs(this->y - other.y) <= factor && std::abs(this->z - other.z) <= factor;
}


Matrix Vector3d::as_matrix() {
    vector<vector<float>> result(4, vector<float>(1, 0));
    result[0][0] = this->x;
    result[1][0] = this->y;
    result[2][0] = this->z;
    result[3][0] = this->is_point;

    return Matrix(result);
}


float Ray::size()
{
    return (p2.minus(p1)).size();
}

// Unitary direction vector
Vector3d Ray::get_dr()
{
    return (p2.minus(p1)).divide(this->size());
}


Matrix Matrix::sum(Matrix other) {
    if (!this->dimension.equals(other.dimension)) {
        throw runtime_error("Tamanho das matrizes é incompatível.");
    }

    vector<vector<float>> new_matrix = this->matrix;
    for (int i = 0; i < new_matrix.size(); i++) {
        for (int j = 0; j < new_matrix.size(); j++) {
            new_matrix[i][j] += other.matrix[i][j];
        }
    }

    return Matrix(new_matrix);
}

Matrix Matrix::minus(Matrix other) {
    if (!this->dimension.equals(other.dimension)) {
        throw runtime_error("Tamanho das matrizes é incompatível.");
    }
    
    vector<vector<float>> new_matrix = this->matrix;
    for (int i = 0; i < new_matrix.size(); i++) {
        for (int j = 0; j < new_matrix.size(); j++) {
            new_matrix[i][j] -= other.matrix[i][j];
        }
    }

    return Matrix(new_matrix);
}


Matrix Matrix::multiply_scalar(float v) {
    vector<vector<float>> new_matrix = this->matrix;
    for (int i = 0; i < new_matrix.size(); i++) {
        for (int j = 0; j < new_matrix.size(); j++) {
            new_matrix[i][j] *= v;
        }
    }

    return Matrix(new_matrix);
}


Matrix Matrix::divide_scalar(float v) {
    Matrix::multiply_scalar(1/v);
}


Matrix Matrix::multiply(Matrix other) {
    if (!this->dimension.can_multiply(other.dimension)) {
        throw runtime_error("Matrizes incompatíveis para a multiplicação.");
    }
    
    int n = this->dimension.n;
    int m = other.dimension.m;

    // vector<vector<float>> result(n, vector<float>(m, 0));
    vector<vector<float>> result;

    for (int i = 0; i < n; i++) { 
        vector<float> row;
        for (int j = 0; j < m; j++) {
            // row.push_back(0);
            // result[i][j] = 0; 
            float acc = 0; 
  
            for (int k = 0; k < other.dimension.n; k++) { 
                acc += this->matrix[i][k] * other.matrix[k][j]; 
            } 
            row.push_back(acc);
        } 
        result.push_back(row);
    }

    return Matrix(result);
}

Vector3d Matrix::as_vector() {
    if (this->dimension.m == 1 && this->dimension.n == 4) {
        return Vector3d(this->matrix[0][0], this->matrix[1][0], this->matrix[2][0], this->matrix[3][0]);
    }
    throw domain_error("A matriz não pode ser convertida para vetor.");
}

void Matrix::print() {
    for (auto& row : this->matrix) {
        for (auto& item : row) {
            cout << item << " ";
        }
        cout << endl;
    }
}


Vector3d Vector3d::apply_transformation(Matrix transformation) {
    return transformation.multiply(this->as_matrix()).as_vector();
}

bool MatrixDimension::equals(MatrixDimension other) {
    return (this->n == other.n && this->m == other.m);
}


bool MatrixDimension::can_multiply(MatrixDimension other) {
    return (this->m == other.n);
}


Matrix MatrixTransformations::translation(float tx, float ty, float tz) {
    vector<vector<float>> result{vector<float>{1,0,0,tx}, vector<float>{0,1,0,ty}, vector<float>{0,0,1,tz}, vector<float>{0,0,0,1}};
    return Matrix(result);
}


// Vector3d MatrixTransformations::apply(Vector3d v) {
//     return this->matrix.multiply(v.as_matrix()).as_vector();
// }