#ifndef LIGHTS_H
#define LIGHTS_H

#include "Color.hpp"
#include "Vector3d.hpp"

using namespace atividades_cg_1::algebra;
using namespace atividades_cg_1::color;

namespace atividades_cg_1::lights {

    class SourceOfLight
    {
    public:
        IntensityColor intensity;
        Vector3d center;
        SourceOfLight() {}
        SourceOfLight(IntensityColor intensity, Vector3d center)
        {
            this->intensity = intensity;
            this->center = center;
        }
        SourceOfLight(Color color, Vector3d center)
        {
            this->intensity = color.to_color_normalized();
            this->center = center;
        }
    };

}
#endif