#ifndef OBJECTS_H
#define OBJECTS_H

#include "Color.hpp"
#include "Vector3d.hpp"
#include "Ray.hpp"
#include "Lights.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::lights;

namespace atividades_cg_1::objects {

    class Object
    {
    public:
        virtual ~Object() {}

        Object(Color color, IntensityColor dr, IntensityColor sr, IntensityColor er, float shininess)
        : color(color), difuse_reflectivity(dr), specular_reflectivity(sr), environment_reflectivity(er), shininess(shininess) {}
        // bool intersected_by(Ray ray) {}
        Color color;
        IntensityColor difuse_reflectivity;   // K_d
        IntensityColor specular_reflectivity; // K_e
        IntensityColor environment_reflectivity; // K_a
        float shininess;
        virtual Intersection get_intersection(Ray ray) { return Intersection(0.0, false); }
        virtual Vector3d get_normal_vector(Vector3d intersection_point) { return Vector3d(20,20,20);};
        virtual Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source_of_light) {return Vector3d(20,20,20); }

        IntensityColor get_difuse_contribution(Vector3d intersection_point, SourceOfLight source_of_light);

        IntensityColor get_specular_contribution(Vector3d intersection_point, Vector3d eye_point, SourceOfLight source_of_light);
    };


    class Sphere : public Object
    {
    public:
        Vector3d center;
        float radius;

        Sphere(Vector3d center, float radius, Color color, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, IntensityColor environment_reflectivity, float shininess)
        : Object(color, difuse_reflectivity, specular_reflectivity, environment_reflectivity, shininess), center(center), radius(radius) {}

        // n unitary vector (normal vector).
        Vector3d get_normal_vector(Vector3d intersection_point) override;

        // l unitary vector in direction to light.
        Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source) override;

        Intersection get_intersection(Ray ray) override;
    };

    class Plan: public Object {
    public:
        Vector3d known_point;
        Vector3d normal;

        Plan(Vector3d known_point, Vector3d normal, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, 
        IntensityColor environment_reflectivity, float shininess, Color color)
        : Object(color, difuse_reflectivity, specular_reflectivity, environment_reflectivity, shininess), known_point(known_point), normal(normal) {}

        Vector3d get_normal_vector(Vector3d intersection_point) override;
        Vector3d get_light_vector(Vector3d intersection_point, SourceOfLight source_of_light) override;

        Intersection get_intersection(Ray ray) override ;
    };
}

#endif