# PathTracer

A from-scratch CPU path tracer in C++, following the shape of "Ray Tracing in One Weekend" as a reference, with room to grow into an actual personal renderer (BVH, scene files, maybe a live pygame viewer over a Python/C++ bridge later).

## Build

```
cmake -B build -S .
cmake --build build
```

Then run the executable from  `build/Release/pathtracer.exe` on Windows with. It writes `output.ppm` in the working directory

## Current state

- `src/vec3.h` — 3D vector math (also reused for colors and points).
- `src/main.cpp` — writes a simple gradient PPM to prove the toolchain
  works end to end.
- `ray` class + a `ray_color` function that returns the background gradient based on ray direction instead of pixel coordinates.

## Roadmap

2. A `camera` (viewport, focal length, pixel-to-ray mapping) so rays are actually cast into the scene.
3. First sphere intersection — ray-sphere hit test, shade by surface normal.
4. `hittable` abstraction + a list of objects, so more than one sphere can exist in a scene.
5. Antialiasing (multisample per pixel) and diffuse materials (random scatter direction).
6. Metal and dielectric (glass) materials.
7. A positionable camera (fov, look-from/look-at, defocus blur).
8. Performance: multithreading, then a BVH for scenes with many objects.
9. Stretch: a scene description format, and/or a pygame front end that displays `output.ppm`-equivalent frames live instead of writing to disk.
