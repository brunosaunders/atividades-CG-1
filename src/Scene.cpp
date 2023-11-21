#include <iostream>
#include <vector>

#include "Scene.hpp"

using namespace atividades_cg_1::scene;


void Scene::push_object(Object *obj)
{
    objects.push_back(obj);
}

Color Scene::get_color_to_draw(Ray ray)
{
    float min_time_intersection = 10000000.0;
    int min_index = -1;
    for (int i = 0; i < objects.size(); i++)
    {
        Intersection intersection = objects[i]->get_intersection(ray);
        if (intersection.is_valid)
        {
            if (intersection.time < min_time_intersection)
            {
                min_time_intersection = intersection.time;
                min_index = i;
            }
        }
    }

    if (min_index == -1)
        return this->background_color;

    Object *obj = objects[min_index];
    Color color = obj->color;

    // Pin + t*dr
    Vector3d intersection_point = ray.p1.sum(ray.get_dr().multiply(min_time_intersection));

    // Check if the object is seen by pontual light
    Ray ray_light(this->source_of_light.center, intersection_point);

    float min_t2 = 10000000.0;
    Object *min_obj = objects[0];

    for (auto & object : objects)
    {
        Intersection intersection2 = object->get_intersection(ray_light);
        if (intersection2.is_valid)
        {
            if (intersection2.time < min_t2)
            {
                min_t2 = intersection2.time;
                min_obj = object;
            }
        }
    }

    Vector3d intersection_point2 = ray_light.p1.sum(ray_light.get_dr().multiply(min_t2));

    // If intersection times are not equal, there is something interrupting light to get into that point, so we discard difuse and specular contributions.
    if (min_obj != obj) {
        IntensityColor environment_contrib = this->environment_light.arroba_multiply(obj->environment_reflectivity);

        return color.multiply(environment_contrib);
    }

    IntensityColor difuse_contrib = obj->get_difuse_contribution(intersection_point, source_of_light);
    IntensityColor specular_contrib = obj->get_specular_contribution(intersection_point, this->eye, source_of_light);
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