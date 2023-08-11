#pragma once

#include "hit_record.hpp"

struct Sphere {
    vec3 center;
    float radius;
    int mat_index;

    Sphere() = default;

    Sphere(vec3 _center, float _radius) : center{_center}, radius{_radius} {}

    bool Hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const {
        vec3 oc = ray.origin - center;
        auto a = dot(ray.direction, ray.direction);
        auto half_b = dot(oc, ray.direction);
        auto c = dot(oc, oc) - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root) {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root) {
                return false;
            }
        }

        rec.t = root;
        rec.point = ray.at(rec.t);
        vec3 outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(ray, outward_normal);
        rec.mat_index = mat_index;

        return true;
    }
};

struct Box {
    int mat_index;
    vec3 center;
    vec3 min;
    vec3 max;

    Box() = default;

    Box(const vec3& p0, const vec3& p1) : min{p0}, max{p1} {}

    bool Hit(const Ray& ray, float t_min, float t_max, hit_record& rec) const {
        vec3 inv_direction = 1.0f / ray.direction;
        vec3 t0s = (min - ray.origin) * inv_direction;
        vec3 t1s = (max - ray.origin) * inv_direction;

        vec3 tsmaller = glm::min(t0s, t1s);
        vec3 tbigger = glm::max(t0s, t1s);

        float tenter = std::max(tsmaller.x, std::max(tsmaller.y, tsmaller.z));
        float texit = std::min(tbigger.x, std::min(tbigger.y, tbigger.z));

        if (tenter < texit && texit > t_min && tenter < t_max) {
            rec.t = tenter;
            rec.point = ray.at(rec.t);

            vec3 outward_normal;
            if (tsmaller.x == tenter)
                outward_normal = vec3(1, 0, 0);
            else if (tsmaller.y == tenter)
                outward_normal = vec3(0, 1, 0);
            else
                outward_normal = vec3(0, 0, 1);

            rec.set_face_normal(ray, outward_normal);
            rec.mat_index = mat_index;
            return true;
        }

        return false;
    }
};
