#pragma once

#include "ray.hpp"
#include "utils.hpp"

struct Camera {
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 look_from;
    vec3 look_at;
    vec3 vup;
    float vfov;
    float aspect_ratio;

    Camera(float _aspect_ratio) {
        vup = vec3{0.0f, 1.0f, 0.0f};
        vfov = 90.0f;
        look_from = vec3{0.0f, 1.0f, 1.0f};
        look_at = vec3{0.0f, 0.0f, 0.0f};
        aspect_ratio = _aspect_ratio;
        UpdateVectors();
    }

    void UpdateVectors() {
        float theta = degrees_to_radians(vfov);
        float h = std::tan(theta / 2.0f);
        float viewport_height = 2.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        vec3 w = normalize(look_from - look_at);
        vec3 u = normalize(cross(vup, w));
        vec3 v = cross(w, u);

        origin = look_from;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - w;
    }

    Ray get_ray(float s, float t) const {
        return Ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }
};
