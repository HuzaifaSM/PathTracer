#pragma once

#include "ray.h"
#include "aabb.h"

class material;

class hit_record {
public:
    point3 p;
    vec3 normal;
    std::shared_ptr<material> mat;
    double t;
    bool front_face;

    // sets re.normal to always point against incoming ray. outward_normal assumed to already be unit length
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal: -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};