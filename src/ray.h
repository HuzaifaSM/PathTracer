#pragma once

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction) {}

    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }

    // the point at parameter t along the ray: P(t) = origin + t * direction
    point3 at(double t) const { return orig + t * dir; }

private:
    point3 orig;
    vec3 dir;
};
