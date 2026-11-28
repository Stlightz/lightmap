
#include <GLFW/glfw3.h>

#include <cstdint>
#include <vector>

#include "math/vec3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "lighting/light.h"
#include "renderer/renderer.h"
#include "scene/scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

constexpr int W = 800;
constexpr int H = 600;

constexpr int PREVIEW_W = W / 2;
constexpr int PREVIEW_H = H / 2;



int main() {
    // Инициализация GLFW
    if (!glfwInit())
        return 1;

    GLFWwindow* win = glfwCreateWindow(
        W, H, "lightmap", nullptr, nullptr
    );

    if (!win) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(win, true)) {
        ImGui::DestroyContext();
        glfwDestroyWindow(win);
        glfwTerminate();
        return 1;
    }

    if (!ImGui_ImplOpenGL2_Init()) {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(win);
        glfwTerminate();
        return 1;
    }

    // Framebuffer
    std::vector<uint8_t> fb(W * H * 3);

    // OpenGL texture
    GLuint tex;

    // Preview texture
    GLuint previewTex;

    glGenTextures(1, &previewTex);
    glBindTexture(GL_TEXTURE_2D, previewTex);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        PREVIEW_W,
        PREVIEW_H,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        W, H,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    // Создание сцены
    Scene scene = buildDefaultRoom();

    // Создание камеры
    Camera cam(
        { 0, 1.5, -0.2 },
        { 0, 1.2, -4 },
        { 0, 1, 0 },
        80.0,
        double(W) / H
    );

    // Источник света
    Light light;

    // Рендерер
    Renderer renderer(W, H);
    // Превью
    Renderer previewRenderer(PREVIEW_W, PREVIEW_H);

    std::vector<uint8_t> previewFb(
        PREVIEW_W * PREVIEW_H * 3
    );

    // Параметры интерфейса
    float lightPosition[3] = {
        static_cast<float>(light.position.x),
        static_cast<float>(light.position.y),
        static_cast<float>(light.position.z)
    };

    float lightIntensity =
        static_cast<float>(light.intensity);

    // Первый расчёт
    renderer.render(fb, cam, scene, light);

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0, 0, 0,
        W, H,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        fb.data()
    );


    bool previewVisible = false;
    // Главный цикл
    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        // Начало кадра ImGui
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Панель управления
        ImGui::Begin("Light settings");

        ImGui::Text("Point light source");
        ImGui::Separator();

        bool positionChanged = ImGui::SliderFloat3(
            "Position",
            lightPosition,
            -5.0f,
            5.0f
        );

        bool positionActive = ImGui::IsItemActive();
        bool positionFinished = ImGui::IsItemDeactivatedAfterEdit();

        bool intensityChanged = ImGui::SliderFloat(
            "Intensity (cd)",
            &lightIntensity,
            0.0f,
            1000.0f
        );

        bool intensityActive = ImGui::IsItemActive();
        bool intensityFinished = ImGui::IsItemDeactivatedAfterEdit();

        // Обновляем параметры света
        light.position = Vec3{
            lightPosition[0],
            lightPosition[1],
            lightPosition[2]
        };

        light.intensity = lightIntensity;

        bool previewActive = positionActive || intensityActive;

        bool previewChanged =
            (positionChanged || intensityChanged) && previewActive;

        bool editFinished =
            positionFinished || intensityFinished;

        // Preview
        if (previewChanged) {
            previewRenderer.render(
                previewFb,
                cam,
                scene,
                light
            );

            glBindTexture(GL_TEXTURE_2D, previewTex);

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0, 0, 0,
                PREVIEW_W,
                PREVIEW_H,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                previewFb.data()

            );
            previewVisible = true;
        }

        // Final render
        if (editFinished) {
            renderer.render(fb, cam, scene, light);

            glBindTexture(GL_TEXTURE_2D, tex);

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0, 0, 0,
                W, H,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                fb.data()
            );
            previewVisible = false;
        }

        ImGui::Separator();

        ImGui::Text(
            "Intensity: %.1f cd",
            light.intensity
        );

        ImGui::End();

        // Отрисовка сцены
        int fw, fh;
        glfwGetFramebufferSize(win, &fw, &fh);

        glViewport(0, 0, fw, fh);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);

      

        glBindTexture(
            GL_TEXTURE_2D,
            previewVisible ? previewTex : tex
        );

        glBegin(GL_QUADS);

        glTexCoord2f(0, 1);
        glVertex2f(-1, -1);

        glTexCoord2f(1, 1);
        glVertex2f(1, -1);

        glTexCoord2f(1, 0);
        glVertex2f(1, 1);

        glTexCoord2f(0, 0);
        glVertex2f(-1, 1);

        glEnd();

        // Отрисовка интерфейса
        ImGui::Render();

        ImGui_ImplOpenGL2_RenderDrawData(
            ImGui::GetDrawData()
        );

        glfwSwapBuffers(win);
    }

    // Освобождение ресурсов
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &tex);
    glDeleteTextures(1, &previewTex);

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
