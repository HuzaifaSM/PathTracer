#pragma once

#include "hittable.h"

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
public:
    lambertian(const colour& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // catch degenerate case where random_unit_vector lands almost exactly opposite rec.normal, maing sum ~(0,0,0)
        // zero-length direction breaks math
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    colour albedo;
};