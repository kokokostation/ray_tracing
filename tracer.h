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
#include "continuous_performer.h"

namespace ray_tracing
{

class Tracer;

class Scene
{
    friend class Tracer;

private:
    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<Light> lights;
    Viewport viewport;

    void add_primitive(const std::shared_ptr<Primitive>& shared_ptr)
    {
        primitives.push_back(shared_ptr);
    }

public:
    void add_primitive(const Triangle& triangle)
    {
        add_primitive(std::make_shared<Triangle>(triangle));
    }
    void add_primitive(const Quadrangle& quadrangle)
    {
        add_primitive(std::make_shared<Quadrangle>(quadrangle));
    }
    template<typename T>
    void add_primitive(const Parallelogramm<T>& parallelogramm)
    {
        add_primitive(std::make_shared<Parallelogramm<T>>(parallelogramm));
    }
    void add_primitive(const Sphere& sphere)
    {
        add_primitive(std::make_shared<Sphere>(sphere));
    }
    void add_light(const Light& light)
    {
        lights.push_back(light);
    }
    void set_viewport(const Viewport& viewport_)
    {
        viewport = viewport_;
    }
};

//tracing is performed in assumption that all the primitves are on the opposite
//side of the screen relatively to the observer
class Tracer
{
private:
    static const size_t TRACE_DEPTH = 10;
    static const size_t RAYS_PER_SECOND = 30000;
    constexpr static const double ANTI_ALIASING_BOUND = 0.05;

private:
    Kd_tree tree;
    Matrix matrix;
    std::vector<std::vector<char>> determinant_matrix;
    Scene scene;
    Continuous_performer performer;

    Color trace(const Ray& ray, size_t depth) const;
    Light::Light_force light_force(const std::shared_ptr<Primitive>& primitive, const Ray& ray) const;
    Ray produce_ray(double i, double j) const;
    void produce_picture_helper(size_t from, size_t to);
    void anti_aliasing_determinant(size_t from, size_t to);
    void anti_aliasing_performer(size_t from, size_t to);
    template<typename F>
    void parallel_perform(F function);

public:
    Tracer(Scene&& scene)
        : tree(scene.primitives),
          matrix(scene.viewport.height, scene.viewport.width),
          determinant_matrix(scene.viewport.height, std::vector<char>(scene.viewport.width)),
          scene(std::move(scene))
    {}
    Matrix produce_picture();
};

}

#endif // TRACER_H

