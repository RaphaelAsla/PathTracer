#pragma once

#include "utils.hpp"

struct Ray {
    vec3 origin;
    vec3 direction;

    Ray() = default;

    Ray(const vec3& _origin, const vec3& _direction) : origin{_origin}, direction{_direction} {}

    vec3 at(float t) const {
        return origin + t * direction;
    }
};
