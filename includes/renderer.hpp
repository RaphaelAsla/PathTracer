#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <thread>

#include "camera.hpp"
#include "scene.hpp"
#include "shapes.hpp"

struct Renderer {
    GLFWwindow* window;
    GLuint texture;
    GLuint framebuffer;
    Camera camera;
    ivec2 window_size;
    ivec2 texture_size;
    float last_render = 0.0f;
    float texture_size_multiplier = 1.0f;
    int max_depth = 8;
    int samples = 0;
    int entity_id = 0;

    std::vector<Color> pixels;
    std::vector<std::thread> threads{std::thread::hardware_concurrency() * 3};

    using Objects = std::tuple<Sphere, Box>;
    using Materials = std::tuple<Metal, Lambertian, DiffuseLight, Dielectric>;
    Scene<Objects, Materials> world;

    Renderer(int width, float aspect_ratio) : camera{aspect_ratio} {
        int height = width / aspect_ratio;

        // Setup GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        window = glfwCreateWindow(width, height, "Ray Tracing Skeleton 02 - ImGui", nullptr, nullptr);

        // Setup OpenGL
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSwapInterval(1);

        // Register for resize callback
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, [](auto wnd, int w, int h) { ((Renderer*)glfwGetWindowUserPointer(wnd))->OnResize(w, h); });

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        // Create OpenGL resources
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        // Initial window size
        glfwGetWindowSize(window, &window_size.x, &window_size.y);
        OnResize(window_size.x, window_size.y);
    }

    ~Renderer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    Color& Pixel(int x, int y) {
        return pixels[y * texture_size.x + x];
    }

    void WritePixelsToTexture() {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture_size.x, texture_size.y, 0, GL_RGB, GL_FLOAT, pixels.data());
    }

    void OnResize(int w, int h) {
        window_size = {w, h};
        ResizeTexture(int(w * texture_size_multiplier), int(h * texture_size_multiplier));
    }

    void ResizeTexture(int w, int h) {
        // Resize CPU-side pixel storage
        // (OpenGL Resources are automatically resized on WritePixelsToTexture)
        texture_size = {w, h};
        pixels.resize(texture_size.x * texture_size.y);

        samples = 0;
    }

    Color RayColor(const Ray& ray, int depth) {
        hit_record rec;

        if (depth <= 0) {
            return Color{0.0f};
        }

        if (!world.Hit(ray, 0.001, infinity, rec)) {
            return Color{0.0f, 0.0f, 0.0f};
        }

        Ray scattered;
        Color attenuation;
        Color emitted;
        bool scatter;

        std::visit(
            [&](auto& mat) {
                emitted = mat.emitted();
                scatter = mat.scatter(ray, rec, attenuation, scattered);
            },
            world.materials[rec.mat_index]);

        if (!scatter) {
            return emitted;
        }

        return (emitted + attenuation) * RayColor(scattered, depth - 1);
    }

    void MakePixels(float weight) {
        const auto make = [&](const int start, const int end) {
            for (int y = start; y < end; y++) {
                for (int x = 0; x < texture_size.x; x++) {
                    float u = (x + norm01(rng)) / texture_size.x;
                    float v = (y + norm01(rng)) / texture_size.y;
                    Color color = RayColor(camera.get_ray(u, v), max_depth);
                    color = pow(color, Color(1.0f / 2.2f));
                    Pixel(x, y) = Pixel(x, y) * (1.0f - weight) + weight * color;
                }
            }
        };

        const size_t render_rows = threads.size();
        const size_t render_rows_size = texture_size.y / render_rows;

        for (size_t i = 0; i < render_rows; i++) {
            threads[i] = std::thread(make, i * render_rows_size, (i + 1) * render_rows_size);
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    void Run() {
        auto ground_material = Lambertian(Color{0.3, 0.2, 0.1});
        auto white_light = DiffuseLight(Color{1.0, 0.91, 0.81} * vec3{10.0f});
        auto metal = Metal(Color{0.8, 0.8, 0.8});
        auto lambertian = Lambertian(Color{1.0, 0.8, 1.0});
        auto dielectric = Dielectric(1.5);

        world.add(Sphere(vec3{0.0f, -100.5f, -1.0f}, 100.0f), ground_material);
        world.add(Sphere(vec3{0.0f, 7.0f, -12.0f}, 8.0f), white_light);

        world.add(Box(vec3(0, -0.5, -1), vec3(1, 3.0, 0)), white_light);
        world.add(Box(vec3(-2.5, -0.5, -3.5), vec3(-1.5, 0.5, -2.5)), white_light);

        world.add(Sphere(vec3{0.5f, 0.0f, -2.0f}, 0.5f), metal);
        world.add(Sphere(vec3{-1.0f, 0.0f, -2.0f}, 0.5f), dielectric);
        world.add(Sphere(vec3{-1.0f, 0.0f, -0.5f}, 0.5f), lambertian);

        std::chrono::steady_clock::time_point current_ticks, delta_ticks;
        float ms;
        while (!glfwWindowShouldClose(window)) {
            current_ticks = std::chrono::steady_clock::now();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");

            ImGui::Text("Sample: %i", samples);
            ImGui::Text("Texture Size: (%i, %i)", texture_size.x, texture_size.y);

            if (ImGui::SliderFloat("Texture scale", &texture_size_multiplier, 0.02f, 1.f)) {
                ResizeTexture(int(window_size.x * texture_size_multiplier), int(window_size.y * texture_size_multiplier));
                samples = 0;
            }

            ImGui::Text("Last render: %.3fms", ms);

            if (ImGui::DragFloat3("Look From", &camera.look_from.x, 0.1f, -10.0f, 10.0f)) {
                samples = 0;
            }

            if (ImGui::DragFloat3("Look at", &camera.look_at.x, 0.1f, -10.0f, 10.0f)) {
                samples = 0;
            }

            if (ImGui::SliderFloat("FOV", &camera.vfov, 0.0f, 180.0f - 0.1f)) {
                samples = 0;
            }

            ImGui::InputInt("Entity ID", &entity_id);

            std::visit(
                [&](auto&& object) {
                    if (ImGui::DragFloat3("Object Coords", &object.center.x, 0.1, -10.0f, 10.0f)) {
                        samples = 0;
                    }
                },
                world.shapes[entity_id]);

            ImGui::End();

            camera.UpdateVectors();

            if (samples < 1e8) {
                MakePixels(1.0f / ++samples);
            }

            WritePixelsToTexture();
            glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, texture_size.x, texture_size.y, 0, 0, window_size.x, window_size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);

            glfwPollEvents();
            delta_ticks = std::chrono::steady_clock::now();
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ticks - current_ticks).count();
        }
    }
};
