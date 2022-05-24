#include "BVHNode.h"

BVHNode::BVHNode()
{}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Object>>& objects,
                 size_t index_begin,
                 size_t index_end,
                 const DEPTH_BOUNDS& bounds)
{
    std::vector<std::shared_ptr<Object>> duplicates;
    duplicates.assign(objects.begin() + index_begin,
                      objects.begin() + index_end);

    int axis = (int)random_in(0, 2.99);
    std::sort(duplicates.begin(),
              duplicates.end(),
              [&](const std::shared_ptr<Object>& a,
                  const std::shared_ptr<Object>& b) {
        FLOAT v1 = a->bounding_box(bounds).min[axis];
        FLOAT v2 = b->bounding_box(bounds).min[axis];
        return v1 < v2;
    });

    if (duplicates.size() == 0)
    {
        printf("Error: objects list in BVH node is empty\n");
        assert(false);
    }
    else if (duplicates.size() == 1)
    {
        _left  = duplicates[0];
        _right = duplicates[0];
    }
    else if (duplicates.size() == 2)
    {
        _left  = duplicates[0];
        _right = duplicates[1];
    }
    else 
    {
        size_t count = duplicates.size();
        _left  = std::make_shared<BVHNode>(duplicates, 0, count / 2, bounds);
        _right = std::make_shared<BVHNode>(duplicates, count / 2, count, bounds);
    }

    AABB left_box  = _left->bounding_box(bounds);
    AABB right_box = _right->bounding_box(bounds);
    _bounding_box = left_box + right_box;
}

BVHNode::~BVHNode()
{}

bool BVHNode::intersect(const Ray& ray,
                        const DEPTH_BOUNDS& bounds,
                        Intersection* hit)
{
    if (!_bounding_box.intersect(ray, bounds))
        return false;

    bool hit_left = _left->intersect(ray, bounds, hit);

    FLOAT closest_hit = hit_left ? hit->depth_hit : bounds.second;
    DEPTH_BOUNDS new_bounds(bounds.first, closest_hit);
    bool hit_right = _right->intersect(ray, new_bounds, hit);

    return hit_left || hit_right;
}

AABB BVHNode::bounding_box(const DEPTH_BOUNDS& bounds) const
{
    return _bounding_box;
}