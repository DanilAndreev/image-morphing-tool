#version 450 core

layout(location = 1) in vec2 iTexCoord;
layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 2) uniform sampler2D targetImage;

void main() {
    fragColor = texture(targetImage, iTexCoord) * 0.2;
}
