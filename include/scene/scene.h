#pragma once

#include <vector>
#include "geometry\triangle.h"

struct Scene {
    std::vector<Triangle> triangles;
};

Scene buildDefaultRoom();