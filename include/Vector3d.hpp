#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <iostream>

/*  Creto's system
    Window center is at (0, 0, -d)
    Observer is at (0, 0, 0)
*/
namespace atividades_cg_1::algebra {

    // A vector that works on Creto's coordinate system
    class Vector3d
    {
    public:
        float x;
        float y;
        float z;

        Vector3d() {}
        Vector3d(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector3d multiply(float value);
        Vector3d divide(float value);
        Vector3d sum(Vector3d v);
        Vector3d minus(Vector3d v);

        float size();

        float scalar_product(Vector3d v);
        Vector3d vectorial_product(Vector3d v);

        Vector3d get_vector_normalized();

        bool equals(Vector3d other);

        // friend std::ostream& operator<<(std::ostream& os, const Vector3d& v);

    };

    // std::ostream &operator<<(std::ostream &os, Vector3d const &v);
}


#endif