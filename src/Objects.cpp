
#include "Objects.hpp"
#include "Vector3d.hpp"
#include "Color.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::objects;


IntensityColor Object::get_difuse_contribution(Vector3d intersection_point, SourceOfLight source_of_light)
    {

        Vector3d l = this->get_light_vector(intersection_point, source_of_light);
        Vector3d n = this->get_normal_vector(intersection_point);
        float scalar_product_l_n = l.scalar_product(n);

        // if scalar_product is negative, the angle is greater than 180 degrees and we can't see the object. The color in result will be black.
        if (scalar_product_l_n < 0)
        {
            scalar_product_l_n = 0;
        }
        float f_difuse = scalar_product_l_n;
        IntensityColor contribution = source_of_light.intensity.arroba_multiply(this->difuse_reflectivity).multiply(f_difuse);

        return contribution;
    }

IntensityColor Object::get_specular_contribution(Vector3d intersection_point, Vector3d eye_point, SourceOfLight source_of_light)
{
    Vector3d l = this->get_light_vector(intersection_point, source_of_light);
    Vector3d n = this->get_normal_vector(intersection_point);

    Vector3d r = n.multiply(2).minus(l).get_vector_normalized();
    Vector3d v = eye_point.minus(intersection_point).get_vector_normalized();

    float f_specular = std::pow(r.scalar_product(v), this->shininess);

    IntensityColor contribution = source_of_light.intensity.arroba_multiply(this->specular_reflectivity).multiply(f_specular);
    return contribution;
}

// Sphere
    // n unitary vector (normal vector).
Vector3d Sphere::get_normal_vector(Vector3d intersection_point)
{
    return (intersection_point.minus(this->center)).divide(this->radius);
}

// l unitary vector in direction to light.
Vector3d Sphere::get_light_vector(Vector3d intersection_point, SourceOfLight source)
{
    return (source.center.minus(intersection_point)).get_vector_normalized();
}

Intersection Sphere::get_intersection(Ray ray)
{

    Vector3d initial_point = ray.p1;
    Vector3d dr = ray.get_dr();

    Vector3d w = initial_point.minus(this->center);

    // We want to check if ||Pint - C||² = R²
    // (Pin + tint*dr - C).(Pin + tint*dr - C) - R² = 0
    // (w + tint*dr).(w + tint*dr) - R² = 0
    // w² + 2w*tint*dr + tint²*dr² - R² = 0
    // dr²*tint² + 2w*dr*tint + w² -     R² = 0
    float a = dr.scalar_product(dr);
    float b = (w.multiply(2)).scalar_product(dr);
    float c = w.scalar_product(w) - std::pow(this->radius, 2);

    float delta = std::pow(b, 2) - (4 * a * c);
    float t1 = (-b + std::sqrt(delta)) / (2 * a);
    float t2 = (-b - std::sqrt(delta)) / (2 * a);

    if (delta < 0)
    {
        return Intersection(0.0, false);
    }

    Vector3d intersection_point;
    if (t1 < t2)
        return Intersection(t1, true);
    return Intersection(t2, true);
}


Vector3d Plan::get_normal_vector(Vector3d intersection_point) {
    return this->normal.get_vector_normalized();
}
Vector3d Plan::get_light_vector(Vector3d intersection_point, SourceOfLight source_of_light) {
    return source_of_light.center.minus(intersection_point).get_vector_normalized();
}

Intersection Plan::get_intersection(Ray ray) {
    Vector3d w = ray.p1.minus(this->known_point);
//        Vector3d w = this->known_point.minus(ray.p2);
    Vector3d dr = ray.get_dr();

    float t_int = -(this->normal.scalar_product(w))/(this->normal.scalar_product(dr));

    return Intersection(t_int, t_int > 0);
}

// We can pass any value of interserction_point
Vector3d Triangle::get_normal_vector(Vector3d intersection_point) {
    Vector3d N = this->p1.vectorial_product(this->p2); // r1 x r2
    return N.get_vector_normalized();
}

Intersection Triangle::get_intersection(Ray ray) {
    Vector3d normal_vector = this->get_normal_vector(Vector3d());
    float intersec_t = - ((ray.p1.minus(this->p1).scalar_product(normal_vector)) / ray.get_dr().scalar_product(normal_vector));

    Vector3d intersec_point = ray.p1.sum(ray.get_dr().multiply(intersec_t));
    float total_area = this->p1.vectorial_product(this->p2).scalar_product(normal_vector);

    float c1 = (this->p3.minus(intersec_point).vectorial_product(this->p1.minus(intersec_point))).scalar_product(normal_vector) / total_area;
    float c2 = (this->p1.minus(intersec_point).vectorial_product(this->p2.minus(intersec_point))).scalar_product(normal_vector) / total_area;
    float c3 = 1 - c1 - c2;

    if (c1 < 0 || c2 < 0 || c3 < 0 || c1 + c2 + c3 != 1 ) {
        return Intersection(intersec_t, false);
    }
    
    return Intersection(intersec_t, true);
    // TODO: check if intersection is valid.
}
