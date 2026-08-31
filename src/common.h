#pragma once

#include <cstdlib>

inline double random_double() {
    // returns a random real in [0, 1)
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // returns a random real in [min, max)
    return min + (max - min) * random_double();
}