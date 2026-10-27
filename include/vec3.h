#pragma
#include<cmath>

struct Vec3 {
	double x = 0, y = 0, z = 0;
	Vec3() = default;
	Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
	Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
	Vec3 operator-(const Vec3& o) const { return { x - o.x,y - o.y,z - o.z }; }
	Vec3 operator*(double t) const { return { x * t, y * t, z * t }; }
	Vec3 operator/(double t) const { return { x / t, y / t, z / t }; }
};

inline Vec3 operator*(double t, const Vec3& v) { return v * t; }
inline double dot(const Vec3& a, const  Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(const Vec3& a, const Vec3& b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
inline double length(const Vec3& v) { return std::sqrt(dot(v, v)); }
inline Vec3 normalize(const Vec3& v) { return v / length(v); }

struct Ray {
	Vec3 origin, dir;
	Vec3 at(double t) const { return origin + dir * t; }
};