#ifndef GEOMETRY
#define GEOMETRY

#include <limits>
#include <cstddef>
#include <array>
#include <cmath>
#include <algorithm>

namespace ray_tracing
{

enum class Orientation {UP, DOWN};

const double EPS = 1e-4;

bool eq_zero(double x);
int signum(double x);

struct Point
{
    static const Point NOWHERE;
    static const Point MAX;
    static const Point MIN;
    enum Axis {X, Y, Z};
    constexpr static int AXIS_SIZE = 3;

    static auto comparator(Axis axis)
    {
        return  [axis](const Point& point1, const Point& point2)
                {
                    return point1[axis] < point2[axis];
                };
    }

    std::array<double, AXIS_SIZE> coordinates;

    Point(double x, double y, double z) : coordinates{x, y, z} {}
    Point(const Point& point) : coordinates(point.coordinates) {}
    Point() {}

    double& operator[](int axis) {return const_cast<double&>(static_cast<const Point*>(this)->operator[](axis));}
    double& x() {return const_cast<double&>(static_cast<const Point*>(this)->x());}
    double& y() {return const_cast<double&>(static_cast<const Point*>(this)->y());}
    double& z() {return const_cast<double&>(static_cast<const Point*>(this)->z());}
    const double& x() const {return coordinates[X];}
    const double& y() const {return coordinates[Y];}
    const double& z() const {return coordinates[Z];}
    const double& operator[](int axis) const {return coordinates[axis];}

    double mod() const {return sqrt(x() * x() + y() * y() + z() * z());}
    Point normalized() const {return *this / mod();}

    Point operator-(const Point& b) const {return *this + (-b);}
    Point operator-() const {return Point(-x(), -y(), -z());}
    Point operator+(const Point& b) const {return Point(x() + b.x(), y() + b.y(), z() + b.z());}
    Point operator/(double b) const {return *this * (1.0 / b);}
    Point operator*(double b) const {return Point(x() * b, y() * b, z() * b);}
    bool operator==(const Point& b) const {return eq_zero((*this - b).mod());}
    bool operator!=(const Point& b) const {return !(*this == b);}
};

double dot(const Point& a, const Point& b);
Point cross(const Point& a, const Point& b);
double determinant(const Point& a, const Point& b, const Point& c);
double projection_coefficient(const Point& a, const Point& b);
double angle(const Point& a, const Point& b, const Point& normal);

//begin - vertex, (begin, second) - guiding line
struct Ray
{
    constexpr static double NOWHERE = -1;
    static Ray NOWHERE_RAY;

    Point begin, second;

    Ray() {}
    Ray(const Point& begin, const Point& second) : begin(begin), second(second) {}
    //takes Point point on the ray and returns c: begin + (second - begin) * c == second
    double coefficient(const Point& point) const;
    Point guiding() const {return second - begin;}
    Ray& correct() {begin = begin + guiding().normalized() * EPS; return *this;}
};

struct Segment
{
    Point begin, end;

    Segment(const Point& begin, const Point& end) : begin(begin), end(end) {}
};

struct Plane
{
    Point a, b, c;

    Plane(const Point& a, const Point& b, const Point& c) : a(a), b(b), c(c) {}
    Point normal() const {return cross(c - a, b - a);}
};


Point intersect(const Ray& ray, const Plane& plane);
Ray reflect(const Ray& ray, const Point& intersection, const Point& perpendicular);
Ray reflect(const Ray& ray, const Plane& plane);
Orientation side(const Point& point, const Plane& plane);
double angle_cos(const Point& a, const Point& b);
double angle_cos(const Ray& ray, const Plane& plane);

struct Box
{
    Point ld, ru;

    Box() {}
    Box(const Point& ld, const Point& ru) : ld(ld), ru(ru) {}

    double surface_area() {return 2 * ( (ru.x() - ld.x()) * (ru.y() - ld.y()) +
                                        (ru.x() - ld.x()) * (ru.z() - ld.z()) +
                                        (ru.y() - ld.y()) * (ru.z() - ld.z()));}

    std::array<Box, 2> split(Point::Axis axis, double splitting_plane) const;
    bool contains(const Point& point) const;
};

double intersect(const Ray& ray, const Box& box);
Ray refract(const Ray& ray, const Point& point, Point normal, double refraction);
Point projection(const Point& a, const Point& b);

}

#endif // GEOMETRY

