 #include <iostream>
 
 #include "Vector3d.hpp"
 #include "Ray.hpp"

using namespace algebra;


float Ray::size()
{
    return (p2.minus(p1)).size();
}

// Unitary direction vector
Vector3d Ray::get_dr()
{
    return (p2.minus(p1)).divide(this->size());
}


// std::ostream &operator<<(std::ostream &os, Ray const &r)
// {
//     return os << "Ray(P1: " << r.p1 << " | P2: " << r.p2 << ")";
// }



