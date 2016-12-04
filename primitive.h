#ifndef PRIMITIVE
#define PRIMITIVE

#include <cassert>

#include "picture.h"
#include "geometry.h"

namespace ray_tracing
{

enum class Either{LEFTEST, RIGHTEST};

class Primitive
{
public:
    virtual Point intersect(const Ray& ray) const = 0;
    virtual double point(Point::Axis axis, Either either) const = 0;
    virtual double angle_cos(const Ray& ray) const = 0;
    virtual Orientation side(const Ray& ray) const = 0;
    virtual Ray reflect(const Ray& ray) const = 0;
    virtual Ray refract(const Ray& ray) const = 0;
    virtual Color get_color(const Point& point) const = 0;
    virtual double get_transparency() const = 0;
    virtual double get_alpha() const = 0;
    virtual double get_refraction() const = 0;

    virtual ~Primitive() = default;
};

template<typename C>
class Simple_surface_primitive : public virtual Primitive
{
private:
    Surface<C> surface;

public:
    Simple_surface_primitive(const Surface<C>& surface)
        : surface(surface)
    {}

    virtual double get_transparency() const override
    {
        return surface.transparency;
    }
    virtual double get_alpha() const override
    {
        return surface.alpha;
    }
    virtual double get_refraction() const override
    {
        return surface.refraction;
    }
    const Surface<C>& get_surface() const
    {
        return surface;
    }
};

class Monochrome_primitive : public Simple_surface_primitive<Color>
{
public:
    Monochrome_primitive(const Surface<Color>& surface)
        : Simple_surface_primitive(surface)
    {}

    virtual Color get_color(const Point& point) const override
    {
        return Simple_surface_primitive::get_surface().color;
    }
};

//assuming points are enumerated clockwise
template<size_t N>
class Polygon
{
private:
    Orientation orientation;
    std::array<Point, N> points;

public:
    Polygon(const std::array<Point, N>& points, Orientation orientation = Orientation::UP)
        : orientation(orientation),
          points(points)
    {}

    Point intersect(const Ray& ray) const;
    double point(Point::Axis axis, Either either) const;
    bool in(const Point& point) const;
    double angle_cos(const Ray& ray) const;
    Orientation side(const Ray& ray) const;
    Ray reflect(const Ray& ray) const;
    Ray refract(const Ray& ray, double refraction) const;

    Plane plane() const
    {
        return Plane(points[0], points[1], points[2]);
    }

    const Point& get_point(size_t i) const
    {
        return points[i];
    }
    Orientation get_orientation() const
    {
        return orientation;
    }
};

template<size_t N>
Point Polygon<N>::intersect(const Ray& ray) const
{
    Plane polygon_plane = Plane(points[0], points[1], points[2]);
    Point intersection = ray_tracing::intersect(ray, polygon_plane);

    if(intersection == Point::NOWHERE)
        return Point::NOWHERE;

    if(!in(intersection))
        return Point::NOWHERE;
    else
        return intersection;
}

template<size_t N>
double Polygon<N>::point(Point::Axis axis, Either either) const
{
    if(either == Either::LEFTEST)
        return (*std::min_element(points.begin(), points.end(), Point::comparator(axis)))[axis];
    else
        return (*std::max_element(points.begin(), points.end(), Point::comparator(axis)))[axis];
}

template<size_t N>
bool Polygon<N>::in(const Point& point) const
{
    double angle_sum = 0;
    for(size_t i = 0; i < N; ++i)
    {
        Point a = points[i] - point, b = points[(i + 1) % N] - point;
        angle_sum += angle(a, b, cross(points[1] - points[0], points[2] - points[0]));
    }

    return !eq_zero(angle_sum);
}

template<size_t N>
double Polygon<N>::angle_cos(const Ray& ray) const
{
    Plane polygon_plane = Plane(points[0], points[1], points[2]);

    return ray_tracing::angle_cos(ray, polygon_plane);
}

template<size_t N>
Orientation Polygon<N>::side(const Ray& ray) const
{
    return ray_tracing::side(ray.begin, plane());
}

template<size_t N>
Ray Polygon<N>::reflect(const Ray& ray) const
{
    return ray_tracing::reflect(ray, plane());
}

template<size_t N>
Ray Polygon<N>::refract(const Ray& ray, double refraction) const
{
    return ray_tracing::refract(ray, intersect(ray), plane().normal(), refraction);
}

class Triangle : public Monochrome_primitive, public Polygon<3ul>
{   
public:


