#include <cmath>
#include <memory>
#include <thread>
#include <algorithm>
#include <future>
#include <map>

#include "tracer.h"
#include "kd_tree.h"
#include "continuous_performer.h"

ray_tracing::Light::Light_force
    ray_tracing::Tracer::light_force(const std::shared_ptr<Primitive>& primitive, const Ray& ray) const
{
    Light::Light_force light_force = Light::DARKNESS;
    Point point = primitive->intersect(ray);

    for(const Light& l : scene.lights)
    {
        Ray light_ray(l.place, point);

        std::shared_ptr<Primitive> light_intersection = tree.trace(light_ray);
        if(!light_intersection)
            continue;

        if(light_intersection->intersect(light_ray) == point &&
           light_intersection->side(light_ray) == primitive->side(ray))
        {
            light_force += l.calculate(primitive->angle_cos(light_ray),
                                       angle_cos(-ray.guiding(), primitive->reflect(light_ray).guiding()),
                                       point);
        }
    }

    return light_force;
}

ray_tracing::Ray ray_tracing::Tracer::produce_ray(double i, double j) const
{
    return Ray(scene.viewport.view,
               scene.viewport.left_down +
               (scene.viewport.left_up - scene.viewport.left_down) * i / scene.viewport.height +
               (scene.viewport.right_down - scene.viewport.left_down) * j / scene.viewport.width);
}

ray_tracing::Color ray_tracing::Tracer::trace(const Ray& ray, size_t depth) const
{
    if(depth == 0)
        return Color::BLACK;

    std::shared_ptr<Primitive> intersection = tree.trace(ray);
    if(!intersection)
        return Color::BLACK;

    Point intersection_point = intersection->intersect(ray);
    Color intersection_color = intersection->get_color(intersection_point);

    double  alpha = intersection->get_alpha(),
            transparency = intersection->get_transparency();
    Color result;

    if(!eq_zero(1 - alpha) && intersection_color != Color::BLACK)
        result += intersection_color * (1 - alpha) * light_force(intersection, ray);

    if(!eq_zero(alpha))
        result += trace(intersection->reflect(ray).correct(), depth - 1) * alpha;

    if(!eq_zero(transparency))
        result += trace(intersection->refract(ray).correct(), depth - 1) * transparency;

    return result;
}

void ray_tracing::Tracer::produce_picture_helper(size_t from, size_t to)
{
    for(size_t i = from; i < to; ++i)
        for(size_t j = 0; j < matrix.width(); ++j)
            matrix[i][j] = trace(produce_ray(i + 0.5, j + 0.5), TRACE_DEPTH);
}

void ray_tracing::Tracer::anti_aliasing_determinant(size_t from, size_t to)
{
    for(size_t i = from; i < to; ++i)
        for(size_t j = 0; j < matrix.width(); ++j)
        {
            std::vector<Color> loc;

            for(int k = -1; k < 2; ++k)
                for(int b = -1; b < 2; ++b)
                {
                    int new_i = i + k, new_j = j + b;
                    if(0 <= new_i && new_i < matrix.height() &&
                       0 <= new_j && new_j < matrix.width())
                        loc.push_back(matrix[new_i][new_j]);
                }

            Color expectation, variance;

            for(const Color& color : loc)
            {
                expectation += color;
                variance += color * color;
            }

            variance = variance / loc.size() - expectation * expectation / pow(loc.size(), 2.0);

            if(variance.mod() > ANTI_ALIASING_BOUND)
                determinant_matrix[i][j] = true;
        }
}

void ray_tracing::Tracer::anti_aliasing_performer(size_t from, size_t to)
{
    std::map<std::array<double, 2>, Color> additionally_traced_rays;

    for(size_t i = from; i < to; ++i)
        for(size_t j = 0; j < determinant_matrix[i].size(); ++j)
        {
            if(!determinant_matrix[i][j])
                continue;

            Color result;
            for(int g = 0; g < 3; ++g)
                for(int h = 0; h < 3; ++h)
                {
                    if(g == 1 && h == 1)
                        continue;

                    double x = i + 0.5 * g, y = j + 0.5 * h;
                    auto iter = additionally_traced_rays.find({x, y});

                    if(iter != additionally_traced_rays.end())
                        result += iter->second;
                    else
                        result += additionally_traced_rays[std::array<double, 2>{x, y}] =
                                    trace(produce_ray(x, y), TRACE_DEPTH);
                }

            matrix[i][j] = (result + matrix[i][j]) / 9;
        }
}

template<typename F>
void ray_tracing::Tracer::parallel_perform(F function)
{
    size_t tasks_num = matrix.width() * matrix.height() / RAYS_PER_SECOND;

    if(tasks_num == 0)
        tasks_num = 1;

    std::vector<std::future<void>> tasks;

    size_t chunk_size = matrix.height() / tasks_num;
    for(size_t i = 0; i + 1 < tasks_num; ++i)
        tasks.push_back(std::async(  std::launch::deferred,
                                     function,
                                     this,
                                     i * chunk_size,
                                     (i + 1) * chunk_size));

    tasks.push_back(std::async(  std::launch::deferred,
                                 function,
                                 this,
                                 (tasks_num - 1) * chunk_size,
                                 matrix.height()));

    performer.continuous_perform(tasks);
}

ray_tracing::Matrix ray_tracing::Tracer::produce_picture()
{
    parallel_perform(&Tracer::produce_picture_helper);
    parallel_perform(&Tracer::anti_aliasing_determinant);
    parallel_perform(&Tracer::anti_aliasing_performer);

    return matrix;
}
