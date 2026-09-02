// axis aligned bounding box
#pragma once

#include "common.h"
#include "ray.h"
#include "vec3.h"

#include <algorithm>

class aabb {
public:
    point3 minimum, maximum;

    aabb() : minimum(point3(infinity, infinity, infinity)),
             maximum(point3(-infinity, -infinity, -infinity)) {}
    aabb(const point3& a, const point3& b) : minimum(a), maximum(b) {}

    point3 min() const { return minimum; }
    point3 max() const { return maximum; }

    bool hit(const ray& r, double ray_tmin, double ray_tmax) const {
        for (int axis = 0; axis < 3; axis++) {
            double invD = 1.0 / r.direction()[axis];
            double t0 = (minimum[axis] - r.origin()[axis]) * invD;
            double t1 = (maximum[axis] - r.origin()[axis]) * invD;

            if (invD < 0.0) std::swap(t0, t1);

            ray_tmin = t0 > ray_tmin ? t0 : ray_tmin;
            ray_tmax = t1 > ray_tmax ? t1 : ray_tmax;

            if (ray_tmax <= ray_tmin)
                return false;
        }
        return true;
    }
};

inline aabb surrounding_box(aabb box0, aabb box1) {
    point3 small(std::fmin(box0.min().x(), box1.min().x()),
                 std::fmin(box0.min().y(), box1.min().y()),
                std::fmin(box0.min().z(), box1.min().z()));
    
    point3 big(std::fmax(box0.max().x(), box1.max().x()),
               std::fmax(box0.max().y(), box1.max().y()),
               std::fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}
