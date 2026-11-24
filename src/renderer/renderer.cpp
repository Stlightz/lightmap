
#include "renderer/renderer.h"

#include <limits>
#include <cmath>
#include <algorithm>
#include <atomic>
#include <thread>

#include "geometry/triangle.h"


static bool occluded(
    const Ray& ray,
    const std::vector<Triangle>& triangles,
    double tmax
) {
    Hit hit;

    for (const Triangle& triangle : triangles) {
        if (intersect(ray, triangle, 1e-4, tmax, hit))
            return true;
    }

    return false;
}


static void renderRow(
    int y,
    int width,
    int height,
    std::vector<uint8_t>& framebuffer,
    const Camera& camera,
    const std::vector<Triangle>& triangles,
    const Light& light
) {
    for (int x = 0; x < width; ++x) {
        double s = (x + 0.5) / width;
        double t = 1.0 - (y + 0.5) / height;

        Ray ray = camera.get_ray(s, t);

        double closest = std::numeric_limits<double>::infinity();

        Hit best;
        bool any = false;

    
        for (const Triangle& triangle : triangles) {
            Hit hit;

            if (intersect(ray, triangle, 1e-4, closest, hit)) {
                closest = hit.t;
                best = hit;
                any = true;
            }
        }

        uint8_t* pixel =
            &framebuffer[(y * width + x) * 3];

        if (!any) {
            pixel[0] = pixel[1] = pixel[2] = 0;
            continue;
        }

        Vec3 toLight = light.position - best.p;
        double distance = length(toLight);

        if (distance < 1e-8) {
            pixel[0] = pixel[1] = pixel[2] = 0;
            continue;
        }

        Vec3 L = toLight / distance;

        double E = 0.0;

        
        if (!occluded(
            Ray{ best.p, L },
            triangles,
            distance - 1e-4
        )) {
            double cosTheta =
                std::max(0.0, dot(best.normal, L));

            E = light.intensity * cosTheta /
                (distance * distance);
        }

      
        double value = 1.0 - std::exp(-0.04 * E);

        uint8_t color =
            static_cast<uint8_t>(255.0 * value);

        pixel[0] = color;
        pixel[1] = color;
        pixel[2] = color;
    }
}


Renderer::Renderer(int width, int height)
    : width_(width), height_(height) {}

// расчёт изображения
void Renderer::render(
    std::vector<uint8_t>& framebuffer,
    const Camera& camera,
    const Scene& scene,
    const Light& light
) const {
    std::atomic<int> nextRow{ 0 };

    unsigned threadCount = std::max(
        1u,
        std::thread::hardware_concurrency()
    );

    threadCount = std::min(
        threadCount,
        static_cast<unsigned>(height_)
    );

    auto worker = [&]() {
        for (int y; (y = nextRow.fetch_add(1)) < height_;) {
            renderRow(
                y,
                width_,
                height_,
                framebuffer,
                camera,
                scene.triangles,
                light
            );
        }
        };

    std::vector<std::thread> pool;

    for (unsigned i = 0; i < threadCount; ++i) {
        pool.emplace_back(worker);
    }

    for (auto& thread : pool) {
        thread.join();
    }
}
