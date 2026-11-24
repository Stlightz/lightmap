#pragma once

#include <vector>
#include <cstdint>

#include "geometry/ray.h"
#include "scene/scene.h"
#include "lighting/light.h"

class Renderer {
public:
    Renderer(int width, int height);

    void render(
        std::vector<uint8_t>& framebuffer,
        const Camera& camera,
        const Scene& scene,
        const Light& light
    ) const;

private:
    int width_;
    int height_;
};