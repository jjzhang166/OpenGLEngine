#ifndef _IBL_
#define _IBL_

/*
 * IBL 使用到的核心函数
 */

const float PI = 3.14159265359;

// https://huailiang.github.io/blog/2019/ibl2/
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//对于上面的公式，某些OpenGL版本(如webGL)不支持位运算，可以用下述代码：
float VanDerCorpus(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom = 1.0;
    float result = 0.0;
    for(uint i = 0u; i < 32u; ++i)
    {
        if(n > 0u)
        {
            denom = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n = uint(float(n) / 2.0);
        }
    }
    return result;
}

// 构建Hammersley序列
// i 是序列 n是采样数量
// 参考： https://huailiang.github.io/blog/2019/ibl2/
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

/*
 * ImportanceSample
 *  流程：Hammersley序列 ->球面坐标系 -> 笛卡尔坐标系 -> 世界坐标系
 *  Xi 是Hammersley序列值
 *  N是法线， roughness是粗糙
 */
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
    
    // from spherical coordinates to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    // from tangent-space H vector to world-space sample vector
    vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}


/*
 * 正太分布 NDF
 */
float DistributionGGXIBL(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}


/*
 * 微面元遮挡函数 indirect-light IBL
 * use a different k for IBL
 */
float GeometrySchlickGGXIBL(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}


float GeometrySmithIBL(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGXIBL(NdotV, roughness);
    float ggx1 = GeometrySchlickGGXIBL(NdotL, roughness);
    
    return ggx1 * ggx2;
}


/*
 * 菲涅尔函数
 */
vec3 FresnelSchlickIBL(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughnessIBL(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

/*
 * IBL 近似求解ambient的diffuse和specular的多项式参数
 */
void IndirectK(vec3 normal, vec3 view, float matel, vec3 albedo, out vec3 kS, out vec3 kD, out vec3 F0)
{
    vec3 N = normalize(normal);
    vec3 V = normalize(view);
    F0 = vec3(0.04);
    F0 = mix(F0, albedo, matel);
    kS  = FresnelSchlickIBL(clamp(dot(N, V), 0.0, 1.0), F0);
    kD = vec3(1.0) - kS;
}


#endif  // _IBL_
