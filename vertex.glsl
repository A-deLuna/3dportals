#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

uniform mat4 mvp;
out vec3 vertexColor;

void main() {
  gl_Position = mvp * vec4(pos, 1.0);
  vertexColor = color;
}
