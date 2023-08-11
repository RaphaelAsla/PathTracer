#pragma once

#include <variant>
#include <vector>

#include "material.hpp"
#include "shapes.hpp"

struct Object {
    int shape_idx;
    int mat_idx;
};

template <typename TupleOne, typename TupleTwo>
struct Scene;

template <typename... Shapes, typename... Materials>
struct Scene<std::tuple<Shapes...>, std::tuple<Materials...>> {
    Scene() = default;

    void clear() {}

    template <typename Shape, typename Mat>
    void add(const Shape& shape, const Mat& material) {
        int shape_idx = shapes.size();
        int material_idx = materials.size();
        Object object{shape_idx, material_idx};
        shapes.emplace_back(shape);
        materials.emplace_back(material);
        objects.emplace_back(object);
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

    std::vector<Object> objects;
    std::vector<std::variant<Shapes...>> shapes;
    std::vector<std::variant<Materials...>> materials;
};
