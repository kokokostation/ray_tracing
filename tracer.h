#ifndef TRACER_H
#define TRACER_H

#include <vector>
#include <cstddef>
#include <memory>

#include "picture.h"
#include "primitive.h"
#include "geometry.h"
#include "light.h"
#include "kd_tree.h"

namespace ray_tracing
{

typedef std::vector<std::vector<char>> Determinant_matrix;

//tracing is performed in assumption that all the primitves are on the opposite side of the screen relatively to observer
class Tracer
{
private:
    constexpr static size_t TRACE_DEPTH = 10;
    constexpr static size_t RAYS_PER_SECOND = 30000;
    constexpr static double ANTI_ALIASING_BOUND = 0.05;

private:
    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<Light> lights;
    Point view;
    Screen screen;

    void add_primitive(const std::shared_ptr<Primitive>& shared_ptr) {primitives.push_back(shared_ptr);}
    Color trace(const Ray& ray, const Kd_tree& tree, size_t depth) const;
    Light_force light_force(const std::shared_ptr<Primitive>& primitive, const Ray& ray, const Kd_tree& tree) const;
    Ray produce_ray(double i, double j) const
    {
        return Ray(view,  screen.left_down + (screen.left_up - screen.left_down) * i / screen.height +
                                             (screen.right_down - screen.left_down) * j / screen.width);
    }
    void produce_picture_helper(size_t from, size_t to, Matrix& matrix, const Kd_tree& tree) const;
    void anti_aliasing_determinant(size_t from, size_t to, const Matrix& matrix, Determinant_matrix& dots) const;
    void anti_aliasing_performer(size_t from, size_t to, Matrix& matrix, const Kd_tree& tree, const Determinant_matrix& dots) const;
    template<typename F, typename... Args>
    void parallel_perform(const Matrix& matrix, F function, Args&... args) const;

public:
    Tracer(const Point& view, const Screen& screen)
        : view(view), screen(screen){}

    void add_primitive(const Triangle& triangle) {add_primitive(std::make_shared<Triangle>(triangle));}
    void add_primitive(const Quadrangle& quadrangle) {add_primitive(std::make_shared<Quadrangle>(quadrangle));}
    template<typename T>
    void add_primitive(const Parallelogramm<T>& parallelogramm) {add_primitive(std::make_shared<Parallelogramm<T>>(parallelogramm));}
    void add_primitive(const Sphere& sphere) {add_primitive(std::make_shared<Sphere>(sphere));}

    void add_light(const Light& light) {lights.push_back(light);}

    Matrix produce_picture() const;
};

}

#endif // TRACER_H

