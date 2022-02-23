#version 450 core

layout(location = 0) in vec2 vPosition;
layout(location = 1) out vec2 oTexCoord;

layout(push_constant) uniform MorphingSettings {
    uint strokeElementsCount;
} morphingSettings;

layout(set = 0, binding = 0, std430) readonly buffer StrokeFrom {
    vec2[] points;
} strokeFrom;

layout(set = 0, binding = 1, std430) readonly buffer StrokeTo {
    vec2[] points;
} strokeTo;

const float PI = 3.14159265359f;

//TODO: move to morphingSettings
const float toolMagnitude = 0.1f;

float powerFunc(float value) {
    return cos(value * PI) / 2.0f + 0.5f;
}

void main() {
    vec2 position = vPosition;
    vec2 shift = vec2(0.0f, 0.0f);
    bool isAffected = false;
    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
        vec2 fromPoint = strokeFrom.points[i];
        vec2 toPoint = strokeTo.points[i];

        float startPointDistance = distance(fromPoint, vPosition);
        if (startPointDistance < toolMagnitude) {
            isAffected = true;
            float normalizedStrength = startPointDistance / toolMagnitude;
            float moveWeight = powerFunc(normalizedStrength);
            shift += (toPoint - fromPoint) * moveWeight;
        }
    }
    position = (position + shift) * 2.0 - 1.0;
    gl_Position = vec4(position, isAffected ? 0.5f : 0.0f, 1.0f);
    oTexCoord = vPosition;
}
