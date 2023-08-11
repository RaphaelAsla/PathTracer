#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "hit_record.hpp"
#include "ray.hpp"
#include "utils.hpp"

struct Metal {
    Color albedo;

    Metal() = default;

    Metal(const Color& a) : albedo(a) {}

    Color emitted() const {
        return albedo;
    }

    bool scatter(const Ray& ray, const hit_record& rec, Color& attenuation, Ray& scattered) const {
        vec3 reflected = reflect(normalize(ray.direction), rec.normal);
        scattered = Ray(rec.point, reflected);
        attenuation = albedo;
        return (dot(scattered.direction, rec.normal) > 0);
    }
};

struct Lambertian {
    Color albedo;

    Lambertian() = default;

    Lambertian(const Color& a) : albedo(a) {}

    Color emitted() const {
        return albedo;
    }

    bool scatter(const Ray& ray, const hit_record& rec, Color& attenuation, Ray& scattered) const {
        vec3 scatter_direction = rec.normal + normalize(random_in_unit_sphere(GRSEED));

        if (near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }

        scattered = Ray(rec.point, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

struct DiffuseLight {
    Color albedo;

    DiffuseLight() = default;

    DiffuseLight(const Color& a) : albedo(a) {}

    Color emitted() const {
        return albedo;
    }

    bool scatter(const Ray& ray, const hit_record& rec, Color& attenuation, Ray& scattered) const {
        return false;
    }
};

struct Dielectric {
    float ir;

    Dielectric() = default;

    Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

    Color emitted() {
        return Color{0};
    }

    bool scatter(const Ray& ray, const hit_record& rec, Color& attenuation, Ray& scattered) const {
        attenuation = Color{1.0, 1.0, 1.0};
        float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = normalize(ray.direction);
        float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || refractance(cos_theta, refraction_ratio) > random_float(GRSEED))
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.point, direction);
        return true;
    }

  private:
    float refractance(float cosine, float ref_idx) const {
        float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1.0f - cosine), 5);
    }
};
