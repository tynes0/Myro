#include <cmath>
#include <algorithm>

namespace myro
{
    inline vec3& vec3::operator=(float scalar) noexcept { x = y = z = scalar; return *this; }
    inline vec3& vec3::operator=(const vec3& other) noexcept { x = other.x; y = other.y; z = other.z; return *this; }
    inline vec3& vec3::operator=(vec3&& other) noexcept { x = other.x; y = other.y; z = other.z; return *this; }

    constexpr vec3 vec3::operator+(const vec3& right) const noexcept { return { x + right.x, y + right.y, z + right.z }; }
    constexpr vec3 vec3::operator-(const vec3& right) const noexcept { return { x - right.x, y - right.y, z - right.z }; }
    constexpr vec3 vec3::operator*(const vec3& right) const noexcept { return { x * right.x, y * right.y, z * right.z }; }
    constexpr vec3 vec3::operator/(const vec3& right) const { return { x / right.x, y / right.y, z / right.z }; }

    constexpr vec3 vec3::operator+(float scalar) const noexcept { return { x + scalar, y + scalar, z + scalar }; }
    constexpr vec3 vec3::operator-(float scalar) const noexcept { return { x - scalar, y - scalar, z - scalar }; }
    constexpr vec3 vec3::operator*(float scalar) const noexcept { return { x * scalar, y * scalar, z * scalar }; }
    constexpr vec3 vec3::operator/(float scalar) const { return { x / scalar, y / scalar, z / scalar }; }

    inline vec3& vec3::operator+=(const vec3& right) noexcept { x += right.x; y += right.y; z += right.z; return *this; }
    inline vec3& vec3::operator-=(const vec3& right) noexcept { x -= right.x; y -= right.y; z -= right.z; return *this; }
    inline vec3& vec3::operator*=(const vec3& right) noexcept { x *= right.x; y *= right.y; z *= right.z; return *this; }
    inline vec3& vec3::operator/=(const vec3& right) { x /= right.x; y /= right.y; z /= right.z; return *this; }

