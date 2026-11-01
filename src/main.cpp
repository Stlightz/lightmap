#include<GLFW/glfw3.h>

#include<vector>
#include<iostream>
#include<cassert>
#include<cmath>
#include "vec3.h"

constexpr int W = 800;
constexpr int H = 600;

void render(std::vector<uint8_t>& fb) {
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			uint8_t* p = &fb[(y * W + x) * 3];
			p[0] = static_cast<uint8_t>(255 * x / W);
			p[1] = static_cast<uint8_t>(255 * y / H);
			p[2] = 128;
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

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        render(fb);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, fb.data());

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

 