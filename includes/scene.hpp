#pragma once

#include <variant>
#include <vector>

#include "material.hpp"
#include "shapes.hpp"

template <typename TupleOne, typename TupleTwo>
struct Scene;

template <typename... Shapes, typename... Materials>
struct Scene<std::tuple<Shapes...>, std::tuple<Materials...>> {
    struct Object {
        int shape_idx;
        int mat_idx;
    };

    std::vector<Object> objects;
    std::vector<std::variant<Shapes...>> shapes;
    std::vector<std::variant<Materials...>> materials;

    Scene() = default;

    template <typename Shape, typename Mat>
    void add(const Shape& shape, const Mat& material) {
        objects.emplace_back(shapes.size(), materials.size());
        shapes.emplace_back(shape);
        materials.emplace_back(material);
    }

    bool Hit(const Ray& ray, float t_min, float t_max, hit_record& rec) {
        hit_record temp_rec;
        bool hit_anything = false;
        float closest = t_max;

        for (const auto& object : objects) {
            std::visit(
                [&](const auto& obj) {
                    if (obj.Hit(ray, t_min, closest, temp_rec)) {
                        hit_anything = true;
                        closest = temp_rec.t;
                        rec = temp_rec;
                        rec.mat_index = object.mat_idx;
                    }
                },
                shapes[object.shape_idx]);
        }

        return hit_anything;
    }
};
