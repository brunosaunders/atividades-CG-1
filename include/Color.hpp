#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <cstdint>

namespace atividades_cg_1::color {
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
        int r;
        int g;
        int b;

        Color() {}
        Color(int red, int green, int blue): r(red), g(green), b(blue) {}

        IntensityColor to_color_normalized() const;
        Color multiply(IntensityColor intensity) const;

        void print() {
            std::cout << r << " " << g << " " << b << "\n";
        }
    };

}
#endif