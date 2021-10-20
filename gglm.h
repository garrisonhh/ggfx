#ifndef GGLM_H
#define GGLM_H

// this header is c11-only, this macro should works GCC (and maybe clang?)
#ifdef __STDC_VERSION__
#if __STDC_VERSION__ < 201112L
#error "gglm is only compatible with c11 onwards."
#endif
#endif

#include <math.h>
#include <stdbool.h>

#define GGLM_EPSILON 0.000001

#define GGLM_MAX(a, b) ((a) > (b) ? (a) : (b))
#define GGLM_MIN(a, b) ((a) < (b) ? (a) : (b))
#define GGLM_CLAMP(x, lo, hi) GGLM_MIN(GGLM_MAX(x, lo), hi)

#define v2_ZERO (v2){{0.0, 0.0}}
#define v3_ZERO (v3){{0.0, 0.0, 0.0}}
#define v4_ZERO (v4){{0.0, 0.0, 0.0, 0.0}}

// map(), any(), all() are super useful for expressive programming
#define v2_MAP(v, func) (v2){{func(v.x), func(v.y)}}
#define v3_MAP(v, func) (v3){{func(v.x), func(v.y), func(v.z)}}
#define v4_MAP(v, func) (v4){{func(v.x), func(v.y), func(v.z), func(v.w)}}

/*
a cool trick with any() and all() is to use conditionals, for example:
> v4_ANY(v, 0.0 >) // checks if zero is more than any of vector values
you can even combine them with functions (this is the v4_eq implementation!):
> v4_ALL(v4_sub(a, b), GGLM_EPSILON > fabsf)
*/
#define v2_ANY(v, func) (func(v.x) || func(v.y))
#define v3_ANY(v, func) (func(v.x) || func(v.y) || func(v.z))
#define v4_ANY(v, func) (func(v.x) || func(v.y) || func(v.z) || func(v.w))

#define v2_ALL(v, func) (func(v.x) && func(v.y))
#define v3_ALL(v, func) (func(v.x) && func(v.y) && func(v.z))
#define v4_ALL(v, func) (func(v.x) && func(v.y) && func(v.z) && func(v.w))

/*
these are great for printf's, expanding vectors into float arrays, or using
functions that weren't built for gglm:
> printf("v: %f %f\n", v2_EXPAND(v));
> float vectors[] = {v2_EXPAND(a), v3_EXPAND(b), v2_EXPAND(c)};
*/
#define v2_EXPAND(v) v.x, v.y
#define v3_EXPAND(v) v.x, v.y, v.z
#define v4_EXPAND(v) v.x, v.y, v.z, v.w

// --- macro-expansion generation below this point ---
typedef union v2 {
    struct {
        float x, y;
    };
    float ptr[2];
} v2;

typedef union v3 {
    struct {
        float x, y, z;
    };
    v2 xy;
    float ptr[3];
} v3;

typedef union v4 {
    struct {
        float x, y, z, w;
    };
    v2 xy;
    v3 xyz;
    float ptr[4];
} v4;

static inline bool gglm_eq(float a, float b) {
    return fabsf(b - a) < GGLM_EPSILON;
}

static inline v2 v2_(float x, float y) {
    return (v2){{x, y}};
}

static inline v2 v2_fill(float x) {
    return (v2){{x, x}};
}

static inline v2 v2_copy(float* p) {
    return (v2){{p[0], p[1]}};
}

static inline v2 v2_add(v2 a, v2 b) {
    return (v2){{a.x + b.x, a.y + b.y}};
}

static inline v2 v2_sub(v2 a, v2 b) {
    return (v2){{a.x - b.x, a.y - b.y}};
}

static inline v2 v2_mul(v2 a, v2 b) {
    return (v2){{a.x * b.x, a.y * b.y}};
}

static inline v2 v2_div(v2 a, v2 b) {
    return (v2){{a.x / b.x, a.y / b.y}};
}

static inline v2 v2_adds(v2 v, float s) {
    return (v2){{v.x + s, v.y + s}};
}

