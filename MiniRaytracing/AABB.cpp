#include "AABB.h"

bool AABB::intersect(const Ray& ray,
                     FLOAT t_min,
                     FLOAT t_max) const
{
    for (int i = 0; i < 3; i++)
    {
        FLOAT t0 = (min[i] - ray.origin[i]) / ray.direction[i];
        FLOAT t1 = (max[i] - ray.origin[i]) / ray.direction[i];

        FLOAT t_small = MIN(t0, t1);
        FLOAT t_big = MAX(t0, t1);

        t_min = MAX(t_small, t_min);
        t_max = MIN(t_big, t_max);

        if (t_max < t_min)
            return false;
    }

    return true;
}

AABB operator+(const AABB& a, const AABB& b)
{
    vec3 small;
    small.x = MIN(a.min.x, b.min.x);
    small.y = MIN(a.min.y, b.min.y);
    small.z = MIN(a.min.z, b.min.z);

    vec3 big;
    big.x = MAX(a.max.x, b.max.x);
    big.y = MAX(a.max.y, b.max.y);
    big.z = MAX(a.max.z, b.max.z);

    return AABB(small, big);
}