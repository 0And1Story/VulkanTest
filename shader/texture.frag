#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 TexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformObject {
  float opacity;
} ubo;

layout(binding = 1) uniform sampler2D tex;

void main() {
  outColor = vec4(fragColor, ubo.opacity) * texture(tex, TexCoord);
}
