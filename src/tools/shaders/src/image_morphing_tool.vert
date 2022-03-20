#version 450 core

layout(location = 0) in vec2 vPosition;
layout(location = 1) out vec2 oTexCoord;
layout(location = 2) out float oDepth;

layout(push_constant) uniform MorphingSettings {
    uint strokeElementsCount;
    float toolMagnitude;
    bool preserveBorders;
} morphingSettings;

layout(set = 0, binding = 0, std430) readonly buffer StrokeFrom {
    vec2[] points;
} strokeFrom;

layout(set = 0, binding = 1, std430) readonly buffer StrokeTo {
    vec2[] points;
} strokeTo;

const float PI = 3.14159265359f;
const float FLOAT_EPS = 0.001f;

float powerFunc(float value) {
    return cos(value * PI) / 2.0f + 0.5f;
}

vec2 uvToVertexSpace(vec2 value) {
    return value * 2.0 - 1.0;
}

bool isBorder(vec2 position) {
    if (abs(abs(position.x) - 1.0f) < FLOAT_EPS || abs(abs(position.y) - 1.0f) < FLOAT_EPS) {
        return true;
    }
    return false;
}

void main() {
    vec2 shift = vec2(0.0f, 0.0f);
    bool isAffected = false;
    if (!(morphingSettings.preserveBorders && isBorder(uvToVertexSpace(vPosition)))) {
        uint affectedCount = 0;
        for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
            vec2 fromPoint = strokeFrom.points[i];
            vec2 toPoint = strokeTo.points[i];

            float startPointDistance = distance(fromPoint, vPosition);
            if (startPointDistance < morphingSettings.toolMagnitude) {
                isAffected = true;
                ++affectedCount;
                float normalizedStrength = startPointDistance / morphingSettings.toolMagnitude;
                float moveWeight = powerFunc(normalizedStrength);
                shift += (toPoint - fromPoint) * moveWeight;
            }
        }
        if (isAffected) shift /= float(affectedCount);
    }
    vec2 position = uvToVertexSpace(vPosition + shift);

    gl_Position = vec4(position, isAffected ? 1.0f : 0.0f, 1.0f);
    oDepth = gl_Position.z;
    oTexCoord = vPosition;
}