    Triangle(const std::array<Point, 3ul>& points,
             const Surface<Color>& surface,
             Orientation orientation = Orientation::UP)
        : Monochrome_primitive(surface),
          Polygon(points, orientation)
    {}

    virtual Point intersect(const Ray& ray) const override;
    virtual double point(Point::Axis axis, Either either) const override;
    virtual double angle_cos(const Ray& ray) const override;
    virtual Orientation side(const Ray& ray) const override;
    virtual Ray reflect(const Ray& ray) const override;
    virtual Ray refract(const Ray& ray) const override;
};

class Base_quadrangle : public virtual Primitive, public Polygon<4ul>
{
public:
    Base_quadrangle(const std::array<Point, 4ul>& points,
                    Orientation orientation = Orientation::UP)
        : Polygon(points, orientation)
    {}

    virtual Point intersect(const Ray& ray) const override;
    virtual double point(Point::Axis axis, Either either) const override;
    virtual double angle_cos(const Ray& ray) const override;
    virtual Orientation side(const Ray& ray) const override;
    virtual Ray reflect(const Ray& ray) const override;
    virtual Ray refract(const Ray& ray) const override;
};

class Quadrangle : public Base_quadrangle, public Monochrome_primitive
{
public:
    Quadrangle(const std::array<Point, 4ul>& points,
               const Surface<Color>& surface,
               Orientation orientation = Orientation::UP)
        : Base_quadrangle(points, orientation),
          Monochrome_primitive(surface)
    {}
};

//assuming points[0] is right_down, points[1] is left_down, points[2] is left_up
class Base_parallelogramm : public Base_quadrangle
{   
public:
    Base_parallelogramm(const std::array<Point, 3ul>& points,
                        Orientation orientation = Orientation::UP)
        : Base_quadrangle(  std::array<Point, 4ul>{ points[0],
                                                    points[1],
                                                    points[2],
                                                    points[0] + points[2] - points[1]},
                            orientation)
    {}
};

template<typename C>
class Parallelogramm
{};

template<>
class Parallelogramm<Color> : public Base_parallelogramm, public Monochrome_primitive
{
public:
    Parallelogramm(const std::array<Point, 3ul>& points,
                   Surface<Color> surface,
                   Orientation orientation = Orientation::UP)
        : Base_parallelogramm(points, orientation),
          Monochrome_primitive(surface)
    {}
};

template<>
class Parallelogramm<Texture> : public Base_parallelogramm, public Simple_surface_primitive<Texture>
{
public:
    Parallelogramm(const std::array<Point, 3ul>& points,
                   Surface<Texture> surface,
                   Orientation orientation = Orientation::UP)
        : Base_parallelogramm(points, orientation),
          Simple_surface_primitive(surface)
    {}

    virtual Color get_color(const Point& point) const override
    {
        std::array<double, 2> decomposition = projections(  Polygon::get_point(2) - Polygon::get_point(1),
                                                            Polygon::get_point(0) - Polygon::get_point(1),
                                                            point - Polygon::get_point(1));

        Texture texture = Simple_surface_primitive::get_surface().color;
        return texture  [decomposition[0] * texture.height()]
                        [decomposition[1] * texture.width()];
    }
};

class Sphere : public Monochrome_primitive
{
private:
    Point center;
    double r;

public:
    Sphere(const Point& center, double r, const Surface<Color>& surface)
        : Monochrome_primitive(surface), center(center), r(r)
    {}

    virtual Point intersect(const Ray& ray) const override;
    virtual double point(Point::Axis axis, Either either) const override;
    virtual double angle_cos(const Ray& ray) const override;
    virtual Orientation side(const Ray& ray) const override;
    virtual Ray reflect(const Ray& ray) const override;
    virtual Ray refract(const Ray& ray) const override;

    bool in(const Point& point) const
    {
        return (center - point).mod() < r;
    }
    Point normal(const Point& point) const
    {
        return point - center;
    }
};

}

#endif // PRIMITIVE

