#pragma once
#include <cmath>
#include <algorithm>

struct Vec2 {
    double x = 0.0, y = 0.0;

    Vec2() = default;
    Vec2(double x, double y) : x(x), y(y) {}

    Vec2  operator+ (const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2  operator- (const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2  operator* (double s)      const { return {x*s,   y*s};   }
    Vec2  operator/ (double s)      const { return {x/s,   y/s};   }
    Vec2& operator+=(const Vec2& o)       { x+=o.x; y+=o.y; return *this; }
    Vec2& operator-=(const Vec2& o)       { x-=o.x; y-=o.y; return *this; }
    Vec2& operator*=(double s)            { x*=s;   y*=s;   return *this; }

    double length()   const { return std::sqrt(x*x + y*y); }
    double lengthSq() const { return x*x + y*y; }

    Vec2 normalized() const {
        double m = length();
        return (m > 1e-15) ? Vec2{x/m, y/m} : Vec2{0, 0};
    }

    static double distance(const Vec2& a, const Vec2& b) {
        return (b - a).length();
    }
};

inline Vec2 operator*(double s, const Vec2& v) { return v * s; }
