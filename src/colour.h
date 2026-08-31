#pragma once

#include "vec3.h"

#include <iostream>

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);
    return 0;
}

inline void write_colour(std::ostream& out, const colour& pixel_colour) {
    double r = linear_to_gamma(pixel_colour.x());
    double g = linear_to_gamma(pixel_colour.y());
    double b = linear_to_gamma(pixel_colour.z());

    r = std::fmin(std::fmax(r, 0.0), 0.999);
    g = std::fmin(std::fmax(g, 0.0), 0.999);
    b = std::fmin(std::fmax(b, 0.0), 0.999);

    out << static_cast<int>(256 * r) << ' '
        << static_cast<int>(256 * g) << ' '
        << static_cast<int>(256 * b) << '\n';
}
