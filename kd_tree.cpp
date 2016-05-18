#include <vector>
#include <algorithm>

#include "kd_tree.h"
#include "geometry.h"
#include "primitive.h"

ray_tracing::Kd_tree::Kd_tree(const std::vector<std::shared_ptr<Primitive>>& primitives) : root(std::make_shared<Node>())
{
    root->box = Box(Point::MAX, Point::MIN);

    for(const std::shared_ptr<Primitive>& primitive : primitives)
    {
        for(size_t i = 0; i < Point::AXIS_SIZE; ++i)
        {
            root->box.ld[i] = std::min(root->box.ld[i], primitive->point(Point::Axis(i), Either::LEFTEST));
            root->box.ru[i] = std::max(root->box.ru[i], primitive->point(Point::Axis(i), Either::RIGHTEST));
        }
    }

    build(root, primitives);
}

std::array<std::vector<std::shared_ptr<ray_tracing::Primitive>>, 2>
    ray_tracing::Kd_tree::split(const std::vector<std::shared_ptr<Primitive>>& primitives, Point::Axis axis, double splitting_plane) const
{
    std::array<std::vector<std::shared_ptr<ray_tracing::Primitive>>, 2> result;

    for(const std::shared_ptr<Primitive>& primitive : primitives)
    {
        if(primitive->point(axis, Either::LEFTEST) <= splitting_plane)
            result[0].push_back(primitive);
        if(primitive->point(axis, Either::RIGHTEST) >= splitting_plane)
            result[1].push_back(primitive);
    }

    return result;
}

void ray_tracing::Kd_tree::build(const std::shared_ptr<Node>& node, const std::vector<std::shared_ptr<Primitive>>& primitives)
{
    Point::Axis best_splitting_axis;
    double best_splitting_plane;
    double  current_cost = primitives.size() * node->box.surface_area(),
            best_cost = current_cost;

    for(size_t i = 0; i < Point::AXIS_SIZE; ++i)
    {
        for(size_t j = 1; j < SPLITTING_PLANES_NUM; ++j)
        {
            double splitting_plane = node->box.ld[i] +
                                    (node->box.ru[i] - node->box.ld[i]) * j / SPLITTING_PLANES_NUM;

            std::array<std::vector<std::shared_ptr<ray_tracing::Primitive>>, 2> primitives_pair =
                    split(primitives, Point::Axis(i), splitting_plane);

            if(primitives_pair[0].empty() || primitives_pair[1].empty())
                continue;

            std::array<Box, 2> box_pair = node->box.split(Point::Axis(i), splitting_plane);

            double cost = 0;
            for(size_t k = 0; k < 2; ++k)
                cost += primitives_pair[k].size() * box_pair[k].surface_area();

            if(cost < best_cost)
            {
                best_cost = cost;
                best_splitting_axis = Point::Axis(i);
                best_splitting_plane = splitting_plane;
            }
        }
    }

    if(best_cost == current_cost)
    {
        node->contents = primitives;
        return;
    }

    std::array<std::vector<std::shared_ptr<ray_tracing::Primitive>>, 2> primitives_pair =
            split(primitives, best_splitting_axis, best_splitting_plane);

    node->left = std::make_shared<Node>();
    node->right = std::make_shared<Node>();

    std::array<Box, 2> box_pair = node->box.split(best_splitting_axis, best_splitting_plane);
    node->left->box = box_pair[0];
    node->right->box = box_pair[1];

    build(node->left, primitives_pair[0]);
    build(node->right, primitives_pair[1]);
}

std::shared_ptr<ray_tracing::Primitive> ray_tracing::Kd_tree::trace(const Ray& ray) const
{
    return trace(root, ray);
}

std::shared_ptr<ray_tracing::Primitive> ray_tracing::Kd_tree::trace(const std::shared_ptr<Node>& node, const Ray& ray) const
{
    if(!node->contents.empty())
    {
        double coefficient = Ray::NOWHERE;
        std::shared_ptr<Primitive> result_primitive;

        for(const std::shared_ptr<Primitive>& primitive : node->contents)
        {
            Point intersection = primitive->intersect(ray);

            if(!node->box.contains(intersection))
                continue;

            double current_coefficient = ray.coefficient(intersection);
            if(current_coefficient != Ray::NOWHERE && (coefficient == Ray::NOWHERE || coefficient > current_coefficient))
            {
                coefficient = current_coefficient;
                result_primitive = primitive;
            }
        }

        if(coefficient != Ray::NOWHERE)
            return result_primitive;
        else
            return nullptr;
    }

    std::pair<double, std::shared_ptr<Node>> left(intersect(ray, node->left->box), node->left),
                                             right(intersect(ray, node->right->box), node->right);

    if(left > right)
        std::swap(left, right);

    if(left.first == Ray::NOWHERE)
    {
        if(right.first == Ray::NOWHERE)
            return nullptr;
        else
            return trace(right.second, ray);
    }
    else
    {
        std::shared_ptr<Primitive> left_intersection = trace(left.second, ray);
        if(left_intersection)
            return left_intersection;
        else
            return trace(right.second, ray);
    }
}
