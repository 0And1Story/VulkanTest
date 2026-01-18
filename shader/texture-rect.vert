#version 450

layout(location = 0) in vec2 position;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 TexCoord;

vec3 colors[4] = vec3[] (
  vec3(1.0, 0.0, 0.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(1.0, 1.0, 1.0)
);

vec2 TexCoords[4] = vec2[] (
  vec2(0.0, 0.0),
  vec2(1.0, 0.0),
  vec2(1.0, 1.0),
  vec2(0.0, 1.0)
);

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  fragColor = colors[gl_VertexIndex];
  TexCoord = TexCoords[gl_VertexIndex];
}
