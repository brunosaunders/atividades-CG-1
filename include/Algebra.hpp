#ifndef ALGEBRA_HPP_
#define ALGEBRA_HPP_

#include <vector>
#include <cmath>

using namespace std;

namespace atividades_cg_1::algebra
{
    const int X_AXIS = 1;
    const int Y_AXIS = 2;
    const int Z_AXIS = 3;
    const int ARBITRARY_AXIS = 4;

    class Vector3d;
    class MatrixDimension
    {
    public:
        int n;
        int m;
        MatrixDimension() {}
        MatrixDimension(int n, int m) : n(n), m(m) {}

        bool equals(MatrixDimension other);
        bool can_multiply(MatrixDimension other);
    };

    class Matrix
    {
    public:
        vector<vector<float>> matrix;
        MatrixDimension dimension;

        Matrix() {}
        Matrix(vector<vector<float>> m) : matrix(m)
        {
            MatrixDimension dimension = MatrixDimension(m.size(), m[0].size());
            this->dimension = dimension;
        }

        Matrix sum(Matrix other);
        Matrix minus(Matrix other);

        Matrix multiply(Matrix other);

        Matrix multiply_scalar(float v);
        Matrix divide_scalar(float v);

        Vector3d as_vector();
        void print();
    };

    
    // A vector that works on Creto's coordinate system
    class Vector3d
    {
    public:
        float x;
        float y;
        float z;
        int8_t is_point;

        Vector3d() {}
        Vector3d(float x, float y, float z, int8_t is_point = 1) : x(x), y(y), z(z), is_point(is_point) {}

        Vector3d multiply(float value);
        Vector3d divide(float value);
        Vector3d sum(Vector3d v);
        Vector3d minus(Vector3d v);

        float size();

        float scalar_product(Vector3d v);
        Vector3d vectorial_product(Vector3d v);

        Vector3d get_vector_normalized();

        bool equals(Vector3d other);
        Matrix as_matrix();

        Vector3d apply_transformation(Matrix transformation);

        void print();

        // friend std::ostream& operator<<(std::ostream& os, const Vector3d& v);
    };

    class Ray
    {
    public:
        Vector3d p1;
        Vector3d p2;

        Ray() {}

        Ray(Vector3d p1, Vector3d p2) : p1(p1), p2(p2) {}

        float size();

        // Unitary direction vector
        Vector3d get_dr();
        // friend std::ostream& operator<<(std::ostream& os, const Ray& r);
    };

    // Responsible of building Transform Matrix
    class MatrixTransformations
    {
    public:
        // Reusable matrix to all points (no point dependency).
        static Matrix translation(float tx, float ty, float tz);

        // Needs the vector you want to scale, so it is not reusable, we have to build a matrix for every point.
        static Matrix scale(Vector3d v, float sx, float sy, float sz);

        static Matrix rotation(float theta, int axis);
        static Matrix rotation(float sen_theta, float cos_theta, int axis);

        static Matrix arbitrary_rotation(float theta, Vector3d p1, Vector3d p2);
    };
}

#endif