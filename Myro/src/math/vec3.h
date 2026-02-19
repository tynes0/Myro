#pragma once

#include "../core/log.h"
#include <limits>

namespace myro
{
    class vec3
    {
    public:
        union
        {
            MYRO_ANONYMOUS_STRUCT { float x, y, z; };
            MYRO_ANONYMOUS_STRUCT { float r, g, b; };
            float v[3];
        };

        static constexpr float default_epsilon = std::numeric_limits<float>::epsilon();

        static const vec3 zero;
        static const vec3 one;
        static const vec3 unit_x;
        static const vec3 unit_y;
        static const vec3 unit_z;

        constexpr vec3() noexcept : x(0), y(0), z(0) {}
        constexpr vec3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
        explicit constexpr vec3(float scalar) noexcept : x(scalar), y(scalar), z(scalar) {}
        constexpr vec3(const vec3& other) noexcept : x(other.x), y(other.y), z(other.z) {}
        constexpr vec3(vec3&& other) noexcept : x(other.x), y(other.y), z(other.z) {}

        vec3& operator=(float scalar) noexcept;
        vec3& operator=(const vec3& other) noexcept;
        vec3& operator=(vec3&& other) noexcept;

        ~vec3() = default;

        constexpr vec3 operator+(const vec3& right) const noexcept;
        constexpr vec3 operator-(const vec3& right) const noexcept;
        constexpr vec3 operator*(const vec3& right) const noexcept;
        constexpr vec3 operator/(const vec3& right) const;

        constexpr vec3 operator+(float scalar) const noexcept;
        constexpr vec3 operator-(float scalar) const noexcept;
        constexpr vec3 operator*(float scalar) const noexcept;
        constexpr vec3 operator/(float scalar) const;

        vec3& operator+=(const vec3& right) noexcept;
        vec3& operator-=(const vec3& right) noexcept;
        vec3& operator*=(const vec3& right) noexcept;
        vec3& operator/=(const vec3& right);

        vec3& operator+=(float scalar) noexcept;
        vec3& operator-=(float scalar) noexcept;
        vec3& operator*=(float scalar) noexcept;
        vec3& operator/=(float scalar);

        vec3 operator++(int);
        vec3& operator++();
        vec3 operator--(int);
        vec3& operator--();

        constexpr float& operator[](size_t i);
        constexpr const float& operator[](size_t i) const;

        constexpr vec3 operator-() const noexcept;
        constexpr bool operator==(const vec3& other) const noexcept;
        constexpr bool operator!=(const vec3& other) const noexcept;

        constexpr float magnitude_squared() const noexcept;
        constexpr float dot(const vec3& other) const noexcept;
        constexpr vec3 cross(const vec3& other) const noexcept;
        constexpr float distance_squared(const vec3& other) const noexcept;

        float magnitude() const;
        float distance(const vec3& other) const;
        float angle(const vec3& other) const;

        vec3& normalize();
        vec3 normalized() const;
        bool is_unit(float epsilon = default_epsilon) const noexcept;

        constexpr vec3 lerp(const vec3& target, float t) const noexcept;
        constexpr vec3 clamp(const vec3& min, const vec3& max) const;
        constexpr vec3 clamp(float min, float max) const;

        vec3 abs() const noexcept;
        vec3 floor() const noexcept;
        vec3 ceil() const noexcept;
        vec3 round() const noexcept;

        constexpr vec3 project(const vec3& onto) const;
        constexpr vec3 reflect(const vec3& normal) const;
        vec3 refract(const vec3& normal, float eta1, float eta2) const;
        vec3 rotate_around(const vec3& axis, float angle) const;

        constexpr vec3 smoothstep(const vec3& target, float t) const noexcept;
        constexpr vec3 smootherstep(const vec3& target, float t) const noexcept;

        constexpr float min_component() const noexcept;
        constexpr size_t min_dimension() const noexcept;
        constexpr float max_component() const noexcept;
        constexpr size_t max_dimension() const noexcept;

        int constexpr sign_dot(const vec3& other) const noexcept;

        bool equals(const vec3& other, float epsilon = default_epsilon) const noexcept;
        constexpr bool is_one_of_zero() const noexcept;
        constexpr bool is_zero() const noexcept;
        bool is_nan() const noexcept;
        bool is_infinite() const noexcept;
    };
}

#include "vec3.inl"