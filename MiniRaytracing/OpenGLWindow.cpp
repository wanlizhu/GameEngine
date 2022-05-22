#include "OpenGLWindow.h"
#include "BasicToolsRT.h"

OpenGLWindow::OpenGLWindow()
{
    glfwInit();
}

OpenGLWindow::~OpenGLWindow()
{
    close();
    glfwTerminate();
}

void OpenGLWindow::open(int width, int height)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    _window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    glfwMakeContextCurrent(_window);

    if (_window == NULL)
    {
        printf("Failed to create OpenGL window\n");
        return;
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return;
    }

    if (!init_display_image())
    {
        close();
    }
}

void OpenGLWindow::close()
{
    if (!_window)
        return;

    if (_program)
    {
        glDeleteProgram(_program);
        _program = 0;
    }

    if (_vao)
    {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }

    if (_vbo)
    {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }

    if (_ibo)
    {
        glDeleteBuffers(1, &_ibo);
        _ibo = 0;
    }

    if (_image)
    {
        glDeleteTextures(1, &_image);
        _image = 0;
    }

    glfwSetWindowShouldClose(_window, GLFW_TRUE);
    glfwDestroyWindow(_window);
    _window = nullptr;
}

bool check_shader_error(GLuint id, GLenum type)
{
    GLint success = 0;
    GLchar info[1024];

    if (type == 0)
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        
        if (!success)
        {
            glGetProgramInfoLog(id, 1024, NULL, info);
            printf("%s\n", info);
            return false;
        }
    }
    else
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(id, 1024, NULL, info);
            printf("%s\n", info);
            return false;
        }
    }

    return true;
}

GLuint OpenGLWindow::create_program(const char* src_vs, const char* src_fs)
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &src_vs, NULL);
    glCompileShader(vs);

    if (!check_shader_error(vs, GL_VERTEX_SHADER))
    {
        glDeleteShader(vs);
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &src_fs, NULL);
    glCompileShader(fs);

    if (!check_shader_error(fs, GL_FRAGMENT_SHADER))
    {
        glDeleteShader(fs);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    if (!check_shader_error(program, 0))
    {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

bool OpenGLWindow::init_display_image()
{
    const char* src_vs = "#version 330 core\n"
        "layout(location = 0) in vec3 in_position;\n"
        "layout(location = 1) in vec2 in_texcoord;\n"
        "out vec3 vs_position;\n"
        "out vec2 vs_texcoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(in_position, 1.0);\n"
        "    vs_position = in_position;\n"
        "    vs_texcoord = in_texcoord;\n"
        "}\n";

    const char* src_fs = "#version 330 core\n"
        "in  vec3 vs_position;\n"
        "in  vec2 vs_texcoord;\n"
        "out vec4 out_color;\n"
        "\n"
        "uniform sampler2D image;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    out_color = texture(image, vs_texcoord);\n"
        "}\n";

    float positions[] = {
        // position  // texcoord
        -1, -1, 0,   0, 1 - 0,
         1, -1, 0,   1, 1 - 0,
         1,  1, 0,   1, 1 - 1,
        -1,  1, 0,   0, 1 - 1,
    };

    uint32_t indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    // create program
    _program = create_program(src_vs, src_fs);

    if (_program == 0)
        return false;

    // create and bind VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    
    // create and fill vertex buffer
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    
    // create and fill index buffer
    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // enable and specify attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // create texture unit
    glGenTextures(1, &_image);
    glBindTexture(GL_TEXTURE_2D, _image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void OpenGLWindow::update_image_data(const RGBA* data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, _image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGLWindow::display(const RGBA* data, int width, int height)
{
    if (!_window)
        return;

    update_image_data(data, width, height);

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_program);
    glBindTexture(GL_TEXTURE_2D, _image);
    glBindVertexArray(_vao); 
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void OpenGLWindow::update_title(const TIME& begin, int completion)
{
    if (!_window)
        return;

    int width, height;
    glfwGetWindowSize(_window, &width, &height);

    const char* title = "";

    if (completion == width * height)
    {
        int past = SECONDS_SINCE(begin);
        title = cstr_format("Completed - cost: %dmin %dsec", past / 60, past % 60);
    }
    else
    {
        float ratio = completion / float(width * height);
        int past = SECONDS_SINCE(begin);
        int eta = past * (1 - ratio) / ratio;
        
        std::string eta_time(256, '\0');
        snprintf(eta_time.data(), eta_time.size(), "%dmin %dsec", eta / 60, eta % 60);

        title = cstr_format("Completion %d/%d - %.2f%% - eta: %s",
                            completion,
                            width * height,
                            ratio * 100,
                            eta_time.c_str());
    }

    glfwSetWindowTitle(_window, title);
}

void OpenGLWindow::update_event()
{
    if (!_window)
        return;

    if (glfwWindowShouldClose(_window))
        close();
    else
        glfwPollEvents();
}

