const float PI              = 3.141592653589793;
const float TWO_PI          = 6.283185307179586;
const float ONE_OVER_PI     = 0.3183098861837907;
const float ONE_OVER_TWO_PI = 0.15915494309189535;

float Pack(vec4 v) {
    return uintBitsToFloat(packUnorm4x8(v));
}

vec4 Unpack(float v) {
    return unpackUnorm4x8(floatBitsToUint(v));
}

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    const vec2 INV_ATAN = vec2(ONE_OVER_TWO_PI, ONE_OVER_PI);
    uv *= INV_ATAN;
    uv += 0.5;
    return uv;
}
