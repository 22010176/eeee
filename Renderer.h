#pragma once

#include <cstdlib>
#include <glad/glad.h>

#define ASSERT(x) if (!x) exit(-1)
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);