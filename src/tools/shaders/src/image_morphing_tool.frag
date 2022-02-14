#version 450 core

layout(location = 1) in vec2 iTexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D targetImage;

void main() {
    fragColor = texture(targetImage, iTexCoord);
}
