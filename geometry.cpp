#include <cmath>
#include <limits>

#include "geometry.h"
#include "tracer.h"

const ray_tracing::Point ray_tracing::Point::NOWHERE = Point(std::numeric_limits<double>::max(),
                                                             std::numeric_limits<double>::max(),
                                                             std::numeric_limits<double>::max());

const ray_tracing::Point ray_tracing::Point::MAX = Point(   std::numeric_limits<double>::max(),
                                                            std::numeric_limits<double>::max(),
                                                            std::numeric_limits<double>::max());

const ray_tracing::Point ray_tracing::Point::MIN = Point(   std::numeric_limits<double>::min(),
                                                            std::numeric_limits<double>::min(),
                                                            std::numeric_limits<double>::min());

ray_tracing::Ray ray_tracing::Ray::NOWHERE_RAY = Ray(ray_tracing::Point::NOWHERE, ray_tracing::Point::NOWHERE);

double ray_tracing::dot(const Point& a, const Point& b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

ray_tracing::Point ray_tracing::cross(const Point& a, const Point& b)
{
    return Point(a.y() * b.z() - a.z() * b.y(),
                 a.z() * b.x() - a.x() * b.z(),
                 a.x() * b.y() - a.y() * b.x());
}

double ray_tracing::determinant(const Point& a, const Point& b, const Point& c)
{
    return a.x() * b.y() * c.z() + a.y() * b.z() * c.x() + a.z() * b.x() * c.y() -
            (a.z() * b.y() * c.x() + a.y() * b.x() * c.z() + a.x() * b.z() * c.y());
}

bool ray_tracing::eq_zero(double x)
{
    return fabs(x) < EPS;
}

int ray_tracing::signum(double x)
{
    if(eq_zero(x))
        return 0;
    else if(x < 0)
        return -1;
    else
        return 1;
}

ray_tracing::Point ray_tracing::intersect(const Ray& ray, const Plane& plane)
{
    double t;
    double det = determinant(plane.b - plane.a, plane.c - plane.a, ray.begin - ray.second);

    if(eq_zero(det))
        t = -1;
    else
        t = determinant(plane.b - plane.a, plane.c - plane.a, ray.begin - plane.a) / det;

    if(t <= EPS)
        return Point::NOWHERE;
    else
        return ray.begin + ray.guiding() * t;
}

double ray_tracing::angle(const Point& a, const Point& b, const Point& normal)
{
    int sign = eq_zero((normal.normalized() - cross(a, b).normalized()).mod()) ? 1 : -1;
    return sign * atan2(cross(a, b).mod(), dot(a, b));
}

double ray_tracing::projection_coefficient(const Point& a, const Point& b)
{
    return dot(a, b) / pow(a.mod(), 2.0);
}

ray_tracing::Point ray_tracing::projection(const Point& a, const Point& b)
{
    return a * projection_coefficient(a, b);
}

double ray_tracing::Ray::coefficient(const Point& point) const
{
    if(point == Point::NOWHERE)
        return Ray::NOWHERE;

    double coefficient = projection_coefficient(guiding(), point - begin);

    if(coefficient <= EPS)
        return ray_tracing::Ray::NOWHERE;

    return coefficient;
}

double ray_tracing::angle_cos(const Point& a, const Point& b)
{
    return dot(a.normalized(), b.normalized());
}

double ray_tracing::angle_cos(const Ray& ray, const Plane& plane)
{
    return fabs(angle_cos(plane.normal(), ray.guiding()));
}

std::array<ray_tracing::Box, 2> ray_tracing::Box::split(Point::Axis axis, double splitting_plane) const
{
    std::array<Box, 2> result{*this, *this};

    result[0].ru[axis] = splitting_plane;
    result[1].ld[axis] = splitting_plane;

    return result;
}

bool ray_tracing::Box::contains(const Point& point) const
{
    for(int i = 0; i < Point::AXIS_SIZE; ++i)
        if(point[i] < ld[i] - EPS || point[i] > ru[i] + EPS)
            return false;

    return true;
}

//intersection coefficient for ray and quadrangle, perpendicular to an axis
double intersection_coefficient(const ray_tracing::Ray& ray, ray_tracing::Point::Axis axis, double axis_coordinate,
                                                    const ray_tracing::Point& ld, const ray_tracing::Point& ru)
{
    if(ray_tracing::eq_zero(ray.guiding()[axis]))
        return ray_tracing::Ray::NOWHERE;

    double coefficient = (axis_coordinate - ray.begin[axis]) / ray.guiding()[axis];

    if(coefficient <= ray_tracing::EPS)
        return ray_tracing::Ray::NOWHERE;

    ray_tracing::Point intersection = ray.begin + ray.guiding() * coefficient;

    for(size_t i = 0; i < ray_tracing::Point::AXIS_SIZE; ++i)
        if(i != axis && (intersection[i] < ld[i] || intersection[i] > ru[i]))
            return ray_tracing::Ray::NOWHERE;

    return coefficient;
}

double ray_tracing::intersect(const Ray& ray, const Box& box)
{
    if(box.contains(ray.begin))
        return 0;

    double min_coefficient = ray_tracing::Ray::NOWHERE;

    for(size_t i = 0; i < Point::AXIS_SIZE; ++i)
        for(double axis_coordinate : {box.ld[i], box.ru[i]})
        {
            double coefficient = intersection_coefficient(ray, Point::Axis(i), axis_coordinate, box.ld, box.ru);
            if(coefficient != ray_tracing::Ray::NOWHERE && (min_coefficient == ray_tracing::Ray::NOWHERE || coefficient < min_coefficient))
                min_coefficient = coefficient;
        }

    return min_coefficient;
}

ray_tracing::Ray ray_tracing::reflect(const Ray& ray, const Point& intersection, const Point& normal)
{
    Point   guiding = intersection - ray.begin,
            middle = projection(normal, guiding);

    return Ray(intersection, (guiding - middle) * 2 + ray.begin);
}

ray_tracing::Orientation ray_tracing::side(const Point& point, const Plane& plane)
{
    return determinant( plane.c - plane.a,
                        plane.b - plane.a,
                        point - plane.a) < 0 ? Orientation::DOWN : Orientation::UP;
}

ray_tracing::Ray ray_tracing::reflect(const Ray& ray, const Plane& plane)
{
    return reflect(ray, intersect(ray, plane), plane.normal());
}

ray_tracing::Ray ray_tracing::refract(const Ray& ray, const Point& point, Point normal, double refraction)
{
    if(eq_zero(refraction) || eq_zero(cross(ray.guiding(), normal).mod()))
        return Ray(point, point + normal * (dot(normal, ray.guiding()) < 0 ? -1 : 1));

    Point   av = projection(normal, ray.guiding()),
            bv = ray.guiding() - av;

    double  a = av.mod(),
            b = bv.mod(),
            c = ray.guiding().mod(),
            f = c / (refraction * b);

    if(f <= 1)
        return Ray(point, point + ray.guiding() - projection(ray.guiding(), normal));

    return Ray(point, point + av + bv.normalized() * a / sqrt(f * f - 1));
}

std::array<double, 2> ray_tracing::projections(const Point& a, const Point& b, const Point& v)
{
    std::array<double, 2> result;

    Point c = cross(a, b);

    double det = determinant(a, b, c);

    result[0] = determinant(v, b, c) / det;
    result[1] = determinant(a, v, c) / det;

    return result;
}
