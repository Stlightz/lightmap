#include<GLFW/glfw3.h>

#include<iostream>
#include<cassert>
#include<cmath>
#include "vec3.h"
int main() {
	std::cout << "lightmap works\n\n";
	Vec3 c = cross({ 1,0,0 }, { 0,1,0 });
	assert(c.x == 0 && c.y == 0 && c.z == 1);
	assert(dot({ 1,2,3 }, { 4,5,6 }) == 32);
	assert(std::abs(length({ 3,4,0 }) - 5.0) < 1e-12);
	assert(std::abs(length(normalize({ 2,0,0 })) - 1.0) < 1e-12);
	Ray r{ {0,0,0},{0,0,1} };
	assert(r.at(2.5).z == 2.5);
	glfwInit();
	glfwTerminate();
	std::cout << "tests passed\n";
}