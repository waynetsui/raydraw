#include <iostream>
#include "raylib.h"
#include "rlgl.h"
#include <vector>
#define MAX_TOUCH_POINTS 64
#define MAX_TOUCH_POINTS_MAK 63

using namespace std;
bool Vector2Equal(Vector2 &a, Vector2 &b)
{
    return (a.x == b.x && a.y == b.y);
}

using Path = vector<Vector2>;

int main()
{
    bool path_valid = false;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    vector<Path> paths;
    Path *currentPath = nullptr;
    SetConfigFlags(FLAG_MSAA_4X_HINT); //4次多重采样，以达到抗锯齿的效果
    InitWindow(screenWidth, screenHeight, "mouse input");
    ToggleFullscreen();
    SetTargetFPS(60);

    int index = 0;
    rlSetLineWidth(5);
    rlEnableSmoothLines();
    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            paths.push_back(Path());
            currentPath = &paths.back();
            currentPath->push_back(GetMousePosition());
            path_valid = true;
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            path_valid = false;
            currentPath = nullptr;
        }

        if (path_valid)
        {
            Vector2 pos = GetMousePosition();
            if (!Vector2Equal(pos, currentPath->back()))
                currentPath->push_back(pos);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto &path : paths)
        {
            if (path.size() > 1)
                DrawLineStrip(path.data(), path.size(), ORANGE);
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}