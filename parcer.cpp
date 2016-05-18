#include <QFile>
#include <map>
#include <string>
#include <algorithm>

#include "picture.h"
#include "tracer.h"
#include "parcer.h"

ray_tracing::Matrix ray_tracing::Parcer::perform(size_t height, size_t width)
{
    QString section;
    while(true)
    {
        in >> section;

        if(section == "viewport")
            parce_viewport();
        else if(section == "materials")
            parce_materials();
        else if(section == "lights")
            parce_lights();
        else if(section == "geometry")
            parce_objects();
        else
        {
            screen.height = height;
            screen.width = width;
            tracer.set_screen(screen);

            return tracer.produce_picture();
        }
    }
}

ray_tracing::Parcer::Parcer(QTextStream& in_)
{
    QString str;
    while(true)
    {
        in_ >> str;
        if(str.isEmpty())
            return;

        str.remove(str.indexOf("#"), str.length() - str.indexOf("#"));
        in << str;
    }
}

ray_tracing::Point ray_tracing::Parcer::read_point()
{
    Point result;
    in >> result.x() >> result.y() >> result.z();

    return result;
}

void ray_tracing::Parcer::parce_viewport()
{
    QString tmp;
    std::array<Point, 3> points;

    while(true)
    {
        in >> tmp;

        if(tmp == "origin")
            tracer.set_view(read_point());
        else if(tmp == "topleft")
            points[0] = read_point();
        else if(tmp == "bottomleft")
            points[1] = read_point();
        else if(tmp == "topright")
            points[2] = read_point();
        else if(tmp == "endviewport")
        {
            screen = Screen(points[1], points[0], points[1] + (points[2] - points[0]), 0, 0);
            return;
        }
    }
}

void ray_tracing::Parcer::parce_materials()
{
//    QString tmp;
//    Surface<Color> surface;
//    QString name;

//    while(true)
//    {
//        in >> tmp;

//        if(tmp == "entry")
//            continue;
//        else if(tmp == "name")
//            in >> name;
//        else if(tmp == "color")
//        {
//            in >> r >> g >> b;

//            surface.color.r = double(r) / RGB_MAX;
//            surface.color.g = double(g) / RGB_MAX;
//            surface.color.b = double(b) / RGB_MAX;
//        }
//        else if(tmp == "")

//        int r, g, b;

//        in >> tmp >> name;
//        in >> tmp >> r >> g >> b;
//        in >> tmp >> surface.alpha;
//        in >> tmp >> surface.refraction;
//        //this case is forgotten to be added to the standard
//        in >> tmp >> surface.transparency;

//        surface.color.r = double(r) / RGB_MAX;
//        surface.color.g = double(g) / RGB_MAX;
//        surface.color.b = double(b) / RGB_MAX;

//        materials[name] = surface;

//        in >> tmp;
//    }
}

void ray_tracing::Parcer::parce_lights()
{
    QString tmp;
    double power, distance;
    in >> tmp;
    in >> tmp >> power;
    in >> tmp >> distance;
    in >> tmp;

    while(true)
    {
        in >> tmp;
        if(tmp != "point")
            return;
        double power_;
        Point point;
        in >> tmp >> power_;
    }
}

void ray_tracing::Parcer::parce_objects()
{
    QString tmp;
    while(true)
    {
        in >> tmp;

        if(tmp == "triangle")
            parce_polygon<3, Triangle>();
        else if(tmp == "quadrangle")
            parce_polygon<4, Quadrangle>();
        else if(tmp == "sphere")
        {
            Point center = read_point();
            double r;
            QString material;
            in >> tmp;
            in >> tmp >> r;
            in >> tmp >> material;

            tracer.add_primitive(Sphere(center, r, materials[material]));
        }
        else
            return;

        in >> tmp;
    }
}
