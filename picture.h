#ifndef PICTURE
#define PICTURE

#include <vector>

#include "geometry.h"

namespace ray_tracing
{

struct Color
{
    static const Color BLACK;

    float r, g, b;

    Color()
        : r(0), g(0), b(0)
    {}
    Color(float r, float g, float b)
        : r(r), g(g), b(b)
    {}
    Color operator*(const Color& color) const
    {
        return Color(r * color.r, g * color.g, b  * color.b);
    }
    Color operator*(float x) const
    {
        return Color(r * x, g * x, b * x);
    }
    Color operator/(float x) const
    {
        return *this * (1.0 / x);
    }
    Color operator+(const Color& color) const
    {
        return Color(r + color.r, g + color.g, b  + color.b);
    }
    Color& operator/=(float x)
    {
        return (*this = *this / x);
    }
    Color& operator+=(const Color& color)
    {
        return (*this = *this + color);
    }
    Color operator-(const Color& color) const
    {
        return *this + (-color);
    }
    Color operator-() const
    {
        return Color(-r, -g, -b);
    }
    float mod() const
    {
        return sqrt(r * r + g * g + b * b);
    }
    bool operator==(const Color& color) const
    {
        return eq_zero((*this - color).mod());
    }
    bool operator!=(const Color& color) const
    {
        return !(*this == color);
    }
};

std::istream& operator>>(std::istream& stream, Color& c);

template<typename C>
struct Surface
{
    C color;
    double alpha, transparency, refraction;

    Surface()
    {}
    Surface(const C& color,
            double alpha,
            double transparency,
            double refraction)
        : color(color),
          alpha(alpha),
          transparency(transparency),
          refraction(refraction)
    {}

    static Surface factory(const C& color,
                           double alpha,
                           double transparency,
                           double refraction)
    {
        return Surface(color, alpha, transparency, refraction);
    }
};

struct Matrix : public std::vector<std::vector<Color>>
{
    typedef std::vector<Color> Row;

    Matrix()
    {}
    Matrix(size_t height, size_t width)
        : std::vector<std::vector<Color>>(height, Row(width))
    {}

    size_t height() const
    {
        return size();
    }
    size_t width() const
    {
        return (*this)[0].size();
    }
};

struct Viewport
{
    Point view, left_down, left_up, right_down;
    size_t width, height;

    Viewport()
    {}
    Viewport(const Point& view,
             const Point& left_down,
             const Point& left_up,
             const Point& right_down,
             size_t height, size_t width)
        : view(view),
          left_down(left_down),
          left_up(left_up),
          right_down(right_down),
          width(width), height(height)
    {}

    static Viewport factory(const Point& view,
                            const Point& left_down,
                            const Point& left_up,
                            const Point& right_down,
                            size_t height, size_t width)
    {
        return Viewport(view, left_down, left_up, right_down, height, width);
    }
};

typedef Matrix Texture;

std::istream& operator>>(std::istream& stream, Texture& texture);

}

#endif // PICTURE

