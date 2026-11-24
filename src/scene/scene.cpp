
#include "scene/scene.h"

static void addQuad(
    std::vector<Triangle>& tris,
    Vec3 a, Vec3 b, Vec3 c, Vec3 d
) {
    tris.push_back({ a, b, c });
    tris.push_back({ a, c, d });
}

static void addBox(
    std::vector<Triangle>& tris,
    Vec3 mn, Vec3 mx
) {
    Vec3 a{ mn.x, mn.y, mn.z };
    Vec3 b{ mx.x, mn.y, mn.z };
    Vec3 c{ mx.x, mx.y, mn.z };
    Vec3 d{ mn.x, mx.y, mn.z };

    Vec3 e{ mn.x, mn.y, mx.z };
    Vec3 f{ mx.x, mn.y, mx.z };
    Vec3 g{ mx.x, mx.y, mx.z };
    Vec3 h{ mn.x, mx.y, mx.z };

    addQuad(tris, a, b, c, d);
    addQuad(tris, e, f, g, h);
    addQuad(tris, a, e, h, d);
    addQuad(tris, b, f, g, c);
    addQuad(tris, a, b, f, e);
    addQuad(tris, d, c, g, h);
}

Scene buildDefaultRoom() {
    Scene scene;

    auto& t = scene.triangles;

    // Пол
    addQuad(t,
        { -2, 0, 0 }, { 2, 0, 0 },
        { 2, 0, -4 }, { -2, 0, -4 }
    );

    // Потолок
    addQuad(t,
        { -2, 2.8, 0 }, { -2, 2.8, -4 },
        { 2, 2.8, -4 }, { 2, 2.8, 0 }
    );

    // Задняя стена
    addQuad(
        t, { -2, 0, -4 }, { 2, 0, -4 },
        { 2, 2.8, -4 }, { -2, 2.8, -4 }
    );

    // Левая стена
    addQuad(t,
        { -2, 0, 0 }, { -2, 0, -4 },
        { -2, 2.8, -4 }, { -2, 2.8, 0 }
    );

    // Правая стена
    addQuad(t,
        { 2, 0, -4 }, { 2, 0, 0 },
        { 2, 2.8, 0 }, { 2, 2.8, -4 }
    );

    // Коробка внутри комнаты
    addBox(t,
        { -1.0, 0, -3.0 },
        { -0.2, 0.8, -2.2 }
    );

    return scene;
}
