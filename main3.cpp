#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if (!x) exit(-1)
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() { while (glGetError() != GL_NO_ERROR); }
static bool GLLogCall(const char* function, const char* file, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "[OpenGL Error] (" << error << "): " << function << std::endl << file << ":" << line << std::endl;
    return false;
  }
  return true;
}

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


int main2(int argc, char* argv[]) {
  if (!glfwInit()) return -1;

  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  std::cout << glGetString(GL_VERSION) << std::endl;

  float position[] = {
    -0.5f,  -0.5f,
     0.5f,  -0.5f,
     0.5f,   0.5f,
    -0.5f,   0.5f
  };
  uint32_t indices[] = { 0,1,2,2,3,0 };

  // uint32_t vao;
  // GLCall(glGenVertexArrays(1, &vao));
  // GLCall(glBindVertexArray(vao));

  uint32_t buffer;
  GLCall(glGenBuffers(1, &buffer));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
  GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), position, GL_STATIC_DRAW));

  GLCall(glEnableVertexAttribArray(0));
  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0));

  uint32_t ibo;
  GLCall(glGenBuffers(1, &ibo));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW));

  ShaderProgramSource source = ParseShader("./shaders/Basic.shader");
  // std::cout << source.VertexSource << std::endl << source.FragmentSource << std::endl;

  uint32_t shader = CreateShader(source.VertexSource, source.FragmentSource);
  GLCall(glUseProgram(shader));

  GLCall(int location = glGetUniformLocation(shader, "u_Color"));
  GLCall(location != -1);

  // GLCall(glBindVertexArray(0));
  GLCall(glUseProgram(0));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
  GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    GLCall(glUseProgram(shader));
    GLCall(glUniform4f(location, 0.0f, 1.0f, 1.0f, 1.0f));
    // GLCall(glBindVertexArray(vao));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}