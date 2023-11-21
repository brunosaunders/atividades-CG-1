#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>

#include "Color.hpp"
#include "Vector3d.hpp"
#include "Lights.hpp"
#include "Objects.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::lights;
using namespace atividades_cg_1::objects;


namespace atividades_cg_1::scene {

    class Scene
    {
    public:
        std::vector<Object *> objects;
        Color background_color;
        SourceOfLight source_of_light;
        IntensityColor environment_light;
        Vector3d eye;

        Scene(Color bg_color, SourceOfLight source, IntensityColor environment_light, Vector3d eye)
        : background_color(bg_color), source_of_light(source), environment_light(environment_light), eye(eye) {}

        void push_object(Object *obj);

        Color get_color_to_draw(Ray ray);

        void dealloc_objects();
    };
}

#endif