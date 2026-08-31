#include <fstream>
#include <iostream>

#include "vec3.h"
#include "ray.h"
#include "material.h"
#include "hittable_list.h"
#include "sphere.h"
#include "common.h"

const double infinity = std::numeric_limits<double>::infinity();

// colours a ray based on direction: blends white to sky-blue based on how 'up' the ray points. a = 0 at bottom, 1 at top
colour ray_colour(const ray& r, int depth, const hittable& world) {
    if (depth <= 0)
        return colour(0,0,0);

    hit_record rec;
    if (world.hit(r, 0.01, infinity, rec)) {
        ray scattered;
        colour attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_colour(scattered, depth - 1, world);
        return colour(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
}

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);
    return 0;
}

void write_colour(std::ostream& out, const colour& pixel_colour) {
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

int main() {
    // image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width) / image_height);
    point3 camera_center = point3(0, 0, 0);

    int samples_per_pixel = 100;
    double pixel_samples_scale = 1.0 / samples_per_pixel;
    int max_depth = 50;

    // vectors spanning viewport's full width (right) and height (down)
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // distance from one pixel's center to next in world units
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // world-space location of center of top-left pixel
    point3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    point3 pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto material_ground = make_shared<lambertian>(colour(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(colour(0.1, 0.2, 0.5));

    hittable_list world;
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, material_center));

    // render
    std::ofstream out("output.ppm");
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            colour pixel_colour(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                double offset_u = random_double() - 0.5;
                double offset_v = random_double() - 0.5;

                point3 pixel_sample = pixel100_loc
                                    + ((i + offset_u) * pixel_delta_u)
                                    + ((j + offset_v) * pixel_delta_v);

                vec3 ray_direction = pixel_sample - camera_center;
                ray r(camera_center, ray_direction);

                pixel_colour += ray_colour(r, max_depth, world);
            }
            write_colour(out, pixel_samples_scale * pixel_colour);
        }
    }

    std::clog << "\rDone.                 \n";
    return 0;
}
