#version 330 core

const int MAX_BONES = 100;
const int BONE_ARRAY_NUM = 3;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 boneIds[BONE_ARRAY_NUM];
layout(location = 8) in vec4 weights[BONE_ARRAY_NUM];

uniform mat4 finalBonesMatrices[MAX_BONES];

layout(std140) uniform View {
    mat4 viewProjectionMatrix;
};

uniform mat4 model;
uniform mat4 fallbackMatrix;
uniform mat3 normalMatrix;
uniform vec2 uvMultiplier = vec2(1.0);
uniform bool rigged;

out FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

void main() {
    mat4 finalModelMatrix = model;
    mat3 finalNormalMatrix = normalMatrix;

    if (rigged) {
        mat4 skinMatrix = mat4(0.0);

        // Accumulate bone transforms weighted by influence
        for (int i = 0 ; i<BONE_ARRAY_NUM ;++i) {
            for (int j = 0; j < 4; ++j) {
                int boneIndex = boneIds[i][j];
                float weight = weights[i][j];

                if (boneIndex >= 0 && weight > 0.0) {
                    skinMatrix += weight * finalBonesMatrices[boneIndex];
                }
            }
        }

        // Fallback to base transform if no valid bone weights
        if (skinMatrix == mat4(0.0)) {
            skinMatrix = fallbackMatrix;
        }

        finalModelMatrix = finalModelMatrix * skinMatrix;
        finalNormalMatrix = finalNormalMatrix * transpose(inverse(mat3(skinMatrix)));
    }

    // Pass world-space values to fragment shader
    vs_out.FragPos = vec3(finalModelMatrix * vec4(aPosition, 1.0));
    vs_out.TexCoords = aTexCoords * uvMultiplier;

    // Construct tangent-space matrix in world space
    vec3 Normal = normalize(finalNormalMatrix * aNormal);
    vec3 Tangent = normalize(finalNormalMatrix * aTangent);
    vec3 Bitangent = normalize(finalNormalMatrix * aBitangent);
    vs_out.TBN = mat3(Tangent,Bitangent,Normal);

    gl_Position = viewProjectionMatrix * vec4(vs_out.FragPos, 1.0);
}
