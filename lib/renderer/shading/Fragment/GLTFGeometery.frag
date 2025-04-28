#version 330 core

in FS_IN {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in ;


struct Material {
  bool useTextureAlbedo;
  bool useTextureMetallicRoughness;
  bool useTextureNormal;
  bool useTextureAmbientOcclusion;
  bool useTextureEmissive;

  vec3 albedo;
  float metallic;
  float roughness;
  float ambientOcclusion;
  vec3 emissive;

  sampler2D textureAlbedo;
  sampler2D textureMetallicRoughness;
  sampler2D textureNormal;
  sampler2D textureAmbientOcclusion;
  sampler2D textureEmissive;
};
uniform Material material;


layout (location=0) out vec3 FragPosition;
layout (location=1) out vec3 FragNormal;
layout (location=2) out vec3 FragColor;
layout (location=3) out vec3 FragMaterial;       // roughness + metalic + ambient


void main() {
    vec2 TexCoords = fs_in.TexCoords;

    FragPosition = fs_in.FragPos;

    // normal
	FragNormal = fs_in.TBN[2]; // interpolated vertex normal
	if (material.useTextureNormal) {
		vec3 tangentNormal = texture(material.textureNormal, TexCoords).xyz * 2.0 - 1.0;
        FragNormal = normalize(fs_in.TBN * tangentNormal);
	}

    // albedo
    FragColor = material.albedo;
    if (material.useTextureAlbedo) {
		FragColor = texture(material.textureAlbedo, TexCoords).rgb;
	}

    // metallic/roughness
	float metallic = material.metallic;
	float roughness = material.roughness;
	if (material.useTextureMetallicRoughness) {
		vec3 metallicRoughness = texture(material.textureMetallicRoughness, TexCoords).rgb;
		metallic = metallicRoughness.b;
		roughness = metallicRoughness.g;
	}

    // ambient occlusion
    float ao = material.ambientOcclusion;
	if (material.useTextureAmbientOcclusion) {
		ao = texture(material.textureAmbientOcclusion, TexCoords).r;
	}

    FragMaterial = vec3(roughness,metallic,ao);


    // emissive
	vec3 emissive = material.emissive;
	if (material.useTextureEmissive) {
		emissive = texture(material.textureEmissive, TexCoords).rgb;
	}
    
}