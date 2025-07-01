#pragma once

#include "../core/log.h"

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <limits>


namespace myro
{
    class vec2
    {
    public:
        union
        {
            struct { float x, y; };
            struct { float r, g; };
            float v[2];
        };

        friend class vec3;

        static const vec2 zero;
        static const vec2 one;
        static const vec2 unit_x;
        static const vec2 unit_y;

        constexpr vec2() noexcept : x(0), y(0) { }
        constexpr vec2(float x, float y) noexcept : x(x), y(y) { }
        explicit constexpr vec2(float scalar) noexcept : x(scalar), y(scalar) { }
        constexpr vec2(const vec2& other) noexcept : x(other.x), y(other.y) { }

        vec2(const vec3& other) noexcept;
        vec2& operator=(const vec3& other) noexcept;

        vec2& operator=(float scalar) noexcept;
        vec2& operator=(const vec2& other) noexcept;

        constexpr vec2 operator+(const vec2& right) const noexcept;
        constexpr vec2 operator-(const vec2& right) const noexcept;
        constexpr vec2 operator*(const vec2& right) const noexcept;
        constexpr vec2 operator/(const vec2& right) const;

        constexpr vec2 operator+(float scalar) const noexcept;
        constexpr vec2 operator-(float scalar) const noexcept;
        constexpr vec2 operator*(float scalar) const noexcept;
        constexpr vec2 operator/(float scalar) const;

        vec2& operator+=(const vec2& right) noexcept;
        vec2& operator-=(const vec2& right) noexcept;
        vec2& operator*=(const vec2& right) noexcept;
        vec2& operator/=(const vec2& right);

        vec2& operator+=(float scalar) noexcept;
        vec2& operator-=(float scalar) noexcept;
        vec2& operator*=(float scalar) noexcept;
        vec2& operator/=(float scalar);

        vec2 operator++(int);
        vec2& operator++();
        vec2 operator--(int);
        vec2& operator--();

        constexpr float& operator[](size_t idx);
        constexpr const float& operator[](size_t idx) const;

        constexpr vec2 operator-() const noexcept;
        constexpr bool operator==(const vec2& other) const noexcept;
        constexpr bool operator!=(const vec2& other) const noexcept;

        constexpr float magnitude_squared() const noexcept;
        constexpr float dot(const vec2& other) const noexcept;
        constexpr float cross(const vec2& other) const noexcept;
        constexpr float distance_squared(const vec2& other) const noexcept;

        float magnitude() const;
        float distance(const vec2& other) const;
        float angle(const vec2& other) const;

        vec2& normalize();
        vec2 normalized() const;
        vec2& rotate(float angle_rad);
        vec2 rotated(float angle_rad) const;

        constexpr vec2 lerp(const vec2& target, float t) const noexcept;
        constexpr vec2 clamp(const vec2& min, const vec2& max) const;

        vec2 abs() const noexcept;
        vec2 floor() const noexcept;
        vec2 ceil() const noexcept;
        vec2 round() const noexcept;

        vec2 project(const vec2& onto) const;
        vec2 reflect(const vec2& normal) const;

        constexpr vec2 perpendicular() const noexcept;

        bool equals(const vec2& other, float epsilon = 1e-9) const noexcept;
        constexpr bool is_one_of_zero() const noexcept;
        constexpr bool is_zero() const noexcept;
        bool is_nan() const noexcept;
        bool is_infinite() const noexcept;
    };
}

#include "vec2.inl"