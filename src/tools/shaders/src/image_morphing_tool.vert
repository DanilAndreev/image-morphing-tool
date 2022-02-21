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
    return value;
}

void main() {
//    vec2 position = vPosition;
//    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
//        vec2 fromPoint = strokeFrom.points[i];
//        vec2 toPoint = strokeTo.points[i];
//
//        float dragDistance = distance(fromPoint, toPoint);
//        float myDistance = distance(fromPoint, vPosition);
//        if (myDistance < dragDistance) {
//            vec2 maxDistort = (toPoint - fromPoint) / 4.0f;
//            float normalizedDistance = myDistance / dragDistance;
//            float normalizedImpact = (cos(normalizedDistance*PI) + 1.0f) / 2.0f;
//            position += maxDistort * normalizedImpact;
//        }
//    }

    vec2 position = vPosition;
    vec2 shift = vec2(0.0f, 0.0f);
    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
        vec2 fromPoint = strokeFrom.points[i];
        vec2 toPoint = strokeTo.points[i];

        float moveDistance = distance(fromPoint, toPoint);
        float affectedPointDistance = distance(fromPoint, vPosition);
        if (affectedPointDistance < toolMagnitude) {
            float moveWeight = powerFunc(moveDistance / toolMagnitude);
            shift += (toPoint - fromPoint) * moveWeight;
            position += shift;
        }
    }
    position = position * 2.0 - 1.0;
    gl_Position = vec4(position, 0.0f, 1.0f);
    oTexCoord = vPosition;
}
