#include <GLFW/glfw3.h>

#include <cstdint>
#include <limits>
#include <vector>
#include <cmath>
#include <algorithm>
#include <atomic>
#include <thread>

#include "vec3.h"
#include "ray.h"
#include "triangle.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

constexpr int W = 800;
constexpr int H = 600;


static void addQuad(std::vector<Triangle>& tris, Vec3 a, Vec3 b, Vec3 c, Vec3 d) {
    tris.push_back({ a, b, c });
    tris.push_back({ a, c, d });
}

static void addBox(std::vector<Triangle>& t, Vec3 mn, Vec3 mx) {
    Vec3 a{ mn.x, mn.y, mn.z }, b{ mx.x, mn.y, mn.z }, c{ mx.x, mx.y, mn.z }, d{ mn.x, mx.y, mn.z };
    Vec3 e{ mn.x, mn.y, mx.z }, f{ mx.x, mn.y, mx.z }, g{ mx.x, mx.y, mx.z }, h{ mn.x, mx.y, mx.z };
    addQuad(t, a, b, c, d);  // z min
    addQuad(t, e, f, g, h);  // z max
    addQuad(t, a, e, h, d);  // x min
    addQuad(t, b, f, g, c);  // x max
    addQuad(t, a, b, f, e);  // y min
    addQuad(t, d, c, g, h);  // y max
}

static std::vector<Triangle> buildRoom() {
    std::vector<Triangle> t;
    addQuad(t, { -2, 0,   0 }, { 2, 0,   0 }, { 2, 0,  -4 }, { -2, 0,  -4 });  
    addQuad(t, { -2, 2.8, 0 }, { -2, 2.8, -4 }, { 2, 2.8, -4 }, { 2, 2.8, 0 }); 
    addQuad(t, { -2, 0,  -4 }, { 2, 0,  -4 }, { 2, 2.8, -4 }, { -2, 2.8, -4 }); 
    addQuad(t, { -2, 0,   0 }, { -2, 0,  -4 }, { -2, 2.8, -4 }, { -2, 2.8, 0 });  
    addQuad(t, { 2, 0,  -4 }, { 2, 0,   0 }, { 2, 2.8, 0 }, { 2, 2.8, -4 }); 
    addBox(t, { -1.0, 0, -3.0 }, { -0.2, 0.8, -2.2 });
    return t;
}

static bool occluded(const Ray& r, const std::vector<Triangle>& tris, double tmax) {
    Hit h;
    for (const Triangle& tri : tris)
        if (intersect(r, tri, 1e-4, tmax, h)) return true;
    return false;
}

static void renderRow(int y, std::vector<uint8_t>& fb, const Camera& cam,
    const std::vector<Triangle>& tris) {
    const Vec3 lightPos{ 0, 2, -2 };
    const double I = 100.0;

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
        if (!any) { p[0] = p[1] = p[2] = 0; continue; }

        Vec3 to = lightPos - best.p;
        double d = length(to);
        Vec3 L = to / d;

        double E = 0.0;
        if (!occluded(Ray{ best.p, L }, tris, d - 1e-4)) {
            double c = std::max(0.0, dot(best.normal, L));
            E = I * c / (d * d);
        }

        double v = 1.0 - std::exp(-0.04 * E);
        p[0] = p[1] = p[2] = static_cast<uint8_t>(255.0 * v);
    }
}

static void render(std::vector<uint8_t>& fb, const Camera& cam,
    const std::vector<Triangle>& tris) {
    std::atomic<int> nextRow{ 0 };
    unsigned n = std::max(1u, std::thread::hardware_concurrency());

    auto worker = [&] {
        for (int y; (y = nextRow.fetch_add(1)) < H; )
            renderRow(y, fb, cam, tris);
        };

    std::vector<std::thread> pool;
    for (unsigned i = 0; i < n; ++i) pool.emplace_back(worker);
    for (auto& th : pool) th.join();
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL2_Init();

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

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Light settings");

        ImGui::Text("Light source");

        static float lightX = 0.0f;
        static float lightY = 2.0f;
        static float lightZ = -2.0f;

        ImGui::SliderFloat("X", &lightX, -5.0f, 5.0f);
        ImGui::SliderFloat("Y", &lightY, 0.0f, 5.0f);
        ImGui::SliderFloat("Z", &lightZ, -5.0f, 5.0f);

        ImGui::End();
        ImGui::Render();

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

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(win);
    }

    glDeleteTextures(1, &tex);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}