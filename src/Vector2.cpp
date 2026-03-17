#include "Vector2.h"

Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(double scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(double scalar) const {
    return Vector2(x / scalar, y / scalar);
}

double Vector2::magnitude() const {
    return std::sqrt(x * x + y * y);
}

Vector2 Vector2::normalize() const {
    double mag = magnitude();
    if (mag == 0) return Vector2(0, 0);
    return (*this) / mag;
}
