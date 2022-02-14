#version 450 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 iTexCoord;
layout(location = 1) out vec2 oTexCoord;

layout(binding = 0, std430) readonly buffer StrokeFrom {
    vec2[] data;
} strokeFrom;

layout(binding = 0, std430) readonly buffer StrokeTo {
    vec2[] data;
} strokeTo;

void main() {
    gl_Position = vec4(vPosition, 1);
    oTexCoord = iTexCoord;
}
