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

void main() {
    vec2 position = vPosition * 2.0 - 1.0;

    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
        vec2 fromPoint = strokeFrom.points[i];
        vec2 toPoint = strokeTo.points[i];

        float dragDistance = distance(fromPoint, toPoint);
        float myDistance = distance(fromPoint, position);
        if (myDistance < dragDistance) {
            vec2 maxDistort = (toPoint - fromPoint) / 4.0f;
            float normalizedDistance = myDistance / dragDistance;
            float normalizedImpact = (cos(normalizedDistance*PI) + 1.0f) / 2.0f;
            position += maxDistort * normalizedImpact;
        }
    }

//    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
//        vec2 fromPoint = strokeFrom.points[i];
//        vec2 toPoint = strokeTo.points[i];
//        float myDistance = distance(fromPoint, position);
//        if (myDistance < 0.1) {
//            position = fromPoint;
//        }
//    }

    gl_Position = vec4(position, 0.0f, 1.0f);
    oTexCoord = vPosition;
}
