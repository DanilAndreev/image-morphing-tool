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
const float P_INFINITY = 1000000.0f;

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

const float fMorphParameterA = 0.01;
const float fMorphParameterB = 2.0f;

void main() {
    float totalWeight = 0.0f;
    vec2 total = vec2(0.0f);
    for (uint i = 0; i < morphingSettings.strokeElementsCount; ++i) {
        vec2 fromPoint = strokeFrom.points[i];
        vec2 toPoint = strokeTo.points[i];
        float dist = distance(fromPoint, toPoint);
        float weight = pow((fMorphParameterA + dist), -fMorphParameterB);
        totalWeight += weight;
        total += weight * vec2(toPoint - fromPoint);
    }
    if (totalWeight > FLOAT_EPS) {
        total = (1 / totalWeight) * total;
    }

    if (morphingSettings.toolMagnitude > FLOAT_EPS) {
        float distMin = P_INFINITY;
        for (uint i = 0; i <  morphingSettings.strokeElementsCount; ++i) {
            vec2 fromPoint = strokeFrom.points[i];
            vec2 toPoint = strokeTo.points[i];
            float dist = distance(fromPoint, vPosition);
            if (dist < distMin) {
                distMin = dist;
            }
            if (distMin < morphingSettings.toolMagnitude) {
                float weight = cos(distMin / morphingSettings.toolMagnitude * PI) * 0.5 + 0.5;
                total *= weight;
            } else {
                total = vec2(0.0f);
            }
        }
    }


    vec2 position = uvToVertexSpace(vPosition + total);
    float shiftLength = length(total);
    float zShift = shiftLength > 0 ? shiftLength / morphingSettings.toolMagnitude : 0.0f;

    gl_Position = vec4(position, 1 - clamp(zShift, 0.0f, 1.0f), 1.0f);
    oDepth = gl_Position.z;
    oTexCoord = vPosition;
}
