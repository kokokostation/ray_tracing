#ifndef LIGHT
#define LIGHT

#include "geometry.h"

namespace ray_tracing
{

struct Light
{
    typedef double Light_force;

    constexpr static Light_force DARKNESS = 0.1;
    constexpr static double LAMBERT_WEIGHT = 0.7;
    constexpr static double FONG_WEIGHT = 1 - LAMBERT_WEIGHT;

    Point place;
    Light_force force;

    Light(const Point& place, Light_force force)
        : place(place), force(force)
    {}

    static Light factory(const Point& place, Light_force force)
    {
        return Light(place, force);
    }

    Light_force calculate(double angle_cos_lambert, double angle_cos_fong, const Point& point) const
    {
        return force *
               (angle_cos_lambert * LAMBERT_WEIGHT +
               (angle_cos_fong < 0 ? 0 : angle_cos_fong) * FONG_WEIGHT) /
               pow((place - point).mod(), 2.0);
    }
};

}

#endif // LIGHT

