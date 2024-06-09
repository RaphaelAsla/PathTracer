#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <random>

using namespace glm;

using Color = vec3;

static std::default_random_engine rng;
static std::uniform_real_distribution<float> norm01{0.f, 1.f};
static std::uniform_real_distribution<float> norm11{-1.f, 1.f};

const double infinity = std::numeric_limits<double>::infinity();
const double pi = std::numbers::pi;

static uint32_t GRSEED = 0;

template <typename T>
void print(const T& t) {
    std::cout << t << std::endl;
}

inline void print(const vec3& v) {
    std::cout << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}

inline void print(const vec2& v) {
    std::cout << v.x << ' ' << v.y << std::endl;
}

inline double degrees_to_radians(double degrees) {
    return degrees * (pi / 180.0f);
}

inline bool near_zero(const vec3& vec) {
    const float k = 1e-8;
    return (std::fabs(vec.x) < k) && (std::fabs(vec.y) < k) && (std::fabs(vec.z) < k);
}

inline uint32_t PCG_Hash(uint32_t input) {
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

inline float random_float(uint32_t& seed) {
    seed = PCG_Hash(seed);
    return (float)seed / (float)std::numeric_limits<uint32_t>::max();
}

inline vec3 random_in_unit_sphere(uint32_t& seed) {
    float theta = random_float(seed) * (2.0f * pi);
    float phi = acos(random_float(seed) * 2.0f - 1.0f);
    return vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

inline vec3 random_in_hemisphere(const vec3& normal, uint32_t& seed) {
    vec3 in_unit_sphere = random_in_unit_sphere(seed);
    if (dot(in_unit_sphere, normal) > 0.0)  // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}
