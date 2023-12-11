#ifndef OBJECTS_H
#define OBJECTS_H

#include "Color.hpp"
#include "Algebra.hpp"
#include "Lights.hpp"
#include "Camera.hpp"

using namespace atividades_cg_1::camera;
using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::lights;


namespace atividades_cg_1::objects {
    class Object;
    class Intersection
    {
    public:
        float time;
        bool is_valid;
        Object *intersepted_object;

        Intersection() {}
        Intersection(float t, bool valid, Object *obj = NULL) : time(t), is_valid(valid), intersepted_object(obj) {}
    };

    class Object
    {
    public:
        virtual ~Object() {}

        Object() {}
        Object(Color color, IntensityColor dr, IntensityColor sr, IntensityColor er, float shininess)
        : color(color), difuse_reflectivity(dr), specular_reflectivity(sr), environment_reflectivity(er), shininess(shininess) {}

        Color color;
        IntensityColor difuse_reflectivity;   // K_d
        IntensityColor specular_reflectivity; // K_e
        IntensityColor environment_reflectivity; // K_a
        float shininess;

        virtual void print() {};

        virtual void apply_coordinate_change(Camera camera, int type_coord_change) {};
        virtual void apply_transformation(Matrix transformation) {};
        virtual void apply_scale_transformation(float sx, float sy, float sz) {};
        virtual void apply_rotation_transformation(float theta, int axis) {};

        virtual Intersection get_intersection(Ray ray) { return Intersection(0.0, false); }
        virtual Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) { return Vector3d();};
        Vector3d get_light_vector(Vector3d intersec_point, Intersection intersection, SourceOfLight source_of_light);

        IntensityColor get_difuse_contribution(Vector3d intersec_point, Intersection intersection, SourceOfLight source_of_light);

        IntensityColor get_specular_contribution(Vector3d intersec_point, Intersection intersection, Vector3d eye_point, SourceOfLight source_of_light);
    };


    class Sphere : public Object
    {
    public:
        Vector3d center;
        float radius;

        Sphere(Vector3d center, float radius, Color color, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, IntensityColor environment_reflectivity, float shininess)
        : Object(color, difuse_reflectivity, specular_reflectivity, environment_reflectivity, shininess), center(center), radius(radius) {}

        // n unitary vector (normal vector).
        Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) override;

        Intersection get_intersection(Ray ray) override;

        void apply_transformation(Matrix transformation) override;
        void apply_scale_transformation(float sx, float sy, float sz) override;

        void apply_coordinate_change(Camera camera, int type_coord_change) override;
    };

    class Plan: public Object {
    public:
        Vector3d known_point;
        Vector3d normal;

        Plan(Vector3d known_point, Vector3d normal, IntensityColor difuse_reflectivity, IntensityColor specular_reflectivity, 
        IntensityColor environment_reflectivity, float shininess, Color color)
        : Object(color, difuse_reflectivity, specular_reflectivity, environment_reflectivity, shininess), known_point(known_point), normal(normal.multiply(100000)) {}

        void apply_transformation(Matrix transformation) override;
        Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) override;
        Intersection get_intersection(Ray ray) override ;

        void apply_coordinate_change(Camera camera, int type_coord_change) override;
    };

    class Composite {
        public:
            
            Composite(){}

            Vector3d virtual get_center(){return Vector3d();};
    };

    class Triangle : public Object, public Composite {
        protected:
            Vector3d p1;
            Vector3d p2;
            Vector3d p3;

        public:
            Triangle(){}
            Triangle(Vector3d p1, Vector3d p2, 
            Vector3d p3, Color color=Color(255,255,255), 
            IntensityColor dr=IntensityColor(.7, .7, .7), IntensityColor sr=IntensityColor(.7, .7, .7),
            IntensityColor er=IntensityColor(.7, .7, .7), float shininess=10);
            Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) override;

            void apply_transformation(Matrix transformation) override;
            void apply_scale_transformation(float sx, float sy, float sz) override;
            void apply_rotation_transformation(float theta, int axis) override;

            void apply_coordinate_change(Camera camera, int type_coord_change) override;

            Vector3d get_p1();
            Vector3d get_p2();
            Vector3d get_p3();

            Intersection get_intersection(Ray ray) override;
            Vector3d get_center() override;
    };

    class FourPointsFace : public Object, public Composite {
        protected:
            Triangle t1;
            Triangle t2;
        
        public:
        FourPointsFace() {}
            FourPointsFace(Vector3d p1, Vector3d p2,
                    Vector3d p3, Vector3d p4, Color color = Color(255, 255, 255),
                    IntensityColor dr = IntensityColor(.7, .7, .7), IntensityColor sr = IntensityColor(.7, .7, .7),
                    IntensityColor er = IntensityColor(.7, .7, .7), float shininess = 10);

            Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) override;

            void apply_transformation(Matrix transformation) override;
            void apply_scale_transformation(float sx, float sy, float sz) override;
            void apply_rotation_transformation(float theta, int axis) override;
            void apply_coordinate_change(Camera camera, int type_coord_change) override;
            void print() override;

            Intersection get_intersection(Ray ray) override;

            Triangle get_t1();
            Triangle get_t2();
            Vector3d get_center() override;

    };

    class Mesh : public Object, public Composite {
        // protected:
        public:
            vector<FourPointsFace> faces;

            Mesh(vector<FourPointsFace> faces, Color color = Color(255, 255, 255),
                    IntensityColor dr = IntensityColor(.7, .7, .7), IntensityColor sr = IntensityColor(.7, .7, .7),
                    IntensityColor er = IntensityColor(.7, .7, .7), float shininess = 10);

            Vector3d get_normal_vector(Vector3d intersec_point, Intersection intersection) override;
            void print() override;

            void apply_transformation(Matrix transformation) override;
            void apply_scale_transformation(float sx, float sy, float sz) override;
            void apply_rotation_transformation(float theta, int axis) override;

            void apply_coordinate_change(Camera camera, int type_coord_change) override;
            Vector3d get_center() override;

            Intersection get_intersection(Ray ray) override;
            
    };

    class Cylinder: public Object {
    public:
        Vector3d base_center;
        Vector3d top_center;
        float radius;

        Cylinder() {}
        Cylinder(Vector3d base_center, Vector3d top_center, float radius, Color color, 
            IntensityColor dr=IntensityColor(.7, .7, .7), IntensityColor sr=IntensityColor(.7, .7, .7),
            IntensityColor er=IntensityColor(.7, .7, .7), float shininess=10);

        Vector3d get_normal_vector(Vector3d intersection_point, Intersection Intersection) override;

        Intersection get_intersection(Ray ray) override;

        Vector3d get_cylinder_dr();
        float get_height(); 
        // Vector3d get_center() override;
    };

}

#endif