#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

layout(std140) uniform View {
    mat4 viewProjectionMatrix;
};
uniform mat4 model;
uniform mat3 normalMatrix;
uniform vec2 uvMultiplier = vec2(1.0);

out FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

void main() {
    vec4 skinnedPosition = vec4(aPosition, 1.0);
    vec3 skinnedNormal = aNormal;
    vec3 skinnedTangent = aTangent;
    vec3 skinnedBitangent = aBitangent;

    // Apply skinning only if bone influences exist
    mat4 boneTransform;
    if (boneIds[0] != -1) {
        boneTransform = weights[0] * finalBonesMatrices[boneIds[0]];
        for (int i = 1; i < MAX_BONE_INFLUENCE; i++) {
            if (boneIds[i] != -1)
                boneTransform += weights[i] * finalBonesMatrices[boneIds[i]];
        }
    }
    else {
        boneTransform = finalBonesMatrices[MAX_BONES - 1];      // Last Matrix Contains Node and Parent Nodes Transformation
    }
    mat3 normalTransform = transpose(inverse(mat3(boneTransform)));
    
    skinnedPosition = boneTransform * vec4(aPosition, 1.0);
    skinnedNormal = normalize(normalTransform * aNormal);
    skinnedTangent = normalize(normalTransform * aTangent);
    skinnedBitangent = normalize(normalTransform * aBitangent);

    // Transform to world space
    vs_out.FragPos = vec3(model * vec4(skinnedPosition.xyz,1.0));
    vs_out.TexCoords = aTexCoords * uvMultiplier;

    // Construct TBN matrix (orthonormal basis)
    vec3 T = normalize(normalMatrix * skinnedTangent);
    vec3 B = normalize(normalMatrix * skinnedBitangent);
    vec3 N = normalize(normalMatrix * skinnedNormal);

    vs_out.TBN = mat3(T, B, N);

    gl_Position = viewProjectionMatrix * vec4(vs_out.FragPos, 1.0);
}