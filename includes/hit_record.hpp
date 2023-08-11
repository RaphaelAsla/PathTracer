#pragma once

#include "ray.hpp"
#include "utils.hpp"

struct hit_record {
    vec3 point;
    vec3 normal;
    int mat_index;
    float t;
    bool front_face;

    inline void set_face_normal(const Ray& ray, const vec3& outward_normal) {
        front_face = dot(ray.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};
