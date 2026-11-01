#pragma once
#include <cmath>
#include "ray.h"

struct Triangle {
    Vec3 v0, v1, v2;
};

struct Hit {
    double t = 0;
    Vec3 normal;
};

inline bool intersect(const Ray& r, const Triangle& tri,
    double tmin, double tmax, Hit& hit) {
    Vec3 e1 = tri.v1 - tri.v0;
    Vec3 e2 = tri.v2 - tri.v0;

    Vec3 p = cross(r.dir, e2);
    double det = dot(e1, p);
    if (std::fabs(det) < 1e-12) return false;  

    double inv = 1.0 / det;
    Vec3 s = r.orig - tri.v0;

    double u = dot(s, p) * inv;
    if (u < 0.0 || u > 1.0) return false;

    Vec3 q = cross(s, e1);
    double v = dot(r.dir, q) * inv;
    if (v < 0.0 || u + v > 1.0) return false;

    double t = dot(e2, q) * inv;
    if (t < tmin || t > tmax) return false;

    Vec3 n = normalize(cross(e1, e2));
    if (dot(n, r.dir) > 0.0) n = -n;           

    hit.t = t;
    hit.normal = n;
    return true;
}