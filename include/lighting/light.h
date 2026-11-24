#pragma once

#include "math\vec3.h"

struct Light {
    Vec3 position{ 0.0, 2.0, -2.0 };
    double intensity = 100.0;
};
