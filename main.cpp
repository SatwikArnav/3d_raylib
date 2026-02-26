#include "raylib.h"
#include "raymath.h"

#include <vector>

struct Room {
    BoundingBox bounds;
    Color wallColor;
    const char *name;
};

static Vector3 Midpoint(const BoundingBox &b) {
    return {
        (b.min.x + b.max.x) * 0.5f,
        (b.min.y + b.max.y) * 0.5f,
        (b.min.z + b.max.z) * 0.5f,
    };
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Vintage House Explorer - raylib");
    DisableCursor();

    Camera3D camera = {0};
    camera.position = {0.0f, 1.8f, 2.5f};
    camera.target = {0.0f, 1.8f, -1.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(120);

    const float houseHalfWidth = 9.0f;
    const float houseHalfDepth = 11.5f;
    const float wallHeight = 3.3f;

    const std::vector<Room> rooms = {
        {{{-8.0f, 0.0f, -11.0f}, {-0.8f, wallHeight, -2.0f}}, Color{136, 97, 76, 255}, "Library"},
        {{{0.8f, 0.0f, -11.0f}, {8.0f, wallHeight, -2.0f}}, Color{121, 105, 74, 255}, "Dining Room"},
        {{{-8.0f, 0.0f, 2.0f}, {-0.8f, wallHeight, 11.0f}}, Color{98, 124, 95, 255}, "Parlor"},
        {{{0.8f, 0.0f, 2.0f}, {8.0f, wallHeight, 11.0f}}, Color{117, 89, 92, 255}, "Bedroom"},
    };

    const std::vector<BoundingBox> corridorBlocks = {
        {{-0.8f, 0.0f, -11.0f}, {0.8f, wallHeight, -5.0f}},
        {{-0.8f, 0.0f, 5.0f}, {0.8f, wallHeight, 11.0f}},
        {{-8.0f, 0.0f, -2.0f}, {-4.5f, wallHeight, 2.0f}},
        {{4.5f, 0.0f, -2.0f}, {8.0f, wallHeight, 2.0f}},
    };

    const std::vector<BoundingBox> furniture = {
        {{-6.8f, 0.0f, -8.2f}, {-3.8f, 1.0f, -6.3f}},
        {{2.5f, 0.0f, -8.8f}, {6.5f, 1.0f, -6.5f}},
        {{-6.5f, 0.0f, 6.5f}, {-2.5f, 0.8f, 8.5f}},
        {{2.8f, 0.0f, 5.5f}, {6.0f, 1.1f, 7.2f}},
    };

    const BoundingBox houseBounds = {{-houseHalfWidth, 0.0f, -houseHalfDepth}, {houseHalfWidth, wallHeight, houseHalfDepth}};

    while (!WindowShouldClose()) {
        const Vector3 oldPos = camera.position;
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        camera.position.y = 1.8f;
        camera.target.y = 1.8f;

        bool blocked = false;

        if (camera.position.x < houseBounds.min.x + 0.3f || camera.position.x > houseBounds.max.x - 0.3f ||
            camera.position.z < houseBounds.min.z + 0.3f || camera.position.z > houseBounds.max.z - 0.3f) {
            blocked = true;
        }

        BoundingBox playerBox = {
            {camera.position.x - 0.25f, 0.0f, camera.position.z - 0.25f},
            {camera.position.x + 0.25f, 1.8f, camera.position.z + 0.25f},
        };

        for (const auto &block : corridorBlocks) {
            if (CheckCollisionBoxes(playerBox, block)) {
                blocked = true;
                break;
            }
        }

        if (!blocked) {
            for (const auto &item : furniture) {
                if (CheckCollisionBoxes(playerBox, item)) {
                    blocked = true;
                    break;
                }
            }
        }

        if (blocked) {
            camera.position = oldPos;
            Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
            camera.target = Vector3Add(camera.position, forward);
        }

        const char *currentRoom = "Hallway";
        for (const auto &room : rooms) {
            if (CheckCollisionPointBox(camera.position, room.bounds)) {
                currentRoom = room.name;
                break;
            }
        }

        BeginDrawing();
        ClearBackground(Color{31, 27, 24, 255});

        BeginMode3D(camera);

        DrawPlane({0.0f, 0.0f, 0.0f}, {houseHalfWidth * 2.0f, houseHalfDepth * 2.0f}, Color{82, 58, 40, 255});
        DrawPlane({0.0f, wallHeight, 0.0f}, {houseHalfWidth * 2.0f, houseHalfDepth * 2.0f}, Color{195, 177, 151, 255});

        DrawCube({-houseHalfWidth, wallHeight * 0.5f, 0.0f}, 0.28f, wallHeight, houseHalfDepth * 2.0f, Color{103, 72, 52, 255});
        DrawCube({houseHalfWidth, wallHeight * 0.5f, 0.0f}, 0.28f, wallHeight, houseHalfDepth * 2.0f, Color{103, 72, 52, 255});
        DrawCube({0.0f, wallHeight * 0.5f, -houseHalfDepth}, houseHalfWidth * 2.0f, wallHeight, 0.28f, Color{103, 72, 52, 255});
        DrawCube({0.0f, wallHeight * 0.5f, houseHalfDepth}, houseHalfWidth * 2.0f, wallHeight, 0.28f, Color{103, 72, 52, 255});

        for (const auto &room : rooms) {
            Vector3 mid = Midpoint(room.bounds);
            Vector3 size = Vector3Subtract(room.bounds.max, room.bounds.min);
            DrawCube(mid, size.x, size.y, size.z, room.wallColor);
            DrawCubeWires(mid, size.x, size.y, size.z, Fade(BLACK, 0.5f));
        }

        for (const auto &block : corridorBlocks) {
            Vector3 mid = Midpoint(block);
            Vector3 size = Vector3Subtract(block.max, block.min);
            DrawCube(mid, size.x, size.y, size.z, Color{118, 81, 58, 255});
        }

        for (const auto &item : furniture) {
            Vector3 mid = Midpoint(item);
            Vector3 size = Vector3Subtract(item.max, item.min);
            DrawCube(mid, size.x, size.y, size.z, Color{84, 50, 34, 255});
            DrawCubeWires(mid, size.x, size.y, size.z, Fade(BLACK, 0.45f));
        }

        DrawCube({0.0f, 1.0f, 0.0f}, 1.0f, 2.0f, 0.2f, Color{186, 155, 103, 255});
        DrawCube({-0.05f, 1.0f, 0.0f}, 0.9f, 1.8f, 0.22f, Color{62, 43, 29, 255});

        EndMode3D();

        DrawRectangle(18, 16, 370, 100, Fade(BLACK, 0.45f));
        DrawText("Vintage House Explorer", 32, 28, 24, RAYWHITE);
        DrawText("W/A/S/D to walk, mouse to look around", 32, 58, 19, Color{214, 207, 197, 255});
        DrawText(TextFormat("Current room: %s", currentRoom), 32, 82, 19, Color{233, 211, 169, 255});

        DrawFPS(screenWidth - 100, 20);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
