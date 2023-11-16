#ifndef RAY_H
#define RAY_H

#include <cmath>
#include <iostream>

#include "Vector3d.hpp"


namespace algebra {

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
    
    // std::ostream &operator<<(std::ostream &os, Ray const &r);
}


#endif