#include <fstream>
#include <iostream>

#include "vec3.h"

void write_color(std::ostream& out, const color& pixel_color) {
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

int main() {
    const int image_width = 256;
    const int image_height = 256;

    std::ofstream out("output.ppm");
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::clog << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(double(i) / (image_width - 1),
                               double(j) / (image_height - 1), 0.25);
            write_color(out, pixel_color);
        }
    }

    std::clog << "\rDone.                 \n";
    return 0;
}
