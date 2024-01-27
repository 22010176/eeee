#shader vertex
#version 460 core
layout(location = 0) in vec4 position;
// layout(location = 1) in vec4 out_color;

// out vec4 g_color;

void main() {
   // g_color = out_color;
   gl_Position = position;
}

#shader fragment
#version 460 core

uniform vec4 u_Color;
// layout(location = 0) in vec4 fr_color;
layout(location = 0) out vec4 color;

void main() {
   color = u_Color;
}