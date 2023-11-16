#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <cstdint>
class Color;

class IntensityColor
{
public:
    float r;
    float g;
    float b;

    IntensityColor();
    IntensityColor(float r, float g, float b);

    IntensityColor arroba_multiply(IntensityColor intensity);
    IntensityColor multiply(float value);
    IntensityColor sum(IntensityColor other);
    IntensityColor minus(IntensityColor other);
    Color to_color();
};
class Color
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Color();
    Color(uint8_t red, uint8_t green, uint8_t blue);

    IntensityColor to_color_normalized() const;
    Color multiply(IntensityColor intensity) const;
};

#endif;