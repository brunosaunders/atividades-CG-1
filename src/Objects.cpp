
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

Vector3d Object::get_light_vector(Vector3d intersection_point, SourceOfLight source)
{
    return (source.center.minus(intersection_point)).get_vector_normalized();
}

// Sphere
// n unitary vector (normal vector).
Vector3d Sphere::get_normal_vector(Vector3d intersection_point)
{
    return (intersection_point.minus(this->center)).divide(this->radius);
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


Intersection Plan::get_intersection(Ray ray) {
    Vector3d w = ray.p1.minus(this->known_point);
    Vector3d dr = ray.get_dr();

    float t_int = -(this->normal.scalar_product(w))/(this->normal.scalar_product(dr));

    return Intersection(t_int, t_int > 0);
}

// We can pass any value of interserction_point
Vector3d Triangle::get_normal_vector(Vector3d intersection_point = Vector3d(0,0,0)) {
    Vector3d N = this->r1.vectorial_product(this->r2); // r1 x r2
    return N.get_vector_normalized();
}


Intersection Triangle::get_intersection(Ray ray) {
    Vector3d normal_vector = this->get_normal_vector();
    float intersec_t = - ((ray.p1.minus(this->p1).scalar_product(normal_vector)) / ray.get_dr().scalar_product(normal_vector));

    if (intersec_t == INFINITY || intersec_t == NAN)  {
        throw std::logic_error("Intersec time is invalid (inf or nan).");
    }

    Vector3d intersec_point = ray.p1.sum(ray.get_dr().multiply(intersec_t));
    float total_area = this->p1.vectorial_product(this->p2).scalar_product(normal_vector);

    float c1 = (this->p3.minus(intersec_point).vectorial_product(this->p1.minus(intersec_point))).scalar_product(normal_vector) / total_area;
    float c2 = (this->p1.minus(intersec_point).vectorial_product(this->p2.minus(intersec_point))).scalar_product(normal_vector) / total_area;
    float c3 = (this->p2.minus(intersec_point).vectorial_product(this->p3.minus(intersec_point))).scalar_product(normal_vector) / total_area;
    if (c1 < 0 || c2 < 0 || c3 < 0 ) {
        return Intersection(intersec_t, false);
    }

    return Intersection(intersec_t, true);
}

Vector3d Cylinder::get_normal_vector(Vector3d intersection_point) {
    float error = 1e-12;
    // The vector that goes from the center of the base to the intersection point
    Vector3d vector_base_point = intersection_point.minus(this->base_center);
    // Scalar product from the vector_base_point with the direction vector of the cylinder
    float vector_scalar_cylinderdr = vector_base_point.scalar_product(this->cyl_direction);
    // Projection of the vector_base_point at the direction of the cylinder
    Vector3d vector_projection = this->cyl_direction.multiply(vector_scalar_cylinderdr);
    // Vector that goes from the intersection point to the closest surface of cylinder perpendicular to the cylinder's direction
    Vector3d vector_point_closest_surface = vector_base_point.minus(vector_projection);

    if(error + 0.0 < vector_scalar_cylinderdr && vector_scalar_cylinderdr < this->height - error)
    {
        return vector_point_closest_surface.get_vector_normalized();
    }
    else if(vector_scalar_cylinderdr <= error + 0.0)
    {
        return this->cyl_direction.multiply(-1.0);
    }
    else
    {
        return this->cyl_direction;
    }
}

Intersection Cylinder::get_intersection(Ray ray) {

    // We want to check if ||(Pint - B) - ((Pint - B) . u) u)||²=R²
    // ||(Pin + tint*dr - B) - ((Pin + tint*dr - B) . u)u)||² = R²
    // ||(Pin - B) - ((Pin - B) . u)u + tint(dr - (d . u)u)||² = R²
    // s = (Pin - B)
    // V = s - u * (s . u)
    // W = d - u * (d . u)
    // ||V + tintW||² = R²
    // (W . W)tint² + 2(V . W)tint + ((V . V) - R² = 0
    // a = (W . W)
    // b = (V . W)
    // c = (V . V) - R²
    Vector3d initial_point = ray.p1;
    // Initial point of the ray
    Vector3d dr = ray.get_dr();
    // Vector direction of the ray
    Vector3d s = initial_point.minus(this->base_center);
    // Vector that goes from the center of the cylinder to the origin of the ray
    Vector3d dc = (this->top_center).minus(this->base_center);
    // Vector direction of the cylinder
    Vector3d W = dr.minus(dc.multiply(dr.scalar_product(dc)));
    Vector3d V = s.minus(dc.multiply(s.scalar_product(dc)));
    float t_int, t_int_1, t_int_2, t_int_base, t_int_top, vector_scalar_dc;
    Plan
    // Plane of the base of the cylinder
    base(this->base_center, this->cyl_direction, IntensityColor(.2, .7, .2), IntensityColor(0, 0, 0), IntensityColor(.2, .7, .2), 1, Color(50, 25, 199)),
    // Plane of the top of the cylinder
    top(this->top_center, this->cyl_direction, IntensityColor(.2, .7, .2), IntensityColor(0, 0, 0), IntensityColor(.2, .7, .2), 1, Color(50, 25, 199));

    // Checks if the ray is parallel to the vector direction of the cylinder
    if(std::abs(dr.scalar_product(this->cyl_direction)) != 1.0)
    {
        // If not...
        float a = W.scalar_product(W);
        float b = V.scalar_product(W);
        float c = V.scalar_product(V) - std::pow(this->radius, 2);
        float delta = std::pow(b, 2) - (4 * a * c);
        // If delta > 0, then there are two intersection points
        if(delta > 0.0)
        {
            t_int_1 = (-b + std::sqrt(delta)) / (2.0 * a);
            t_int_2 = (-b - std::sqrt(delta)) / (2.0 * a);

            // Checks if the first interception point is valid
            vector_scalar_dc = ((initial_point.sum(dr.multiply(t_int_1))).minus(this->base_center)).scalar_product(this->cyl_direction);
            if(vector_scalar_dc < 0.0 || vector_scalar_dc > this->height)
            {
                t_int_1 = -1.0;
            }
            // Checks if the second interception point is valid
            vector_scalar_dc = ((initial_point.sum(dr.multiply(t_int_2))).minus(this->base_center)).scalar_product(this->cyl_direction);
            if(vector_scalar_dc < 0.0 || vector_scalar_dc > this->height)
            {
                t_int_2 = -1.0;
            }
            if(t_int_1 >= 0.0 && t_int_2 >= 0.0)
            {
                if(t_int_1 < t_int_2)
                {
                    t_int = t_int_1;
                }
                else
                {
                    t_int = t_int_2;
                }
            }
            else
            {
                t_int_base = (base.get_intersection(ray)).time;
                t_int_top = (top.get_intersection(ray)).time;

                if(t_int_base >= 0.0 && ((initial_point.sum(dr.multiply(t_int_base))).minus(this->base_center).size()) > this->radius)
                {
                    t_int_base = -1.0;
                }

                if(t_int_top >= 0.0 && ((initial_point.sum(dr.multiply(t_int_top))).minus(this->top_center).size()) > this->radius)
                {
                    t_int_top = -1.0;
                }

                if(t_int_base >= 0.0 && t_int_top >= 0.0)
                {
                    if (t_int_base < t_int_top)
                    {
                        t_int = t_int_base;
                    }
                    else
                    {
                        t_int = t_int_top;
                    }
                }
                else
                {
                    if(t_int_base >= 0.0)
                    {
                        t_int = t_int_base;
                    }
                    else if(t_int_top >= 0.0)
                    {
                        t_int = t_int_top;
                    }
                    else
                    {
                        return Intersection(-1.0, false);
                    }

                    if(t_int_1 >= 0.0)
                    {
                        t_int = t_int_1;
                    }
                    else {
                        t_int = t_int_2;
                    }
                }
            }
        }
        // If delta == 0, then there is only on intersection point
        else if(delta == 0)
        {
            t_int_1 = -b / (2 * a);
            vector_scalar_dc = ((initial_point.sum(dr.multiply(t_int_1))).minus(this->base_center)).scalar_product(this->cyl_direction);
            if(0.0 <= vector_scalar_dc && vector_scalar_dc <= this->height)
            {
                t_int = t_int_1;
            }
            else
            {
                return Intersection(-1.0, false);
            }
        }
        // If delta < 0, then there is no intersection point at all
        else
        {
            return Intersection(-1.0, true);
        }
    }
    else
    {
        // If it is parallel...
        t_int_base = (base.get_intersection(ray)).time;
        t_int_top = (top.get_intersection(ray)).time;

        if(t_int_base >= 0.0 && ((initial_point.sum(dr.multiply(t_int_base))).minus(this->base_center).size()) > this->radius)
        {
            t_int_base = -1.0;
        }

        if(t_int_top >= 0.0 && ((initial_point.sum(dr.multiply(t_int_top))).minus(this->top_center).size()) > this->radius)
        {
            t_int_top = -1.0;
        }

        if(t_int_base >= 0.0 && t_int_top >= 0.0)
        {
            if (t_int_base < t_int_top)
            {
                t_int = t_int_base;
            }
            else
            {
                t_int = t_int_top;
            }
        }
        else if(t_int_base >= 0.0)
        {
            t_int = t_int_base;
        }
        else if(t_int_top >= 0.0)
        {
            t_int = t_int_top;
        }
        else
        {
            return Intersection(-1.0, false);
        }
    }
    return Intersection(t_int, true);
}
