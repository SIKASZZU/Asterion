#include <iostream>
#include <cmath>

struct Vector2 {
    float x, y;
};

// Constants for isometric transform (i-hat and j-hat)
constexpr float i_x = 1.0f;
constexpr float i_y = 0.5f;
constexpr float j_x = -1.0f;
constexpr float j_y = 0.5f;

// Sprite size
constexpr int w = 32;
constexpr int h = 32;

// Convert grid (tile) coordinate to screen coordinate
Vector2 to_screen_coordinate(const Vector2& tile) {
    // With sprite size considered
    return {
        tile.x * i_x * 0.5f * w + tile.y * j_x * 0.5f * w,
        tile.x * i_y * 0.5f * h + tile.y * j_y * 0.5f * h
    };
}

// Invert a 2x2 matrix
struct Matrix2x2 {
    float a, b, c, d;
};

Matrix2x2 invert_matrix(float a, float b, float c, float d) {
    float det = 1.0f / (a * d - b * c);
    return {
        det * d,
        det * -b,
        det * -c,
        det * a
    };
}

// Convert screen coordinate back to grid (tile) coordinate
Vector2 to_grid_coordinate(const Vector2& screen) {
    float a = i_x * 0.5f * w;
    float b = j_x * 0.5f * w;
    float c = i_y * 0.5f * h;
    float d = j_y * 0.5f * h;

    Matrix2x2 inv = invert_matrix(a, b, c, d);

    return {
        screen.x * inv.a + screen.y * inv.b,
        screen.x * inv.c + screen.y * inv.d
    };
}