    inline vec3& vec3::operator+=(float scalar) noexcept { x += scalar; y += scalar; z += scalar; return *this; }
    inline vec3& vec3::operator-=(float scalar) noexcept { x -= scalar; y -= scalar; z -= scalar; return *this; }
    inline vec3& vec3::operator*=(float scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    inline vec3& vec3::operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    inline vec3 vec3::operator++(int) { vec3 r = *this; ++x; ++y; ++z; return r; }
    inline vec3& vec3::operator++() { ++x; ++y; ++z; return *this; }
    inline vec3 vec3::operator--(int) { vec3 r = *this; --x; --y; --z; return r; }
    inline vec3& vec3::operator--() { --x; --y; --z; return *this; }

    constexpr float& vec3::operator[](size_t i) { MYRO_ASSERT(i < 3, "vec3 operator[] out_of_bounds_error!"); return v[i]; }
    constexpr const float& vec3::operator[](size_t i) const { MYRO_ASSERT(i < 3, "vec3 operator[] out_of_bounds_error!"); return v[i]; }

    constexpr vec3 vec3::operator-() const noexcept { return vec3{- x, -y, -z}; }

    // NOLINTNEXTLINE(clang-diagnostic-float-equal)
    constexpr bool vec3::operator==(const vec3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
    constexpr bool vec3::operator!=(const vec3& o) const noexcept { return !(*this == o); }

    constexpr float vec3::magnitude_squared() const noexcept
    {
        return x * x + y * y + z * z;
    }

    constexpr float vec3::dot(const vec3& other) const noexcept
    { 
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr vec3 vec3::cross(const vec3& other) const noexcept
    {
        return vec3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    constexpr float vec3::distance_squared(const vec3& other) const noexcept
    { 
        return (*this - other).magnitude_squared();
    }

    inline float vec3::magnitude() const
    {
        return std::sqrt(magnitude_squared());
    }

    inline float vec3::distance(const vec3& other) const
    { 
        return (*this - other).magnitude(); 
    }

    inline float vec3::angle(const vec3& other) const
    {
        float dp = dot(other);
        float mp = magnitude() * other.magnitude();
        if (mp < default_epsilon) return 0.0;
        float cos_theta = dp / mp;
        return std::acos(std::clamp(cos_theta, -1.0f, 1.0f));
    }

    inline vec3& vec3::normalize()
    {
        float mag = magnitude();
        if (mag > 0) *this /= mag;
        return *this;
    }

    inline vec3 vec3::normalized() const
    { 
        return vec3(*this).normalize();
    }

    inline bool vec3::is_unit(float epsilon) const noexcept
    {
        return std::abs(magnitude_squared() - 1.0f) < epsilon;
    }

    constexpr vec3 vec3::lerp(const vec3& target, float t) const noexcept
    {
        return *this * (1.0f - t) + target * t;
    }

    constexpr vec3 vec3::clamp(const vec3& min, const vec3& max) const
    {
        return vec3{
            std::clamp(x, min.x, max.x),
            std::clamp(y, min.y, max.y),
            std::clamp(z, min.z, max.z)
        };
    }

    constexpr vec3 vec3::clamp(float min, float max) const
    {
        return vec3{
            std::clamp(x, min, max),
            std::clamp(y, min, max),
            std::clamp(z, min, max)
        };
    }

    inline vec3 vec3::abs() const noexcept
    { 
        return vec3{ std::fabs(x), std::fabs(y), std::fabs(z) }; 
    }

    inline vec3 vec3::floor() const noexcept
    {
        return vec3{ std::floor(x), std::floor(y), std::floor(z) }; 
    }

    inline vec3 vec3::ceil() const noexcept
    { 
        return vec3{ std::ceil(x), std::ceil(y), std::ceil(z) }; 
    }

    inline vec3 vec3::round() const noexcept
    {
        return vec3{ std::round(x), std::round(y), std::round(z) }; 
    }

    constexpr vec3 vec3::project(const vec3& onto) const
    {
        float omag = onto.magnitude_squared();
        if (omag < default_epsilon) return vec3::zero;
        return onto * (dot(onto) / omag);
    }

    constexpr vec3 vec3::reflect(const vec3& normal) const
    {
        float nmag = normal.magnitude_squared();
        if (nmag < default_epsilon) return *this;
        return *this - normal * (2.0f * dot(normal) / nmag);
    }

    inline vec3 vec3::refract(const vec3& normal, float eta1, float eta2) const
    {
        float n_dot_i = dot(normal);
        float k = 1.0f - (eta1 / eta2) * (eta1 / eta2) * (1.0f - n_dot_i * n_dot_i);
        if (k < 0.0f)
            return zero;
        return (*this - normal * n_dot_i) * (eta1 / eta2) - normal * std::sqrt(k);
    }

    inline vec3 vec3::rotate_around(const vec3& axis, float angle) const
    {
        float kx = axis.x, ky = axis.y, kz = axis.z;
        float vx = x, vy = y, vz = z;
        float cos_theta = std::cos(angle);
        float sin_theta = std::sin(angle);
        float one_minus_cos = 1.0f - cos_theta;
        
        return vec3{
            vx * (kx * kx * one_minus_cos + cos_theta) + vy * (kx * ky * one_minus_cos - kz * sin_theta) + vz * (kx * kz * one_minus_cos + ky * sin_theta),
            vx * (ky * kx * one_minus_cos + kz * sin_theta) + vy * (ky * ky * one_minus_cos + cos_theta) + vz * (ky * kz * one_minus_cos - kx * sin_theta),
            vx * (kz * kx * one_minus_cos - ky * sin_theta) + vy * (kz * ky * one_minus_cos + kx * sin_theta) + vz * (kz * kz * one_minus_cos + cos_theta)
        };
    }

    constexpr vec3 vec3::smoothstep(const vec3& target, float t) const noexcept
    {
        float clamped_t = std::clamp(t, 0.0f, 1.0f);
        float smooth_t = clamped_t * clamped_t * (3.0f - 2.0f * clamped_t);
        return lerp(target, smooth_t);
    }

    constexpr vec3 vec3::smootherstep(const vec3& target, float t) const noexcept
    {
        float clamped_t = std::clamp(t, 0.0f, 1.0f);
        float smooth_t = clamped_t * clamped_t * clamped_t * (clamped_t * (clamped_t * 6.0f - 15.0f) + 10.0f);
        return lerp(target, smooth_t);
    }

    constexpr float vec3::min_component() const noexcept
    {
        return std::min({ x, y, z });
    }

    constexpr size_t vec3::min_dimension() const noexcept
    {
        if (y < x)
        {
            if (z < y) return 2;
            return 1;
        }
        if (z < x) return 2;
        return 0;
    }

    constexpr float vec3::max_component() const noexcept
    {
        return std::max({ x, y, z });
    }

    constexpr size_t vec3::max_dimension() const noexcept
    {
        if (y > x)
        {
            if (z > y) return 2;
            return 1;
        }
        if (z > x) return 2;
        return 0;
    }

    constexpr int vec3::sign_dot(const vec3& other) const noexcept
    {
        float dot_product = dot(other);
        if (dot_product > default_epsilon) return 1;
        if (dot_product < -default_epsilon) return -1;
        return 0;
    }

    inline bool vec3::equals(const vec3& other, float epsilon) const noexcept
    {
        return (std::fabs(x - other.x) < epsilon) && (std::fabs(y - other.y) < epsilon) && (std::fabs(z - other.z) < epsilon);
    }

    constexpr bool vec3::is_one_of_zero() const noexcept
    { 
        return (x == 0.0 || y == 0.0 || z == 0.0);
    }
    
    constexpr bool vec3::is_zero() const noexcept
    {
        return (x == 0.0 && y == 0.0 && z == 0.0);
    }

    inline bool vec3::is_nan() const noexcept
    {
        return (std::isnan(x) || std::isnan(y) || std::isnan(z));
    }

    inline bool vec3::is_infinite() const noexcept
    {
        return (std::isinf(x) || std::isinf(y) || std::isinf(z));
    }
}