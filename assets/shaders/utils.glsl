const float PI              = 3.141592653589793;
const float TWO_PI          = 6.283185307179586;
const float ONE_OVER_PI     = 0.3183098861837907;
const float ONE_OVER_TWO_PI = 0.15915494309189535;
const float LN2             = 0.6931471805599453;
const float INF             = 1.0 / 0.0;

float Pack(vec4 v) {
    return uintBitsToFloat(packUnorm4x8(v));
}

vec4 Unpack(float v) {
    return unpackUnorm4x8(floatBitsToUint(v));
}

vec2 SampleSphericalMap(vec3 v)
{
    const vec2 uv = vec2(
    atan(v.z, v.x) * ONE_OVER_TWO_PI + 0.5,
    asin(v.y) * ONE_OVER_PI + 0.5
    );
    return uv;
}

vec4 RGBMToLinear(vec4 value) {
    return vec4(value.rgb * value.a * 6.0f, 1.0);
}
