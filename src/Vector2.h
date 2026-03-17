#pragma once
#include <cmath>

struct Vector2 {
    double x;
    double y;

    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(double scalar) const;
    Vector2 operator/(double scalar) const;

    double magnitude() const;
    Vector2 normalize() const;
};
