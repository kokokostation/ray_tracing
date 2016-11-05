#include <iostream>
#include <string>
#include <assert.h>
#include <map>

#include "parser.h"
#include "tracer.h"
#include "picture.h"
#include "template_utils.h"

void ray_tracing::assert_read(std::istream& stream, const std::string& str)
{
    std::string temp;
    stream >> temp;

    assert(temp == str);
}

ray_tracing::Scene ray_tracing::parse(std::istream& stream)
{
    Scene scene;

    std::string temp;

    while(stream >> temp)
    {
        if(temp == "viewport")
        {
            scene.set_viewport(call<Viewport>(Viewport::factory,
                                              parse<Point, Point, Point, Point, size_t, size_t>(
                                                   {"view", "left_down", "left_up", "right_down", "height", "width"},
                                                   stream)));

            assert_read(stream, "endviewport");
        }
        else if(temp == "lights")
        {
            while(true)
            {
                stream >> temp;

                if(temp == "point")
                {
                    scene.add_light(call<Light>(Light::factory,
                                                parse<Point, Light::Light_force>(
                                                     {"coords", "power"},
                                                     stream)));

                    assert_read(stream, "endpoint");
                }
                else
                {
                    assert(temp == "endlights");
                    break;
                }
            }
        }
        else if(temp == "geometry")
        {
            while(true)
            {
                std::string material;

                stream >> material;

                Surface<Color> sc;
                Surface<Texture> st;

                if(material == "texture")
                {
                    st = call<Surface<Texture>>(Surface<Texture>::factory,
                                                parse<Texture, double, double, double>(
                                                     {"file", "alpha", "transparency", "refraction"},
                                                     stream));
                }
                else if(material == "color")
                {
                    sc = call<Surface<Color>>(Surface<Color>::factory,
                                              parse<Color, double, double, double>(
                                                   {"color", "alpha", "transparency", "refraction"},
                                                   stream));
                }
                else
                {
                    assert(material == "endgeometry");
                    break;
                }

                stream >> temp;
                if(temp == "triangle")
                {
                    scene.add_primitive(Triangle(parse_array<Point, 3>(stream, "vertex"), sc));
                }
                else if(temp == "sphere")
                {
                    struct Sphere_input
                    {
                        Point center;
                        double r;

                        Sphere_input()
                        {}
                        Sphere_input(const Point& center, double r)
                            : center(center), r(r)
                        {}

                        static Sphere_input factory(const Point& center, double r)
                        {
                            return Sphere_input(center, r);
                        }
                    } si;

                    si = call<Sphere_input>(Sphere_input::factory,
                                            parse<Point, double>(
                                                 {"center", "radius"},
                                                 stream));

                    scene.add_primitive(Sphere(si.center, si.r, sc));
                }
                else if(temp == "parallelogramm")
                {
                    std::array<Point, 3> vertices = parse_array<Point, 3>(stream, "vertex");
                    if(material == "texture")
                        scene.add_primitive(Parallelogramm<Texture>(vertices, st));
                    else if(material == "color")
                        scene.add_primitive(Parallelogramm<Color>(vertices, sc));
                }
            }
        }
    }

    return scene;
}
