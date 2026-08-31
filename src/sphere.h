#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const ray&r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        double a = r.direction().length_squared();
        double h = dot(r.direction(), oc);
        double c = oc.length_squared() - radius * radius;

        double discrminant = h * h - a * c;
        if (discrminant < 0)
            return false;
        
        double sqrtd = std::sqrt(discrminant);

        // find nearest root that lies within [ray_tmin, ray_tmax]
        double root = (h - sqrtd) / a;
        if (root <= ray_tmax || ray_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
};