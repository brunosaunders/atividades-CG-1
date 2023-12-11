#include <iostream>
#include <vector>

#include "Scene.hpp"

using namespace atividades_cg_1::scene;


Color Scene::get_color_to_draw(Ray ray)
{
    Intersection intersection_min(INFINITY, false);
    // float min_time_intersection = INFINITY;
    // int min_index = -1;
    for (int i = 0; i < objects.size(); i++)
    {
        Intersection intersection = objects[i]->get_intersection(ray);
        if (intersection.is_valid)
        {
            if (intersection.time < intersection_min.time)
            {
                intersection_min = intersection;
                // min_time_intersection = intersection.time;
                // min_index = i;
            }
        }
    }

    if (intersection_min.time == INFINITY)
        return this->background_color;

    Object *obj = intersection_min.intersepted_object;
    Color color = obj->color;

    // Pin + t*dr
    Vector3d intersection_point = ray.p1.sum(ray.get_dr().multiply(intersection_min.time));

    // Check if the object is seen by pontual light
    Ray ray_light(this->source_of_light.center, intersection_point);

    Intersection intersection_min2(INFINITY, false);
    // float min_t2 = INFINITY;
    // Object *min_obj = objects[0];

    for (auto & object : objects)
    {
        Intersection intersection2 = object->get_intersection(ray_light);
        if (intersection2.is_valid)
        {
            if (intersection2.time < intersection_min2.time)
            {
                intersection_min2 = intersection2;
                // min_t2 = intersection2.time;
                // min_obj = object;
            }
        }
    }

    Vector3d intersection_point2 = ray_light.p1.sum(ray_light.get_dr().multiply(intersection_min2.time));

    // If intersection times are not equal, there is something interrupting light to get into that point, so we discard difuse and specular contributions.
    if (intersection_min2.intersepted_object != intersection_min.intersepted_object) {
        IntensityColor environment_contrib = this->environment_light.arroba_multiply(obj->environment_reflectivity);

        return color.multiply(environment_contrib);
    }

    IntensityColor difuse_contrib = obj->get_difuse_contribution(intersection_point, intersection_min, source_of_light);
    IntensityColor specular_contrib = obj->get_specular_contribution(intersection_point, intersection_min, this->camera->eye, source_of_light);
    IntensityColor environment_contrib = this->environment_light.arroba_multiply(obj->environment_reflectivity);

    IntensityColor result = environment_contrib.sum(difuse_contrib).sum(specular_contrib);


    return color.multiply(result);
}


void Scene::dealloc_objects()
{
    for (auto &obj : objects)
    {
        delete obj;
    }
}


Scene::Scene(Color bg_color, SourceOfLight source, IntensityColor environment_light, Camera* camera)
        : background_color(bg_color), source_of_light(source), environment_light(environment_light), coordinates_type(WORLD_COORDINATES) {
    this->set_camera(camera);
}

void Scene::push_object(Object *obj)
{
    obj->apply_coordinate_change(*this->camera, CHANGE_FROM_WORLD_TO_CAMERA);
    objects.push_back(obj);
}

void Scene::set_eye(Vector3d eye) {
    this->camera->set_eye(eye);
    calculate_everything();
}

void Scene::calculate_everything() {
    camera->window.should_update = true; // Please check if we have to pass camera as reference.

    // Apply Camera matrix(W->C) to each object
    if (this->coordinates_type == WORLD_COORDINATES) {
        for (auto& obj : this->objects) {
            cout << "oba";
            obj->apply_coordinate_change(*camera, CHANGE_FROM_WORLD_TO_CAMERA);
        }
        this->camera = camera;
        this->coordinates_type = CAMERA_COORDINATES;
        return;
    }

    // Apply Old Camera matrix(C->W) to each object
    // Apply New Camera matrix(W->C) to each object
    for (auto& obj : this->objects) {
        obj->apply_coordinate_change(*this->camera, CHANGE_FROM_CAMERA_TO_WORLD); // Old camera
        obj->apply_coordinate_change(*camera, CHANGE_FROM_WORLD_TO_CAMERA); // New camera
    }
}

void Scene::set_camera(Camera* camera) {
    cout << "eae";
    camera->window.should_update = true; // Please check if we have to pass camera as reference.

    // Apply Camera matrix(W->C) to each object
    if (this->coordinates_type == WORLD_COORDINATES) {
        for (auto& obj : this->objects) {
            cout << "oba";
            obj->apply_coordinate_change(*camera, CHANGE_FROM_WORLD_TO_CAMERA);
        }
        this->camera = camera;
        this->coordinates_type = CAMERA_COORDINATES;
        return;
    }

    // Apply Old Camera matrix(C->W) to each object
    // Apply New Camera matrix(W->C) to each object
    for (auto& obj : this->objects) {
        obj->apply_coordinate_change(*this->camera, CHANGE_FROM_CAMERA_TO_WORLD); // Old camera
        obj->apply_coordinate_change(*camera, CHANGE_FROM_WORLD_TO_CAMERA); // New camera
    }

    this->camera->destroy();
    this->camera = camera;
}