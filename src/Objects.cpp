
#include "Objects.hpp"
#include "Algebra.hpp"
#include "Color.hpp"
#include "Camera.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::objects;
using namespace atividades_cg_1::camera;

IntensityColor Object::get_difuse_contribution(Vector3d intersec_point, Intersection intersection, SourceOfLight source_of_light)
{
    Vector3d l = this->get_light_vector(intersec_point, intersection, source_of_light);
    Vector3d n = this->get_normal_vector(intersec_point, intersection);
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

IntensityColor Object::get_specular_contribution(Vector3d intersec_point, Intersection intersection, Vector3d eye_point, SourceOfLight source_of_light)
{
    Vector3d l = this->get_light_vector(intersec_point, intersection, source_of_light);
    Vector3d n = this->get_normal_vector(intersec_point, intersection);

    Vector3d r = n.multiply(2).minus(l).get_vector_normalized();
    Vector3d v = eye_point.minus(intersec_point).get_vector_normalized();

    float f_specular = std::pow(r.scalar_product(v), this->shininess);

    IntensityColor contribution = source_of_light.intensity.arroba_multiply(this->specular_reflectivity).multiply(f_specular);
    return contribution;
}

Vector3d Object::get_light_vector(Vector3d intersec_point, Intersection intersection, SourceOfLight source)
{
    return (source.center.minus(intersec_point)).get_vector_normalized();
}

// Sphere
// n unitary vector (normal vector).
Vector3d Sphere::get_normal_vector(Vector3d intersec_point, Intersection intersection)
{
    return (intersec_point.minus(this->center)).divide(this->radius);
}

void Sphere::apply_transformation(Matrix transformation)
{
    this->center = this->center.apply_transformation(transformation);
}

void Sphere::apply_scale_transformation(float sx, float sy, float sz)
{
    // We need to grow sphere's radius.
    float s = min(sx, sy);
    s = min(s, sz);
    this->radius *= s;
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
        return Intersection(t1, true, this);
    return Intersection(t2, true, this);
}

void Sphere::apply_coordinate_change(Camera camera, int type_coord_change)
{
    switch (type_coord_change)
    {
    case CHANGE_FROM_WORLD_TO_CAMERA:
        cout << "Esfera: ";
        this->center.print();
        this->center = camera.transform_vector_from_world_to_camera(this->center);
        cout << "Esfera depois: ";
        this->center.print();
        break;

    case CHANGE_FROM_CAMERA_TO_WORLD:
        this->center = camera.transform_vector_from_camera_to_world(this->center);
        break;
    default:
        throw runtime_error("Tipo de mudança de coordenada inválida");
        break;
    }
}

Vector3d Plan::get_normal_vector(Vector3d intersec_point, Intersection intersection)
{
    return this->normal.get_vector_normalized();
}

void Plan::apply_transformation(Matrix transformation)
{
    this->known_point = this->known_point.apply_transformation(transformation);
    this->normal = this->normal.apply_transformation(transformation);
}

Intersection Plan::get_intersection(Ray ray)
{
    Vector3d w = ray.p1.minus(this->known_point);
    Vector3d dr = ray.get_dr();

    // float t_int = -(this->get_normal_vector(Vector3d(), Intersection()).scalar_product(w)) / (this->get_normal_vector(Vector3d(), Intersection()).scalar_product(dr));
    float t_int = -(this->normal).scalar_product(w) / (this->normal).scalar_product(dr);
    if (t_int > 0) {
        return Intersection(t_int, true, this);
    }
    return Intersection(t_int, false);
}

void Plan::apply_coordinate_change(Camera camera, int type_coord_change)
{
    switch (type_coord_change)
    {
    case CHANGE_FROM_WORLD_TO_CAMERA:
        this->known_point = camera.transform_vector_from_world_to_camera(this->known_point);
        this->normal = camera.transform_vector_from_world_to_camera(this->normal);
        break;

    case CHANGE_FROM_CAMERA_TO_WORLD:
        this->known_point = camera.transform_vector_from_camera_to_world(this->known_point);
        this->normal = camera.transform_vector_from_camera_to_world(this->normal);
        break;
    default:
        throw runtime_error("Tipo de mudança de coordenada inválida");
        break;
    }
}

Triangle::Triangle(Vector3d p1, Vector3d p2,
                   Vector3d p3, Color color,
                   IntensityColor dr, IntensityColor sr,
                   IntensityColor er, float shininess) : p1(p1), p2(p2), p3(p3), Object(color, dr, sr, er, shininess) {}

// We can pass any value of interserction_point
Vector3d Triangle::get_normal_vector(Vector3d intersec_point = Vector3d(), Intersection intersection = Intersection())
{
    Vector3d r1 = this->p2.minus(this->p1);
    Vector3d r2 = this->p3.minus(this->p1);

    Vector3d N = r1.vectorial_product(r2); // r1 x r2
    return N.get_vector_normalized();
}

Vector3d Triangle::get_center() {
    return this->p1.sum(this->p2).sum(this->p3).divide(3);
}

void Triangle::apply_transformation(Matrix transformation)
{

    this->p1 = this->p1.apply_transformation(transformation);
    this->p2 = this->p2.apply_transformation(transformation);
    this->p3 = this->p3.apply_transformation(transformation);
}

void Triangle::apply_scale_transformation(float sx, float sy, float sz)
{
    Vector3d fixed_point = this->get_center();
    Matrix matrix_transformation = MatrixTransformations::scale(fixed_point, sx, sy, sz);
    Triangle::apply_transformation(matrix_transformation);
}

void Triangle::apply_rotation_transformation(float theta, int axis)
{
    Matrix rotation_matrix = MatrixTransformations::rotation(theta, axis);
    Triangle::apply_transformation(rotation_matrix);
}

Intersection Triangle::get_intersection(Ray ray)
{
    Vector3d normal_vector = this->get_normal_vector();
    float intersec_t = -(((ray.p1.minus(this->p1)).scalar_product(normal_vector)) / ray.get_dr().scalar_product(normal_vector));

    if (intersec_t <= 0)
    {
        return Intersection(intersec_t, false);
    }

    Vector3d intersec_point = ray.p1.sum(ray.get_dr().multiply(intersec_t));

    Vector3d r1 = this->p2.minus(this->p1);
    Vector3d r2 = this->p3.minus(this->p1);
    Vector3d v = intersec_point.minus(this->p1);

    float total_area = r1.vectorial_product(r2).scalar_product(normal_vector);

    float c1 = v.vectorial_product(r2).scalar_product(normal_vector) / total_area;
    float c2 = r1.vectorial_product(v).scalar_product(normal_vector) / total_area;
    float c3 = 1.0 - c1 - c2;

    if (c1 >= 0.0 && c2 >= 0.0 && c3 >= 0.0 && abs(c1 + c2 + c3 - 1.0) <= 1.0e-12)
    {
        return Intersection(intersec_t, true, this);
    }

    return Intersection(intersec_t, false);
}

void Triangle::apply_coordinate_change(Camera camera, int type_coord_change)
{
    switch (type_coord_change)
    {
    case CHANGE_FROM_WORLD_TO_CAMERA:
        this->p1 = camera.transform_vector_from_world_to_camera(this->p1);
        this->p2 = camera.transform_vector_from_world_to_camera(this->p2);
        this->p3 = camera.transform_vector_from_world_to_camera(this->p3);
        break;

    case CHANGE_FROM_CAMERA_TO_WORLD:
        this->p1 = camera.transform_vector_from_camera_to_world(this->p1);
        this->p2 = camera.transform_vector_from_camera_to_world(this->p2);
        this->p3 = camera.transform_vector_from_camera_to_world(this->p3);
        break;
    default:
        throw runtime_error("Tipo de mudança de coordenada inválida");
        break;
    }
}


Triangle FourPointsFace::get_t1()
{
    return this->t1;
}

Triangle FourPointsFace::get_t2()
{
    return this->t2;
}

Vector3d Triangle::get_p1()
{
    return this->p1;
}

Vector3d Triangle::get_p2()
{
    return this->p2;
}

Vector3d Triangle::get_p3()
{
    return this->p3;
}


FourPointsFace::FourPointsFace(Vector3d p1, Vector3d p2,
                               Vector3d p3, Vector3d p4, Color color,
                               IntensityColor dr, IntensityColor sr,
                               IntensityColor er, float shininess) : Object(color, dr, sr, er, shininess)
{
    this->t1 = Triangle(p1, p2, p3);
    this->t2 = Triangle(p3, p4, p1);
}

Vector3d FourPointsFace::get_center() {
    return this->t1.get_center().sum(this->t2.get_center()).divide(2);
}

Vector3d FourPointsFace::get_normal_vector(Vector3d intersec_point, Intersection intersection)
{
    return this->t1.get_normal_vector();
}

void FourPointsFace::apply_transformation(Matrix transformation)
{
    this->t1.apply_transformation(transformation);
    this->t2.apply_transformation(transformation);
}

void FourPointsFace::apply_scale_transformation(float sx, float sy, float sz)
{
    Vector3d fixed_point = this->get_center();
    Matrix matrix_transformation = MatrixTransformations::scale(fixed_point, sx, sy, sz);
    this->t1.apply_transformation(matrix_transformation);
    this->t2.apply_transformation(matrix_transformation);
}

void FourPointsFace::apply_rotation_transformation(float theta, int axis)
{
    Matrix rotation_matrix = MatrixTransformations::rotation(theta, axis);
    this->t1.apply_transformation(rotation_matrix);
    this->t2.apply_transformation(rotation_matrix);
}

Intersection FourPointsFace::get_intersection(Ray ray)
{
    Intersection intersec1 = this->t1.get_intersection(ray);
    if (intersec1.is_valid)
    {
        this->color = this->t1.color;
        this->shininess = this->t1.shininess;
        this->difuse_reflectivity = this->t1.difuse_reflectivity;
        this->environment_reflectivity = this->t1.environment_reflectivity;
        this->specular_reflectivity = this->t1.specular_reflectivity;
        return intersec1;
        
    } 

    Intersection intersec2 = this->t2.get_intersection(ray);
    if (intersec2.is_valid)
    {
        this->color = this->t2.color;
        this->shininess = this->t2.shininess;
        this->difuse_reflectivity = this->t2.difuse_reflectivity;
        this->environment_reflectivity = this->t2.environment_reflectivity;
        this->specular_reflectivity = this->t2.specular_reflectivity;
    }
    
    return intersec2;
}

void FourPointsFace::apply_coordinate_change(Camera camera, int type_coord_change)
{
    this->t1.apply_coordinate_change(camera, type_coord_change);
    this->t2.apply_coordinate_change(camera, type_coord_change);
}

void FourPointsFace::print() {
    this->get_center().print();
}

Mesh::Mesh(vector<FourPointsFace> faces, Color color,
           IntensityColor dr, IntensityColor sr,
           IntensityColor er, float shininess) : faces(faces), Object(color, dr, sr, er, shininess)
{}

Vector3d Mesh::get_center() {
    int count = 0;
    Vector3d v(0, 0, 0);

    for (auto &face : this->faces)
    {
        v = v.sum(face.get_center());
        count++;
    }

    if (count == 0)
    {
        throw runtime_error("Malha inválida (não possui faces).");
    }

    return v.divide(count);
}

void Mesh::apply_transformation(Matrix transformation)
{
    for (auto &face : this->faces)
    {
        face.apply_transformation(transformation);
    }
}

void Mesh::apply_coordinate_change(Camera camera, int type_coord_change)
{
    for (auto &face : this->faces)
    {
        face.apply_coordinate_change(camera, type_coord_change);
    }
}

void Mesh::apply_scale_transformation(float sx, float sy, float sz)
{
    Vector3d fixed_point = this->get_center();
    Matrix matrix_transformation = MatrixTransformations::scale(fixed_point, sx, sy, sz);

    for (auto& face : this->faces) {
        face.apply_transformation(matrix_transformation);
    }
}

void Mesh::apply_rotation_transformation(float theta, int axis)
{
    Matrix rotation_matrix = MatrixTransformations::rotation(theta, axis);
    for (auto &face : this->faces)
    {
        face.apply_transformation(rotation_matrix);
    }
}

void Mesh::print() {
    cout << "Face centers\n";
    for (auto& item : this->faces) {
        item.get_center().print();
    }
    cout << endl;
}

Vector3d Mesh::get_normal_vector(Vector3d intersec_point, Intersection intersection) {
    return intersection.intersepted_object->get_normal_vector(intersec_point, intersection);
}

Intersection Mesh::get_intersection(Ray ray) {

    Intersection intersection_min(INFINITY, false);

    for (auto& face : this->faces) {
        Intersection intersection = face.get_intersection(ray);
        if (intersection.is_valid && intersection.time < intersection_min.time) {
            intersection_min = intersection;
        }
    }

    if (intersection_min.time != INFINITY) {
        this->color = intersection_min.intersepted_object->color;
        this->shininess = intersection_min.intersepted_object->shininess;
        this->difuse_reflectivity = intersection_min.intersepted_object->difuse_reflectivity;
        this->environment_reflectivity = intersection_min.intersepted_object->environment_reflectivity;
        this->specular_reflectivity = intersection_min.intersepted_object->specular_reflectivity;
    }
    
    return intersection_min;
}

Cylinder::Cylinder(Vector3d base_center, Vector3d top_center, float radius, Color color, 
            IntensityColor dr, IntensityColor sr,
            IntensityColor er, float shininess)
                : top_center(top_center), radius(radius), Object(color, difuse_reflectivity, specular_reflectivity,
                environment_reflectivity, shininess), base_center(base_center){
                    this->color = Color(255,255,255);
                    this->color.print();
                }

Vector3d Cylinder::get_cylinder_dr() {
    return this->top_center.minus(this->base_center).get_vector_normalized();
}

float Cylinder::get_height() {
    return this->top_center.minus(this->base_center).size();
}

// Vector3d Cylinder::get_center() {
//     return this->top_center.sum(this->base_center).divide(2);
// }


Vector3d Cylinder::get_normal_vector(Vector3d intersection_point, Intersection intersection) {
    // return Vector3d(0, 15, -85);
    // Vector3d v, p_v, h;
    float v_escalar_dc, erro = 1e-12;

    Vector3d v = intersection_point.minus(this->base_center);
    v_escalar_dc = v.scalar_product(this->get_cylinder_dr());
    Vector3d p_v = this->get_cylinder_dr().multiply(v_escalar_dc);
    Vector3d h = v.minus(p_v);

    if (0.0 < v_escalar_dc && v_escalar_dc < this->get_height()) {
        return h.get_vector_normalized();

    } else if (v_escalar_dc <= 0.0) {

        return this->get_cylinder_dr().multiply(-1.0);

    } else {
        return this->get_cylinder_dr();
    }
}

Intersection Cylinder::get_intersection(Ray ray) {

    // w = P0 - centro_base
    Vector3d dr = ray.get_dr();
    // Vector3d d = ray.get_dr();

    Vector3d u = this->get_cylinder_dr();
    Vector3d s = ray.p1.minus(this->base_center);
    // Vector3d v = (ray.p1.minus(this->base_center)).minus(u.multiply((ray.p1.minus(this->base_center)).scalar_product(u)));
    Vector3d v = s.minus(u.multiply(s.scalar_product(u)));
    Vector3d w = dr.minus(u.multiply(dr.scalar_product(u)));
    // Vector3d w = ray.p1.minus(this->base_center);

    float cylinder_height = this->get_height();


    float a = w.scalar_product(w);
    float b = (v.scalar_product(w)) * 2;
    float c = v.scalar_product(v) - pow(this->radius, 2);
    
    float delta = pow(b, 2) - 4*a*c;

    if (delta < 0) {
        return Intersection(-1, false, this);
        // caso em que o raio pode passar pela base.
    } else if (delta == 0) {
        if (a == 0) {
            return Intersection(-1, false, this); // é pra ser true
        }

        float t_int = -b / (2*a);
        return Intersection(t_int, true, this);
    } else {
        float t1 = (-b + sqrt(delta)) / (2*a);
        float t2 = (-b - sqrt(delta)) / (2*a);

        Vector3d P1 = ray.p1.sum(dr.multiply(t1));
        Vector3d P2 = ray.p1.sum(dr.multiply(t2));

        Vector3d W1 = P1.minus(this->base_center);
        Vector3d W2 = P2.minus(this->base_center);

        // Check if it is valid
        Vector3d dc = this->get_cylinder_dr();
        
        float t1_scalar_product_check = W1.scalar_product(dc);
        float t2_scalar_product_check = W2.scalar_product(dc);

        float height = this->get_height();

        bool t1_valid = t1_scalar_product_check >= 0 && t1_scalar_product_check <= height;
        bool t2_valid = t2_scalar_product_check >= 0 && t2_scalar_product_check <= height;
        Intersection intersection_min(INFINITY, false);

        if (t1_valid) {
            intersection_min = Intersection(t1, true, this);
            // cout << "t1 ";
        }

        if (t2_valid && t2 < intersection_min.time) {
            intersection_min = Intersection(t2, true, this);
            // cout << "t2 ";
        }
        return intersection_min;
    }

    // return Intersection(t_int, std::abs(t_int + 1.0) > 1.0e-12, this);
}