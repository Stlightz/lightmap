#include <GLFW/glfw3.h>

#include <cstdint>
#include <limits>
#include <vector>
#include <cmath>
#include <algorithm>

#include "vec3.h"
#include "ray.h"
#include "triangle.h"

constexpr int W = 800;
constexpr int H = 600;


static void addQuad(std::vector<Triangle>& tris, Vec3 a, Vec3 b, Vec3 c, Vec3 d) {
    tris.push_back({ a, b, c });
    tris.push_back({ a, c, d });
}

static std::vector<Triangle> buildRoom() {
    std::vector<Triangle> t;
    addQuad(t, { -2, 0,   0 }, { 2, 0,   0 }, { 2, 0,  -4 }, { -2, 0,  -4 });  
    addQuad(t, { -2, 2.8, 0 }, { -2, 2.8, -4 }, { 2, 2.8, -4 }, { 2, 2.8, 0 }); 
    addQuad(t, { -2, 0,  -4 }, { 2, 0,  -4 }, { 2, 2.8, -4 }, { -2, 2.8, -4 }); 
    addQuad(t, { -2, 0,   0 }, { -2, 0,  -4 }, { -2, 2.8, -4 }, { -2, 2.8, 0 });  
    addQuad(t, { 2, 0,  -4 }, { 2, 0,   0 }, { 2, 2.8, 0 }, { 2, 2.8, -4 });  
    return t;
}

static void render(std::vector<uint8_t>& fb, const Camera& cam,
    const std::vector<Triangle>& tris) {
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double s = (x + 0.5) / W;
            double t = 1.0 - (y + 0.5) / H;    
            Ray r = cam.get_ray(s, t);

            double closest = std::numeric_limits<double>::infinity();
            Hit best;
            bool any = false;
            for (const Triangle& tri : tris) {
                Hit h;
                if (intersect(r, tri, 1e-4, closest, h)) {
                    closest = h.t;
                    best = h;
                    any = true;
                }
            }

            uint8_t* p = &fb[(y * W + x) * 3];
            if (any) {
                Vec3 lightPos{ 0, 2, -2 };
                double I = 100.0;

                Vec3 to = lightPos - best.p;
                double d = length(to);
                double c = std::max(0.0, dot(best.normal, to / d));
                double E = I * c / (d * d);

                double v = 1.0 - std::exp(-0.04 * E);
                uint8_t g = static_cast<uint8_t>(255.0 * v);
                p[0] = p[1] = p[2] = g;
            }
            else {
                p[0] = p[1] = p[2] = 0;
            }
        }
    }
}

int main() {
    if (!glfwInit()) return 1;

    GLFWwindow* win = glfwCreateWindow(W, H, "lightmap", nullptr, nullptr);
    if (!win) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    std::vector<uint8_t> fb(W * H * 3);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


    std::vector<Triangle> scene = buildRoom();
    Camera cam({ 0, 1.5, -0.2 }, { 0, 1.2, -4 }, { 0, 1, 0 }, 80.0, double(W) / H);
    render(fb, cam, scene);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, fb.data());

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        int fw, fh;
        glfwGetFramebufferSize(win, &fw, &fh);
        glViewport(0, 0, fw, fh);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(-1, -1);
        glTexCoord2f(1, 1); glVertex2f(1, -1);
        glTexCoord2f(1, 0); glVertex2f(1, 1);
        glTexCoord2f(0, 0); glVertex2f(-1, 1);
        glEnd();

        glfwSwapBuffers(win);
    }

    glDeleteTextures(1, &tex);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}