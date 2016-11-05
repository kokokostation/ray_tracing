#ifndef KD_TREE
#define KD_TREE

#include <vector>
#include <memory>

#include "primitive.h"
#include "geometry.h"

namespace ray_tracing
{

struct Node
{
    Box box;
    std::vector<std::shared_ptr<Primitive>> contents;
    std::shared_ptr<Node> left, right;
};

class Kd_tree
{
private:
    static const size_t SPLITTING_PLANES_NUM = 3;

private:
    std::shared_ptr<Node> root;

    void build(const std::shared_ptr<Node>& node,
               const std::vector<std::shared_ptr<Primitive>>& primitives);

    std::shared_ptr<Primitive> trace(const std::shared_ptr<Node>& node, const Ray& ray) const;

    std::array<std::vector<std::shared_ptr<Primitive>>, 2>
        split(const std::vector<std::shared_ptr<Primitive>>& primitives,
              Point::Axis axis, double splitiing_plane) const;

public:
    Kd_tree(const std::vector<std::shared_ptr<Primitive>>& primitives);
    std::shared_ptr<Primitive> trace(const Ray& ray) const;
};

}

#endif // KD_TREE

