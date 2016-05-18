#ifndef PARCER
#define PARCER

#include <QTextStream>
#include <map>
#include <string>

#include "picture.h"
#include "tracer.h"

namespace ray_tracing
{

//parcing is performed without any error handling
class Parcer
{
private:
    const int RGB_MAX = 255;

private:
    QTextStream in;
    Tracer tracer;
    std::map<QString, Surface<Color>> materials;
    Screen screen;

    void parce_viewport();
    void parce_materials();
    void parce_lights();
    void parce_objects();

    template<size_t n, typename Polygon>
    void parce_polygon();

    Point read_point();
    template<size_t n>
    std::array<Point, n> read_points();

public:
    Parcer(QTextStream& in_);
    Matrix perform(size_t height, size_t width);
};

template<size_t n>
std::array<Point, n> ray_tracing::Parcer::read_points()
{
    QString tmp;
    std::array<Point, n> result;

    for(int i = 0; i < n; ++i)
        in >> tmp >> result[i].x() >> result[i].y() >> result[i].z();

    return result;
}

template<size_t n, typename Polygon>
void ray_tracing::Parcer::parce_polygon()
{
    QString tmp, material;
    std::array<Point, n> points = read_points<n>();

    in >> tmp >> material;

    tracer.add_primitive(Polygon(points, materials[material]));
}

}

#endif // PARCER
