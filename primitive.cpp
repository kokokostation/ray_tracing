#include "primitive.h"
#include "geometry.h"

ray_tracing::Point ray_tracing::Base_quadrangle::intersect(const Ray& ray) const
{
    return Polygon::intersect(ray);
}

double ray_tracing::Base_quadrangle::point(Point::Axis axis, Either either) const
{
    return Polygon::point(axis, either);
}

double ray_tracing::Base_quadrangle::angle_cos(const Ray& ray) const
{
    return Polygon::angle_cos(ray);
}

ray_tracing::Orientation ray_tracing::Base_quadrangle::side(const Ray& ray) const
{
    return Polygon::side(ray);
}

ray_tracing::Ray ray_tracing::Base_quadrangle::reflect(const Ray& ray) const
{
    return Polygon::reflect(ray);
}

ray_tracing::Ray ray_tracing::Base_quadrangle::refract(const Ray& ray) const
{
    return Polygon::refract(ray, get_refraction());
}

ray_tracing::Point ray_tracing::Triangle::intersect(const Ray& ray) const
{
    return Polygon::intersect(ray);
}

double ray_tracing::Triangle::point(Point::Axis axis, Either either) const
{
    return Polygon::point(axis, either);
}

double ray_tracing::Triangle::angle_cos(const Ray& ray) const
{
    return Polygon::angle_cos(ray);
}

ray_tracing::Orientation ray_tracing::Triangle::side(const Ray& ray) const
{
    return Polygon::side(ray);
}

ray_tracing::Ray ray_tracing::Triangle::reflect(const Ray& ray) const
{
    return Polygon::reflect(ray);
}

ray_tracing::Ray ray_tracing::Triangle::refract(const Ray& ray) const
{
    return Polygon::refract(ray, surface.refraction);
}

ray_tracing::Point ray_tracing::Sphere::intersect(const Ray& ray) const
{
    Point normal = projection(ray.guiding(), center - ray.begin) + ray.begin - center;

    if(!in(center + normal))
        return Point::NOWHERE;

    double c = sqrt(r * r - normal.mod() * normal.mod());
    std::array<Point, 2> points{center + normal + ray.guiding().normalized() * c,
                                center + normal - ray.guiding().normalized() * c};

    double t[2];
    for(int i = 0; i < 2; ++i)
        t[i] = ray.coefficient(points[i]);

    if(t[0] < t[1])
        std::swap(t[0], t[1]);

    if(t[0] == Ray::NOWHERE)
        return Point::NOWHERE;
    else
        return t[1] == Ray::NOWHERE ? ray.begin + ray.guiding() * t[0] : ray.begin + ray.guiding() * t[1];
}

double ray_tracing::Sphere::point(Point::Axis axis, Either either) const
{
    if(either == Either::LEFTEST)
        return center[axis] - r;
    else
        return center[axis] + r;
}

double ray_tracing::Sphere::angle_cos(const Ray& ray) const
{
    return fabs(ray_tracing::angle_cos(ray.guiding(), normal(intersect(ray))));
}

ray_tracing::Orientation ray_tracing::Sphere::side(const Ray& ray) const
{
    return in(ray.begin) ? Orientation::DOWN : Orientation::UP;
}

ray_tracing::Ray ray_tracing::Sphere::reflect(const Ray& ray) const
{
    Point intersection = intersect(ray);
    return ray_tracing::reflect(ray, intersection, normal(intersection));
}

ray_tracing::Ray ray_tracing::Sphere::refract(const Ray& ray) const
{
    Point intersection = intersect(ray);
    return ray_tracing::refract(ray, intersection, normal(intersection), side(ray) == Orientation::UP ? surface.refraction : 1. / surface.refraction);
}

