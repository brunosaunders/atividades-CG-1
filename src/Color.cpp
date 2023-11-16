#include "Color.hpp"

using namespace std;
using namespace atividades_cg_1::color;


IntensityColor Color::to_color_normalized() const {
    return {this->r / (float)255, this->g / (float)255, this->b / (float)255};
};

Color Color::multiply(IntensityColor intensity) const {
    return {static_cast<uint8_t>(this->r * intensity.r), static_cast<uint8_t>(this->g * intensity.g), static_cast<uint8_t>(this->b * intensity.b)};
}


IntensityColor::IntensityColor() = default;
IntensityColor::IntensityColor(float r, float g, float b)
{
    if (r > 1.0) r = 1.0;
    if (g > 1.0) g = 1.0;
    if (b > 1.0) b = 1.0;

    this->r = r;
    this->g = g;
    this->b = b;
}
// Combines source intensity color with true color from object.
IntensityColor IntensityColor::arroba_multiply(IntensityColor intensity)
{
    float r = this->r * intensity.r;
    float g = this->g * intensity.g;
    float b = this->b * intensity.b;
    return {r, g, b};
}

IntensityColor IntensityColor::multiply(float value)
{
    return IntensityColor(this->r * value, this->g * value, this->b * value);
}

IntensityColor IntensityColor::sum(IntensityColor other) {
    return IntensityColor(this->r + other.r, this->g + other.g, this->b + other.b);
}
IntensityColor IntensityColor::minus(IntensityColor other) {
    return IntensityColor(this->r - other.r, this->g - other.g, this->b - other.b);
}

Color IntensityColor::to_color()
{
    return Color(this->r * 255, this->g * 255, this->b * 255);
}