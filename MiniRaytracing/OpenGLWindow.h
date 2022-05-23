#pragma once

#include "RaytracingAPI.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

class OpenGLWindow
{
public:
    OpenGLWindow();
    OpenGLWindow(const OpenGLWindow&) = delete;
    OpenGLWindow& operator=(const OpenGLWindow&) = delete;
    virtual ~OpenGLWindow();

    void open(int width, int height);
    void close();
    bool is_closing() const;
    void display(const RGBA* data, int width, int height);
    void update_title(const TIME& begin, int completion);
    void update_event();

private:
    bool init_display_image();
    void update_image_data(const RGBA* data, int width, int height);
    GLuint create_program(const char* src_vs, const char* src_fs);

private:
    GLFWwindow* _window = nullptr;
    GLuint _program = 0;
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _ibo = 0;
    GLuint _image = 0;
};
