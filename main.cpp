#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource { std::string VertexSource, FragmentSource; };
enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos) type = ShaderType::FRAGMENT;
            continue;
        }

        ss[(int)type] << line << '\n';
    }
    stream.close();

    return  { ss[0].str(), ss[1].str() };
}

static uint32_t CompileShader(uint32_t type, const std::string& source) {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int res;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &res));

    if (res == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::cout << "Fail to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

static uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(uint32_t program = glCreateProgram());

    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}


int main(int argc, char* argv[]) {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::vector<float> position{ -0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f, -1.0f,1.0f,0.0f,1.0f,-1.0f,-1.0f };
    std::vector<float> position2{ 0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f };
    std::vector<uint32_t> indices{ 0,1,2,3,4,5 };
    {
        uint32_t vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(position.data(), position.size() * sizeof(float));
        VertexBuffer vb2(position2.data(), position2.size() * sizeof(float));

        IndexBuffer ib(indices.data(), indices.size());

        ShaderProgramSource source = ParseShader("./shaders/Basic.shader");
        ShaderProgramSource source2 = ParseShader("./shaders/Test.shader");

        uint32_t shader = CreateShader(source.VertexSource, source.FragmentSource);
        uint32_t shader2 = CreateShader(source2.VertexSource, source2.FragmentSource);


        GLCall(glUseProgram(shader));
        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        GLCall(location != -1);
        GLCall(glUniform4f(location, 0.0f, 1.0f, 1.0f, 1.0f));

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            vb.Bind();
            GLCall(glEnableVertexAttribArray(0));
            GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), NULL));
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            vb2.Bind();
            GLCall(glEnableVertexAttribArray(1));
            GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), NULL));
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteProgram(shader);
        glDeleteProgram(shader2);
    }
    glfwTerminate();
    return 0;
}