static inline v2 v2_subs(v2 v, float s) {
    return (v2){{v.x - s, v.y - s}};
}

static inline v2 v2_muls(v2 v, float s) {
    return (v2){{v.x * s, v.y * s}};
}

static inline v2 v2_divs(v2 v, float s) {
    return (v2){{v.x / s, v.y / s}};
}

static inline v2 v2_negate(v2 v) {
    return (v2){{-v.x, -v.y}};
}

static inline float v2_dot(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline float v2_norm2(v2 v) {
    return v2_dot(v, v);
}

static inline float v2_norm(v2 v) {
    return sqrtf(v2_norm2(v));
}

static inline float v2_dist2(v2 a, v2 b) {
    return v2_norm2(v2_sub(b, a));
}

static inline float v2_dist(v2 a, v2 b) {
    return sqrtf(v2_dist2(a, b));
}

static inline v2 v2_normalize(v2 v) {
    return v2_divs(v, v2_norm(v));
}

static inline v2 v2_lerp(v2 a, v2 b, float t) {
    return v2_add(a, v2_muls(v2_sub(b, a), t));
}

static inline v2 v2_upper_bound(v2 a, v2 b) {
    return (v2){{GGLM_MAX(a.x, b.x), GGLM_MAX(a.y, b.y)}};
}

static inline v2 v2_lower_bound(v2 a, v2 b) {
    return (v2){{GGLM_MIN(a.x, b.x), GGLM_MIN(a.y, b.y)}};
}

static inline v2 v2_clamp(v2 v, v2 lower, v2 upper) {
    return (v2){
        {GGLM_CLAMP(v.x, lower.x, upper.x), GGLM_CLAMP(v.y, lower.y, upper.y)}};
}

static inline bool v2_eq(v2 a, v2 b) {
    return v2_ALL(v2_sub(a, b), GGLM_EPSILON > fabsf);
}

static inline v3 v3_(float x, float y, float z) {
    return (v3){{x, y, z}};
}

static inline v3 v3_fill(float x) {
    return (v3){{x, x, x}};
}

static inline v3 v3_copy(float* p) {
    return (v3){{p[0], p[1], p[2]}};
}

static inline v3 v3_add(v3 a, v3 b) {
    return (v3){{a.x + b.x, a.y + b.y, a.z + b.z}};
}

static inline v3 v3_sub(v3 a, v3 b) {
    return (v3){{a.x - b.x, a.y - b.y, a.z - b.z}};
}

static inline v3 v3_mul(v3 a, v3 b) {
    return (v3){{a.x * b.x, a.y * b.y, a.z * b.z}};
}

static inline v3 v3_div(v3 a, v3 b) {
    return (v3){{a.x / b.x, a.y / b.y, a.z / b.z}};
}

static inline v3 v3_adds(v3 v, float s) {
    return (v3){{v.x + s, v.y + s, v.z + s}};
}

static inline v3 v3_subs(v3 v, float s) {
    return (v3){{v.x - s, v.y - s, v.z - s}};
}

static inline v3 v3_muls(v3 v, float s) {
    return (v3){{v.x * s, v.y * s, v.z * s}};
}

static inline v3 v3_divs(v3 v, float s) {
    return (v3){{v.x / s, v.y / s, v.z / s}};
}

static inline v3 v3_negate(v3 v) {
    return (v3){{-v.x, -v.y, -v.z}};
}

static inline float v3_dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float v3_norm2(v3 v) {
    return v3_dot(v, v);
}

static inline float v3_norm(v3 v) {
    return sqrtf(v3_norm2(v));
}

static inline float v3_dist2(v3 a, v3 b) {
    return v3_norm2(v3_sub(b, a));
}

static inline float v3_dist(v3 a, v3 b) {
    return sqrtf(v3_dist2(a, b));
}

static inline v3 v3_normalize(v3 v) {
    return v3_divs(v, v3_norm(v));
}

static inline v3 v3_lerp(v3 a, v3 b, float t) {
    return v3_add(a, v3_muls(v3_sub(b, a), t));
}

static inline v3 v3_upper_bound(v3 a, v3 b) {
    return (v3){{GGLM_MAX(a.x, b.x), GGLM_MAX(a.y, b.y), GGLM_MAX(a.z, b.z)}};
}

static inline v3 v3_lower_bound(v3 a, v3 b) {
    return (v3){{GGLM_MIN(a.x, b.x), GGLM_MIN(a.y, b.y), GGLM_MIN(a.z, b.z)}};
}

static inline v3 v3_clamp(v3 v, v3 lower, v3 upper) {
    return (v3){{GGLM_CLAMP(v.x, lower.x, upper.x),
                 GGLM_CLAMP(v.y, lower.y, upper.y),
                 GGLM_CLAMP(v.z, lower.z, upper.z)}};
}

static inline bool v3_eq(v3 a, v3 b) {
    return v3_ALL(v3_sub(a, b), GGLM_EPSILON > fabsf);
}

static inline v4 v4_(float x, float y, float z, float w) {
    return (v4){{x, y, z, w}};
}

static inline v4 v4_fill(float x) {
    return (v4){{x, x, x, x}};
}

static inline v4 v4_copy(float* p) {
    return (v4){{p[0], p[1], p[2], p[3]}};
}

static inline v4 v4_add(v4 a, v4 b) {
    return (v4){{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}};
}

static inline v4 v4_sub(v4 a, v4 b) {
    return (v4){{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}};
}

static inline v4 v4_mul(v4 a, v4 b) {
    return (v4){{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}};
}

static inline v4 v4_div(v4 a, v4 b) {
    return (v4){{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}};
}

static inline v4 v4_adds(v4 v, float s) {
    return (v4){{v.x + s, v.y + s, v.z + s, v.w + s}};
}

static inline v4 v4_subs(v4 v, float s) {
    return (v4){{v.x - s, v.y - s, v.z - s, v.w - s}};
}

static inline v4 v4_muls(v4 v, float s) {
    return (v4){{v.x * s, v.y * s, v.z * s, v.w * s}};
}

static inline v4 v4_divs(v4 v, float s) {
    return (v4){{v.x / s, v.y / s, v.z / s, v.w / s}};
}

static inline v4 v4_negate(v4 v) {
    return (v4){{-v.x, -v.y, -v.z, -v.w}};
}

static inline float v4_dot(v4 a, v4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float v4_norm2(v4 v) {
    return v4_dot(v, v);
}

static inline float v4_norm(v4 v) {
    return sqrtf(v4_norm2(v));
}

static inline float v4_dist2(v4 a, v4 b) {
    return v4_norm2(v4_sub(b, a));
}

static inline float v4_dist(v4 a, v4 b) {
    return sqrtf(v4_dist2(a, b));
}

static inline v4 v4_normalize(v4 v) {
    return v4_divs(v, v4_norm(v));
}

static inline v4 v4_lerp(v4 a, v4 b, float t) {
    return v4_add(a, v4_muls(v4_sub(b, a), t));
}

static inline v4 v4_upper_bound(v4 a, v4 b) {
    return (v4){{GGLM_MAX(a.x, b.x), GGLM_MAX(a.y, b.y), GGLM_MAX(a.z, b.z),
                 GGLM_MAX(a.w, b.w)}};
}

static inline v4 v4_lower_bound(v4 a, v4 b) {
    return (v4){{GGLM_MIN(a.x, b.x), GGLM_MIN(a.y, b.y), GGLM_MIN(a.z, b.z),
                 GGLM_MIN(a.w, b.w)}};
}

static inline v4 v4_clamp(v4 v, v4 lower, v4 upper) {
    return (v4){
        {GGLM_CLAMP(v.x, lower.x, upper.x), GGLM_CLAMP(v.y, lower.y, upper.y),
         GGLM_CLAMP(v.z, lower.z, upper.z), GGLM_CLAMP(v.w, lower.w, upper.w)}};
}

static inline bool v4_eq(v4 a, v4 b) {
    return v4_ALL(v4_sub(a, b), GGLM_EPSILON > fabsf);
}

#endif
