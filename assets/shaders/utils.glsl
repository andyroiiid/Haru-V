const float PI  = 3.141592653589793;

float pack(vec4 v) {
    return uintBitsToFloat(packUnorm4x8(v));
}

vec4 unpack(float v) {
    return unpackUnorm4x8(floatBitsToUint(v));
}
