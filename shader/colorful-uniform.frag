#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformObject {
  float opacity;
} ubo;

void main() {
  outColor = vec4(fragColor * ubo.opacity, 1.0);
}
