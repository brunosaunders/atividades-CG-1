#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>

#include "Color.hpp"
#include "Algebra.hpp"
#include "Lights.hpp"
#include "Objects.hpp"
#include "Camera.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;
using namespace atividades_cg_1::lights;
using namespace atividades_cg_1::objects;
using namespace atividades_cg_1::camera;

namespace atividades_cg_1::scene {
    const int WORLD_COORDINATES = 1;
    const int CAMERA_COORDINATES = 2;

    class Scene
    {
    public:
        Camera* camera;
        std::vector<Object *> objects;
        int coordinates_type;
        Color background_color;
        SourceOfLight source_of_light;
        IntensityColor environment_light;

        bool should_update;


        Scene(Color bg_color, SourceOfLight source, IntensityColor environment_light, Camera *camera);
       
        void push_object(Object *obj);

        Color get_color_to_draw(Ray ray);

        void dealloc_objects();

        /* Transformations: C->W (old camera) and then W->C (new camera)*/
        void set_camera(Camera* camera);
        void set_eye(Vector3d eye);
        void calculate_everything();
        Camera get_camera();
    };
}

#endif