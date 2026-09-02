#pragma once

#include "colour.h"
#include "common.h"
#include "hittable.h"
#include "material.h"

#include <atomic>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

class camera {
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov = 90;
    point3 lookfrom = point3(0,0,0);
    point3 lookat = point3(0,0,-1);
    vec3 vup = vec3(0,1,0);
    double defocus_angle = 0;
    double focus_dist = 10;

    void render(const hittable& world) {
        initialise();

        std::vector<colour> framebuffer(image_width * image_height);
        std::atomic<int> rows_done{0};

        unsigned int thread_count = std::thread::hardware_concurrency();
        if (thread_count == 0) thread_count = 4;

        std::vector<std::thread> threads;

        for (unsigned int t = 0; t < thread_count; ++t) {
            threads.emplace_back([this, t, thread_count, &world, &framebuffer, &rows_done]() {
                render_rows(t, thread_count, world, framebuffer, rows_done);
            });
        }

        for (auto& thread : threads)
            thread.join();

        std::ofstream out("output.ppm");
        out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (const colour& pixel_colour : framebuffer)
            write_colour(out, pixel_colour);

        std::clog << "\rDone.                        \n";
    }

private:
    int image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel100_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialise() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        double theta = degrees_to_radians(vfov);
        double h = std::tan(theta / 2);
        double viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width) / image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        vec3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        vec3 offset = sample_square();
        point3 pixel_sample = pixel100_loc
                             + ((i + offset.x()) * pixel_delta_u)
                             + ((j + offset.y()) * pixel_delta_v);

        point3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    void render_rows(int start_row, int stride, const hittable& world, std::vector<colour>& framebuffer, std::atomic<int>& rows_done) const {
        for (int j = start_row; j < image_height; j += stride) {
            for (int i = 0; i < image_width; ++i) {
                colour pixel_colour(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_colour += ray_colour(r, max_depth, world);
                }
                framebuffer[j * image_width + i] = pixel_samples_scale * pixel_colour;
            }

            int done = ++rows_done;
            std::clog << "\rScanlines completed: " << done << " / " << image_height << ' ' << std::flush;
        }
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    colour ray_colour(const ray& r, int depth, const hittable& world) const {
        if (depth <= 0)
            return colour(0,0,0);

        hit_record rec;
        if (world.hit(r, 0.001, infinity, rec)) {
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
};