 #include <iostream>
 
 #include "Vector3d.hpp"
 #include "Ray.hpp"

using namespace atividades_cg_1::algebra;


float Ray::size()
{
    return (p2.minus(p1)).size();
}

// Unitary direction vector
Vector3d Ray::get_dr()
{
    return (p2.minus(p1)).divide(this->size());
}

