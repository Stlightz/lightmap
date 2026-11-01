#pragma once
#include <cmath>
#include "vec3.h"

struct Ray {
    Vec3 orig;
    Vec3 dir;
    Vec3 at(double t) const { return orig + t * dir; }
};

struct Camera {
    Vec3 origin, lower_left, horizontal, vertical;

    Camera(Vec3 from, Vec3 look_at, Vec3 up, double vfov_deg, double aspect) {
        double theta = vfov_deg * 3.14159265358979323846 / 180.0;
        double vh = 2.0 * std::tan(theta / 2.0);
        double vw = aspect * vh;

        Vec3 w = normalize(from - look_at);
        Vec3 u = normalize(cross(up, w));
        Vec3 v = cross(w, u);

        origin = from;
        horizontal = vw * u;
        vertical = vh * v;
        lower_left = origin - horizontal / 2 - vertical / 2 - w;
    }

    // s, t в диапазоне [0, 1], (0,0) это левый нижний угол кадра
    Ray get_ray(double s, double t) const {
        return { origin, lower_left + s * horizontal + t * vertical - origin };
    }
};