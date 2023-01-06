#ifndef PBR_GLSL
#define PBR_GLSL

// PBR functions from https://learnopengl.com/PBR/Lighting

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PBR(vec3 lightDirection, vec3 lightColor, vec3 V, vec3 N, float roughness, float metallic, vec3 albedo, vec3 F0)
{
    const vec3 L = normalize(lightDirection);
    const vec3 H = normalize(V + L);
    const float NdotL = max(dot(N, L), 0.0);
    const float NdotV = max(dot(N, V), 0.0);

    const float NDF = DistributionGGX(N, H, roughness);
    const float G = GeometrySmith(N, V, L, roughness);
    const vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    const vec3 kS = F;
    const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    const vec3 numerator = NDF * G * F;
    const float denominator = 4.0 * NdotV * NdotL + 0.0001;
    const vec3 specular = numerator / denominator;

    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

// from Real Shading in Unreal Engine 4 by Brian Karis, Epic Games
float PointLightFalloff(float distance, float radius) {
    float A = distance / radius;
    A *= A;
    A *= A;

    A = clamp(1.0 - A, 0.0, 1.0);
    A *= A;

    float B = distance * distance + 1;
    return A / B;
}

#endif
