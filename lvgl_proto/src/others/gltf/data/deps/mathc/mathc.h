/*
Copyright © 2018 Felipe Ferreira da Silva

This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a
     product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef MATHC_H
#define MATHC_H

#include <stdbool.h>
#include <math.h>

#define MATHC_VERSION_YYYY 2019
#define MATHC_VERSION_MM 02
#define MATHC_VERSION_DD 16
#define MATHC_VERSION_MICRO 0

#if !defined(MATHC_NO_INT)
#define MATHC_USE_INT
#endif
#if !defined(MATHC_NO_FLOATING_POINT)
#define MATHC_USE_FLOATING_POINT
#endif
#if !defined(MATHC_NO_POINTER_STRUCT_FUNCTIONS)
#define MATHC_USE_POINTER_STRUCT_FUNCTIONS
#endif
#if !defined(MATHC_NO_STRUCT_FUNCTIONS)
#define MATHC_USE_STRUCT_FUNCTIONS
#endif
#if !defined(MATHC_NO_EASING_FUNCTIONS)
#define MATHC_USE_EASING_FUNCTIONS
#endif

#if defined(MATHC_USE_INT)
#include <stdint.h>
#endif
#if defined(MATHC_USE_FLOATING_POINT)
#include <float.h>
#endif

#define VEC2_SIZE 2
#define VEC3_SIZE 3
#define VEC4_SIZE 4
#define QUAT_SIZE 4
#define MAT2_SIZE 4
#define MAT3_SIZE 9
#define MAT4_SIZE 16

#if defined(MATHC_USE_INT)
#if defined(MATHC_INT_TYPE)
typedef MATHC_INT_TYPE mint_t;
#endif
#if !defined(MATHC_USE_INT8) && !defined(MATHC_USE_INT16) && !defined(MATHC_USE_INT32) && !defined(MATHC_USE_INT64)
#define MATHC_USE_INT32
#endif
#if defined(MATHC_USE_INT8)
#if !defined(MATHC_INT_TYPE)
typedef int8_t mint_t;
#endif
#define MINT_MAX INT8_MAX
#define MINT_MIN INT8_MIN
#endif
#if defined(MATHC_USE_INT16)
#if !defined(MATHC_INT_TYPE)
typedef int16_t mint_t;
#endif
#define MINT_MAX INT16_MAX
#define MINT_MIN INT16_MIN
#endif
#if defined(MATHC_USE_INT32)
#if !defined(MATHC_INT_TYPE)
typedef int32_t mint_t;
#endif
#define MINT_MAX INT32_MAX
#define MINT_MIN INT32_MIN
#endif
#if defined(MATHC_USE_INT64)
#if !defined(MATHC_INT_TYPE)
typedef int64_t mint_t;
#endif
#define MINT_MAX INT64_MAX
#define MINT_MIN INT64_MIN
#endif
#endif

#if defined(MATHC_USE_FLOATING_POINT)
#if defined(MATHC_FLOATING_POINT_TYPE)
typedef MATHC_FLOATING_POINT_TYPE mfloat_t;
#endif
#if !defined(MATHC_USE_SINGLE_FLOATING_POINT) && !defined(MATHC_USE_DOUBLE_FLOATING_POINT)
#define MATHC_USE_SINGLE_FLOATING_POINT
#endif
#if defined(MATHC_USE_SINGLE_FLOATING_POINT)
#if !defined(MATHC_FLOATING_POINT_TYPE)
typedef float mfloat_t;
#endif
#define MPI 3.1415926536f
#define MPI_2 1.5707963268f
#define MPI_4 0.7853981634f
#define MFLT_EPSILON FLT_EPSILON
#define MFABS fabsf
#define MFMIN fminf
#define MFMAX fmaxf
#define MSQRT sqrtf
#define MSIN sinf
#define MCOS cosf
#define MACOS acosf
#define MASIN asinf
#define MTAN tanf
#define MATAN2 atan2f
#define MPOW powf
#define MFLOOR floorf
#define MCEIL ceilf
#define MROUND roundf
#define MFLOAT_C(c) c ## f
#endif
#if defined(MATHC_USE_DOUBLE_FLOATING_POINT)
#if !defined(MATHC_FLOATING_POINT_TYPE)
typedef double mfloat_t;
#endif
#define MPI 3.14159265358979323846
#define MPI_2 1.57079632679489661923
#define MPI_4 0.78539816339744830962
#define MFLT_EPSILON DBL_EPSILON
#define MFABS fabs
#define MFMIN fmin
#define MFMAX fmax
#define MSQRT sqrt
#define MSIN sin
#define MCOS cos
#define MACOS acos
#define MASIN asin
#define MTAN tan
#define MATAN2 atan2
#define MPOW pow
#define MFLOOR floor
#define MCEIL ceil
#define MROUND round
#define MFLOAT_C(c) c
#endif
#endif

#if defined(MATHC_USE_STRUCT_FUNCTIONS) || defined(MATHC_USE_POINTER_STRUCT_FUNCTIONS)
#if defined(MATHC_USE_INT)
struct vec2i_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mint_t x;
			mint_t y;
		};
		mint_t v[VEC2_SIZE];
	};
#else
	mint_t x;
	mint_t y;
#endif
};

struct vec3i_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mint_t x;
			mint_t y;
			mint_t z;
		};
		mint_t v[VEC3_SIZE];
	};
#else
	mint_t x;
	mint_t y;
	mint_t z;
#endif
};

struct vec4i_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mint_t x;
			mint_t y;
			mint_t z;
			mint_t w;
		};
		mint_t v[VEC4_SIZE];
	};
#else
	mint_t x;
	mint_t y;
	mint_t z;
	mint_t w;
#endif
};
#endif

#if defined(MATHC_USE_FLOATING_POINT)
struct vec2_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t x;
			mfloat_t y;
		};
		mfloat_t v[VEC2_SIZE];
	};
#else
	mfloat_t x;
	mfloat_t y;
#endif
};

struct vec3_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t x;
			mfloat_t y;
			mfloat_t z;
		};
		mfloat_t v[VEC3_SIZE];
	};
#else
	mfloat_t x;
	mfloat_t y;
	mfloat_t z;
#endif
};

struct vec4_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t x;
			mfloat_t y;
			mfloat_t z;
			mfloat_t w;
		};
		mfloat_t v[VEC4_SIZE];
	};
#else
	mfloat_t x;
	mfloat_t y;
	mfloat_t z;
	mfloat_t w;
#endif
};

struct quat_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t x;
			mfloat_t y;
			mfloat_t z;
			mfloat_t w;
		};
		mfloat_t v[QUAT_SIZE];
	};
#else
	mfloat_t x;
	mfloat_t y;
	mfloat_t z;
	mfloat_t w;
#endif
};

/*
Matrix 2×2 representation:
0/m11 2/m12
1/m21 3/m22
*/
struct mat2_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t m11;
			mfloat_t m21;
			mfloat_t m12;
			mfloat_t m22;
		};
		mfloat_t v[MAT2_SIZE];
	};
#else
	mfloat_t m11;
	mfloat_t m21;
	mfloat_t m12;
	mfloat_t m22;
#endif
};

/*
Matrix 3×3 representation:
0/m11 3/m12 6/m13
1/m21 4/m22 7/m23
2/m31 5/m32 8/m33
*/
struct mat3_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t m11;
			mfloat_t m21;
			mfloat_t m31;
			mfloat_t m12;
			mfloat_t m22;
			mfloat_t m32;
			mfloat_t m13;
			mfloat_t m23;
			mfloat_t m33;
		};
		mfloat_t v[MAT3_SIZE];
	};
#else
	mfloat_t m11;
	mfloat_t m21;
	mfloat_t m31;
	mfloat_t m12;
	mfloat_t m22;
	mfloat_t m32;
	mfloat_t m13;
	mfloat_t m23;
	mfloat_t m33;
#endif
};

/*
Matrix 4×4 representation:
0/m11 4/m12  8/m13 12/m14
1/m21 5/m22  9/m23 13/m24
2/m31 6/m32 10/m33 14/m34
3/m41 7/m42 11/m43 15/m44
*/
struct mat4_st {
#if defined(MATHC_USE_UNIONS)
	union {
		struct {
			mfloat_t m11;
			mfloat_t m21;
			mfloat_t m31;
			mfloat_t m41;
			mfloat_t m12;
			mfloat_t m22;
			mfloat_t m32;
			mfloat_t m42;
			mfloat_t m13;
			mfloat_t m23;
			mfloat_t m33;
			mfloat_t m43;
			mfloat_t m14;
			mfloat_t m24;
			mfloat_t m34;
			mfloat_t m44;
		};
		mfloat_t v[MAT4_SIZE];
	};
#else
	mfloat_t m11;
	mfloat_t m21;
	mfloat_t m31;
	mfloat_t m41;
	mfloat_t m12;
	mfloat_t m22;
	mfloat_t m32;
	mfloat_t m42;
	mfloat_t m13;
	mfloat_t m23;
	mfloat_t m33;
	mfloat_t m43;
	mfloat_t m14;
	mfloat_t m24;
	mfloat_t m34;
	mfloat_t m44;
#endif
};
#endif
#endif

#if defined(MATHC_USE_INT)
mint_t clampi(mint_t value, mint_t min, mint_t max);
#endif

#if defined(MATHC_USE_FLOATING_POINT)
#define MRADIANS(degrees) (degrees * MPI / MFLOAT_C(180.0))
#define MDEGREES(radians) (radians * MFLOAT_C(180.0) / MPI)
bool nearly_equal(mfloat_t a, mfloat_t b, mfloat_t epsilon);
mfloat_t to_radians(mfloat_t degrees);
mfloat_t to_degrees(mfloat_t radians);
mfloat_t clampf(mfloat_t value, mfloat_t min, mfloat_t max);
#endif

#if defined(MATHC_USE_INT)
bool vec2i_is_zero(mint_t *v0);
bool vec2i_is_equal(mint_t *v0, mint_t *v1);
mint_t *vec2i(mint_t *result, mint_t x, mint_t y);
mint_t *vec2i_assign(mint_t *result, mint_t *v0);
#if defined(MATHC_USE_FLOATING_POINT)
mint_t *vec2i_assign_vec2(mint_t *result, mfloat_t *v0);
#endif
mint_t *vec2i_zero(mint_t *result);
mint_t *vec2i_one(mint_t *result);
mint_t *vec2i_sign(mint_t *result, mint_t *v0);
mint_t *vec2i_add(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_add_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec2i_subtract(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_subtract_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec2i_multiply(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_multiply_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec2i_divide(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_divide_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec2i_snap(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_snap_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec2i_negative(mint_t *result, mint_t *v0);
mint_t *vec2i_abs(mint_t *result, mint_t *v0);
mint_t *vec2i_max(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_min(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec2i_clamp(mint_t *result, mint_t *v0, mint_t *v1, mint_t *v2);
mint_t *vec2i_tangent(mint_t *result, mint_t *v0);
bool vec3i_is_zero(mint_t *v0);
bool vec3i_is_equal(mint_t *v0, mint_t *v1);
mint_t *vec3i(mint_t *result, mint_t x, mint_t y, mint_t z);
mint_t *vec3i_assign(mint_t *result, mint_t *v0);
#if defined(MATHC_USE_FLOATING_POINT)
mint_t *vec3i_assign_vec3(mint_t *result, mfloat_t *v0);
#endif
mint_t *vec3i_zero(mint_t *result);
mint_t *vec3i_one(mint_t *result);
mint_t *vec3i_sign(mint_t *result, mint_t *v0);
mint_t *vec3i_add(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_add_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec3i_subtract(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_subtract_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec3i_multiply(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_multiply_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec3i_divide(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_divide_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec3i_snap(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_snap_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec3i_cross(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_negative(mint_t *result, mint_t *v0);
mint_t *vec3i_abs(mint_t *result, mint_t *v0);
mint_t *vec3i_max(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_min(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec3i_clamp(mint_t *result, mint_t *v0, mint_t *v1, mint_t *v2);
bool vec4i_is_zero(mint_t *v0);
bool vec4i_is_equal(mint_t *v0, mint_t *v1);
mint_t *vec4i(mint_t *result, mint_t x, mint_t y, mint_t z, mint_t w);
mint_t *vec4i_assign(mint_t *result, mint_t *v0);
#if defined(MATHC_USE_FLOATING_POINT)
mint_t *vec4i_assign_vec4(mint_t *result, mfloat_t *v0);
#endif
mint_t *vec4i_zero(mint_t *result);
mint_t *vec4i_one(mint_t *result);
mint_t *vec4i_sign(mint_t *result, mint_t *v0);
mint_t *vec4i_add(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_add_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec4i_subtract(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_subtract_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec4i_multiply(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_multiply_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec4i_divide(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_divide_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec4i_snap(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_snap_i(mint_t *result, mint_t *v0, mint_t i);
mint_t *vec4i_negative(mint_t *result, mint_t *v0);
mint_t *vec4i_abs(mint_t *result, mint_t *v0);
mint_t *vec4i_max(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_min(mint_t *result, mint_t *v0, mint_t *v1);
mint_t *vec4i_clamp(mint_t *result, mint_t *v0, mint_t *v1, mint_t *v2);
#endif
#if defined(MATHC_USE_FLOATING_POINT)
bool vec2_is_zero(mfloat_t *v0);
bool vec2_is_equal(mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2(mfloat_t *result, mfloat_t x, mfloat_t y);
mfloat_t *vec2_assign(mfloat_t *result, mfloat_t *v0);
#if defined(MATHC_USE_INT)
mfloat_t *vec2_assign_vec2i(mfloat_t *result, mint_t *v0);
#endif
mfloat_t *vec2_zero(mfloat_t *result);
mfloat_t *vec2_one(mfloat_t *result);
mfloat_t *vec2_sign(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_add(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_add_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_subtract(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_subtract_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_multiply(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_multiply_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_multiply_mat2(mfloat_t *result, mfloat_t *v0, mfloat_t *m0);
mfloat_t *vec2_divide(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_divide_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_snap(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_snap_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_negative(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_abs(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_floor(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_ceil(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_round(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_max(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_min(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_clamp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2);
mfloat_t *vec2_normalize(mfloat_t *result, mfloat_t *v0);
mfloat_t vec2_dot(mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_project(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec2_slide(mfloat_t *result, mfloat_t *v0, mfloat_t *normal);
mfloat_t *vec2_reflect(mfloat_t *result, mfloat_t *v0, mfloat_t *normal);
mfloat_t *vec2_tangent(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec2_rotate(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec2_lerp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t f);
mfloat_t *vec2_bezier3(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2, mfloat_t f);
mfloat_t *vec2_bezier4(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2, mfloat_t *v3, mfloat_t f);
mfloat_t vec2_angle(mfloat_t *v0);
mfloat_t vec2_length(mfloat_t *v0);
mfloat_t vec2_length_squared(mfloat_t *v0);
mfloat_t vec2_distance(mfloat_t *v0, mfloat_t *v1);
mfloat_t vec2_distance_squared(mfloat_t *v0, mfloat_t *v1);
bool vec2_linear_independent(mfloat_t *v0, mfloat_t *v1);
mfloat_t** vec2_orthonormalization(mfloat_t result[2][2], mfloat_t basis[2][2]);
bool vec3_is_zero(mfloat_t *v0);
bool vec3_is_equal(mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3(mfloat_t *result, mfloat_t x, mfloat_t y, mfloat_t z);
mfloat_t *vec3_assign(mfloat_t *result, mfloat_t *v0);
#if defined(MATHC_USE_INT)
mfloat_t *vec3_assign_vec3i(mfloat_t *result, mint_t *v0);
#endif
mfloat_t *vec3_zero(mfloat_t *result);
mfloat_t *vec3_one(mfloat_t *result);
mfloat_t *vec3_sign(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_add(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_add_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec3_subtract(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_subtract_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec3_multiply(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_multiply_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec3_multiply_mat3(mfloat_t *result, mfloat_t *v0, mfloat_t *m0);
mfloat_t *vec3_divide(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_divide_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec3_snap(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_snap_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec3_negative(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_abs(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_floor(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_ceil(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_round(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec3_max(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_min(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_clamp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2);
mfloat_t *vec3_cross(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_normalize(mfloat_t *result, mfloat_t *v0);
mfloat_t vec3_dot(mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_project(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec3_slide(mfloat_t *result, mfloat_t *v0, mfloat_t *normal);
mfloat_t *vec3_reflect(mfloat_t *result, mfloat_t *v0, mfloat_t *normal);
mfloat_t *vec3_rotate(mfloat_t *result, mfloat_t *v0, mfloat_t *ra, mfloat_t f);
mfloat_t *vec3_lerp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t f);
mfloat_t *vec3_bezier3(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2, mfloat_t f);
mfloat_t *vec3_bezier4(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2, mfloat_t *v3, mfloat_t f);
mfloat_t vec3_length(mfloat_t *v0);
mfloat_t vec3_length_squared(mfloat_t *v0);
mfloat_t vec3_distance(mfloat_t *v0, mfloat_t *v1);
mfloat_t vec3_distance_squared(mfloat_t *v0, mfloat_t *v1);
bool vec3_linear_independent(mfloat_t *v0, mfloat_t *v1, mfloat_t *v2);
mfloat_t** vec3_orthonormalization(mfloat_t result[3][3], mfloat_t basis[3][3]);
bool vec4_is_zero(mfloat_t *v0);
bool vec4_is_equal(mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4(mfloat_t *result, mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
mfloat_t *vec4_assign(mfloat_t *result, mfloat_t *v0);
#if defined(MATHC_USE_INT)
mfloat_t *vec4_assign_vec4i(mfloat_t *result, mint_t *v0);
#endif
mfloat_t *vec4_zero(mfloat_t *result);
mfloat_t *vec4_one(mfloat_t *result);
mfloat_t *vec4_sign(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_add(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_add_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec4_subtract(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_subtract_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec4_multiply(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_multiply_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec4_multiply_mat4(mfloat_t *result, mfloat_t *v0, mfloat_t *m0);
mfloat_t *vec4_divide(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_divide_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec4_snap(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_snap_f(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *vec4_negative(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_abs(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_floor(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_ceil(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_round(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_max(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_min(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *vec4_clamp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t *v2);
mfloat_t *vec4_normalize(mfloat_t *result, mfloat_t *v0);
mfloat_t *vec4_lerp(mfloat_t *result, mfloat_t *v0, mfloat_t *v1, mfloat_t f);
bool quat_is_zero(mfloat_t *q0);
bool quat_is_equal(mfloat_t *q0, mfloat_t *q1);
mfloat_t *quat(mfloat_t *result, mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
mfloat_t *quat_assign(mfloat_t *result, mfloat_t *q0);
mfloat_t *quat_zero(mfloat_t *result);
mfloat_t *quat_null(mfloat_t *result);
mfloat_t *quat_multiply(mfloat_t *result, mfloat_t *q0, mfloat_t *q1);
mfloat_t *quat_multiply_f(mfloat_t *result, mfloat_t *q0, mfloat_t f);
mfloat_t *quat_divide(mfloat_t *result, mfloat_t *q0, mfloat_t *q1);
mfloat_t *quat_divide_f(mfloat_t *result, mfloat_t *q0, mfloat_t f);
mfloat_t *quat_negative(mfloat_t *result, mfloat_t *q0);
mfloat_t *quat_conjugate(mfloat_t *result, mfloat_t *q0);
mfloat_t *quat_inverse(mfloat_t *result, mfloat_t *q0);
mfloat_t *quat_normalize(mfloat_t *result, mfloat_t *q0);
mfloat_t quat_dot(mfloat_t *q0, mfloat_t *q1);
mfloat_t *quat_power(mfloat_t *result, mfloat_t *q0, mfloat_t exponent);
mfloat_t *quat_from_axis_angle(mfloat_t *result, mfloat_t *v0, mfloat_t angle);
mfloat_t *quat_from_vec3(mfloat_t *result, mfloat_t *v0, mfloat_t *v1);
mfloat_t *quat_from_mat4(mfloat_t *result, mfloat_t *m0);
mfloat_t *quat_lerp(mfloat_t *result, mfloat_t *q0, mfloat_t *q1, mfloat_t f);
mfloat_t *quat_slerp(mfloat_t *result, mfloat_t *q0, mfloat_t *q1, mfloat_t f);
mfloat_t quat_length(mfloat_t *q0);
mfloat_t quat_length_squared(mfloat_t *q0);
mfloat_t quat_angle(mfloat_t *q0, mfloat_t *q1);
mfloat_t *mat2(mfloat_t *result, mfloat_t m11, mfloat_t m12, mfloat_t m21, mfloat_t m22);
mfloat_t *mat2_zero(mfloat_t *result);
mfloat_t *mat2_identity(mfloat_t *result);
mfloat_t mat2_determinant(mfloat_t *m0);
mfloat_t *mat2_assign(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_negative(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_transpose(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_cofactor(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_adjugate(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_multiply(mfloat_t *result, mfloat_t *m0, mfloat_t *m1);
mfloat_t *mat2_multiply_f(mfloat_t *result, mfloat_t *m0, mfloat_t f);
mfloat_t *mat2_inverse(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat2_scaling(mfloat_t *result, mfloat_t *v0);
mfloat_t *mat2_scale(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat2_rotation_z(mfloat_t *result, mfloat_t f);
mfloat_t *mat2_lerp(mfloat_t *result, mfloat_t *m0, mfloat_t *m1, mfloat_t f);
mfloat_t *mat3(mfloat_t *result, mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m31, mfloat_t m32, mfloat_t m33);
mfloat_t *mat3_zero(mfloat_t *result);
mfloat_t *mat3_identity(mfloat_t *result);
mfloat_t mat3_determinant(mfloat_t *m0);
mfloat_t *mat3_assign(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat3_negative(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat3_transpose(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat3_cofactor(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat3_multiply(mfloat_t *result, mfloat_t *m0, mfloat_t *m1);
mfloat_t *mat3_multiply_f(mfloat_t *result, mfloat_t *m0, mfloat_t f);
mfloat_t *mat3_inverse(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat3_scaling(mfloat_t *result, mfloat_t *v0);
mfloat_t *mat3_scale(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat3_rotation_x(mfloat_t *result, mfloat_t f);
mfloat_t *mat3_rotation_y(mfloat_t *result, mfloat_t f);
mfloat_t *mat3_rotation_z(mfloat_t *result, mfloat_t f);
mfloat_t *mat3_rotation_axis(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *mat3_rotation_quat(mfloat_t *result, mfloat_t *q0);
mfloat_t *mat3_lerp(mfloat_t *result, mfloat_t *m0, mfloat_t *m1, mfloat_t f);
mfloat_t *mat4(mfloat_t *result, mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m14, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m24, mfloat_t m31, mfloat_t m32, mfloat_t m33, mfloat_t m34, mfloat_t m41, mfloat_t m42, mfloat_t m43, mfloat_t m44);
mfloat_t *mat4_zero(mfloat_t *result);
mfloat_t *mat4_identity(mfloat_t *result);
mfloat_t mat4_determinant(mfloat_t *m0);
mfloat_t *mat4_assign(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat4_negative(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat4_transpose(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat4_cofactor(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat4_rotation_x(mfloat_t *result, mfloat_t f);
mfloat_t *mat4_rotation_y(mfloat_t *result, mfloat_t f);
mfloat_t *mat4_rotation_z(mfloat_t *result, mfloat_t f);
mfloat_t *mat4_rotation_axis(mfloat_t *result, mfloat_t *v0, mfloat_t f);
mfloat_t *mat4_rotation_quat(mfloat_t *result, mfloat_t *q0);
mfloat_t *mat4_translation(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat4_translate(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat4_scaling(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat4_scale(mfloat_t *result, mfloat_t *m0, mfloat_t *v0);
mfloat_t *mat4_multiply(mfloat_t *result, mfloat_t *m0, mfloat_t *m1);
mfloat_t *mat4_multiply_f(mfloat_t *result, mfloat_t *m0, mfloat_t f);
mfloat_t *mat4_inverse(mfloat_t *result, mfloat_t *m0);
mfloat_t *mat4_lerp(mfloat_t *result, mfloat_t *m0, mfloat_t *m1, mfloat_t f);
mfloat_t *mat4_look_at(mfloat_t *result, mfloat_t *position, mfloat_t *target, mfloat_t *up);
mfloat_t *mat4_ortho(mfloat_t *result, mfloat_t l, mfloat_t r, mfloat_t b, mfloat_t t, mfloat_t n, mfloat_t f);
mfloat_t *mat4_perspective(mfloat_t *result, mfloat_t fov_y, mfloat_t aspect, mfloat_t n, mfloat_t f);
mfloat_t *mat4_perspective_fov(mfloat_t *result, mfloat_t fov, mfloat_t w, mfloat_t h, mfloat_t n, mfloat_t f);
mfloat_t *mat4_perspective_infinite(mfloat_t *result, mfloat_t fov_y, mfloat_t aspect, mfloat_t n);
#endif

#if defined(MATHC_USE_STRUCT_FUNCTIONS)
#if defined(MATHC_USE_INT)
bool svec2i_is_zero(struct vec2i_st v0);
bool svec2i_is_equal(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i(mint_t x, mint_t y);
struct vec2i_st svec2i_assign(struct vec2i_st v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec2i_st svec2i_assign_vec2(struct vec2_st v0);
#endif
struct vec2i_st svec2i_zero(void);
struct vec2i_st svec2i_one(void);
struct vec2i_st svec2i_sign(struct vec2i_st v0);
struct vec2i_st svec2i_add(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_add_i(struct vec2i_st v0, mint_t i);
struct vec2i_st svec2i_subtract(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_subtract_i(struct vec2i_st v0, mint_t i);
struct vec2i_st svec2i_multiply(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_multiply_i(struct vec2i_st v0, mint_t i);
struct vec2i_st svec2i_divide(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_divide_i(struct vec2i_st v0, mint_t i);
struct vec2i_st svec2i_snap(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_snap_i(struct vec2i_st v0, mint_t i);
struct vec2i_st svec2i_negative(struct vec2i_st v0);
struct vec2i_st svec2i_abs(struct vec2i_st v0);
struct vec2i_st svec2i_max(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_min(struct vec2i_st v0, struct vec2i_st v1);
struct vec2i_st svec2i_clamp(struct vec2i_st v0, struct vec2i_st v1, struct vec2i_st v2);
struct vec2i_st svec2i_tangent(struct vec2i_st v0);
bool svec3i_is_zero(struct vec3i_st v0);
bool svec3i_is_equal(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i(mint_t x, mint_t y, mint_t z);
struct vec3i_st svec3i_assign(struct vec3i_st v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec3i_st svec3i_assign_vec3(struct vec3_st v0);
#endif
struct vec3i_st svec3i_zero(void);
struct vec3i_st svec3i_one(void);
struct vec3i_st svec3i_sign(struct vec3i_st v0);
struct vec3i_st svec3i_add(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_add_i(struct vec3i_st v0, mint_t i);
struct vec3i_st svec3i_subtract(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_subtract_i(struct vec3i_st v0, mint_t i);
struct vec3i_st svec3i_multiply(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_multiply_i(struct vec3i_st v0, mint_t i);
struct vec3i_st svec3i_divide(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_divide_i(struct vec3i_st v0, mint_t i);
struct vec3i_st svec3i_snap(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_snap_i(struct vec3i_st v0, mint_t i);
struct vec3i_st svec3i_cross(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_negative(struct vec3i_st v0);
struct vec3i_st svec3i_abs(struct vec3i_st v0);
struct vec3i_st svec3i_max(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_min(struct vec3i_st v0, struct vec3i_st v1);
struct vec3i_st svec3i_clamp(struct vec3i_st v0, struct vec3i_st v1, struct vec3i_st v2);
bool svec4i_is_zero(struct vec4i_st v0);
bool svec4i_is_equal(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i(mint_t x, mint_t y, mint_t z, mint_t w);
struct vec4i_st svec4i_assign(struct vec4i_st v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec4i_st svec4i_assign_vec4(struct vec4_st v0);
#endif
struct vec4i_st svec4i_zero(void);
struct vec4i_st svec4i_one(void);
struct vec4i_st svec4i_sign(struct vec4i_st v0);
struct vec4i_st svec4i_add(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_add_i(struct vec4i_st v0, mint_t i);
struct vec4i_st svec4i_subtract(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_subtract_i(struct vec4i_st v0, mint_t i);
struct vec4i_st svec4i_multiply(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_multiply_i(struct vec4i_st v0, mint_t i);
struct vec4i_st svec4i_divide(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_divide_i(struct vec4i_st v0, mint_t i);
struct vec4i_st svec4i_snap(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_snap_i(struct vec4i_st v0, mint_t i);
struct vec4i_st svec4i_negative(struct vec4i_st v0);
struct vec4i_st svec4i_abs(struct vec4i_st v0);
struct vec4i_st svec4i_max(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_min(struct vec4i_st v0, struct vec4i_st v1);
struct vec4i_st svec4i_clamp(struct vec4i_st v0, struct vec4i_st v1, struct vec4i_st v2);
#endif
#if defined(MATHC_USE_FLOATING_POINT)
bool svec2_is_zero(struct vec2_st v0);
bool svec2_is_equal(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2(mfloat_t x, mfloat_t y);
struct vec2_st svec2_assign(struct vec2_st v0);
#if defined(MATHC_USE_INT)
struct vec2_st svec2_assign_vec2i(struct vec2i_st v0);
#endif
struct vec2_st svec2_zero(void);
struct vec2_st svec2_one(void);
struct vec2_st svec2_sign(struct vec2_st v0);
struct vec2_st svec2_add(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_add_f(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_subtract(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_subtract_f(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_multiply(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_multiply_f(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_multiply_mat2(struct vec2_st v0, struct mat2_st m0);
struct vec2_st svec2_divide(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_divide_f(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_snap(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_snap_f(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_negative(struct vec2_st v0);
struct vec2_st svec2_abs(struct vec2_st v0);
struct vec2_st svec2_floor(struct vec2_st v0);
struct vec2_st svec2_ceil(struct vec2_st v0);
struct vec2_st svec2_round(struct vec2_st v0);
struct vec2_st svec2_max(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_min(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_clamp(struct vec2_st v0, struct vec2_st v1, struct vec2_st v2);
struct vec2_st svec2_normalize(struct vec2_st v0);
mfloat_t svec2_dot(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_project(struct vec2_st v0, struct vec2_st v1);
struct vec2_st svec2_slide(struct vec2_st v0, struct vec2_st normal);
struct vec2_st svec2_reflect(struct vec2_st v0, struct vec2_st normal);
struct vec2_st svec2_tangent(struct vec2_st v0);
struct vec2_st svec2_rotate(struct vec2_st v0, mfloat_t f);
struct vec2_st svec2_lerp(struct vec2_st v0, struct vec2_st v1, mfloat_t f);
struct vec2_st svec2_bezier3(struct vec2_st v0, struct vec2_st v1, struct vec2_st v2, mfloat_t f);
struct vec2_st svec2_bezier4(struct vec2_st v0, struct vec2_st v1, struct vec2_st v2, struct vec2_st v3, mfloat_t f);
mfloat_t svec2_angle(struct vec2_st v0);
mfloat_t svec2_length(struct vec2_st v0);
mfloat_t svec2_length_squared(struct vec2_st v0);
mfloat_t svec2_distance(struct vec2_st v0, struct vec2_st v1);
mfloat_t svec2_distance_squared(struct vec2_st v0, struct vec2_st v1);
bool svec3_is_zero(struct vec3_st v0);
bool svec3_is_equal(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3(mfloat_t x, mfloat_t y, mfloat_t z);
struct vec3_st svec3_assign(struct vec3_st v0);
#if defined(MATHC_USE_INT)
struct vec3_st svec3_assign_vec3i(struct vec3i_st v0);
#endif
struct vec3_st svec3_zero(void);
struct vec3_st svec3_one(void);
struct vec3_st svec3_sign(struct vec3_st v0);
struct vec3_st svec3_add(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_add_f(struct vec3_st v0, mfloat_t f);
struct vec3_st svec3_subtract(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_subtract_f(struct vec3_st v0, mfloat_t f);
struct vec3_st svec3_multiply(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_multiply_f(struct vec3_st v0, mfloat_t f);
struct vec3_st svec3_multiply_mat3(struct vec3_st v0, struct mat3_st m0);
struct vec3_st svec3_divide(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_divide_f(struct vec3_st v0, mfloat_t f);
struct vec3_st svec3_snap(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_snap_f(struct vec3_st v0, mfloat_t f);
struct vec3_st svec3_negative(struct vec3_st v0);
struct vec3_st svec3_abs(struct vec3_st v0);
struct vec3_st svec3_floor(struct vec3_st v0);
struct vec3_st svec3_ceil(struct vec3_st v0);
struct vec3_st svec3_round(struct vec3_st v0);
struct vec3_st svec3_max(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_min(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_clamp(struct vec3_st v0, struct vec3_st v1, struct vec3_st v2);
struct vec3_st svec3_cross(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_normalize(struct vec3_st v0);
mfloat_t svec3_dot(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_project(struct vec3_st v0, struct vec3_st v1);
struct vec3_st svec3_slide(struct vec3_st v0, struct vec3_st normal);
struct vec3_st svec3_reflect(struct vec3_st v0, struct vec3_st normal);
struct vec3_st svec3_rotate(struct vec3_st v0, struct vec3_st ra, mfloat_t f);
struct vec3_st svec3_lerp(struct vec3_st v0, struct vec3_st v1, mfloat_t f);
struct vec3_st svec3_bezier3(struct vec3_st v0, struct vec3_st v1, struct vec3_st v2, mfloat_t f);
struct vec3_st svec3_bezier4(struct vec3_st v0, struct vec3_st v1, struct vec3_st v2, struct vec3_st v3, mfloat_t f);
mfloat_t svec3_length(struct vec3_st v0);
mfloat_t svec3_length_squared(struct vec3_st v0);
mfloat_t svec3_distance(struct vec3_st v0, struct vec3_st v1);
mfloat_t svec3_distance_squared(struct vec3_st v0, struct vec3_st v1);
bool svec4_is_zero(struct vec4_st v0);
bool svec4_is_equal(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4(mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
struct vec4_st svec4_assign(struct vec4_st v0);
#if defined(MATHC_USE_INT)
struct vec4_st svec4_assign_vec4i(struct vec4i_st v0);
#endif
struct vec4_st svec4_zero(void);
struct vec4_st svec4_one(void);
struct vec4_st svec4_sign(struct vec4_st v0);
struct vec4_st svec4_add(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_add_f(struct vec4_st v0, mfloat_t f);
struct vec4_st svec4_subtract(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_subtract_f(struct vec4_st v0, mfloat_t f);
struct vec4_st svec4_multiply(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_multiply_f(struct vec4_st v0, mfloat_t f);
struct vec4_st svec4_multiply_mat4(struct vec4_st v0, struct mat4_st m0);
struct vec4_st svec4_divide(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_divide_f(struct vec4_st v0, mfloat_t f);
struct vec4_st svec4_snap(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_snap_f(struct vec4_st v0, mfloat_t f);
struct vec4_st svec4_negative(struct vec4_st v0);
struct vec4_st svec4_abs(struct vec4_st v0);
struct vec4_st svec4_floor(struct vec4_st v0);
struct vec4_st svec4_ceil(struct vec4_st v0);
struct vec4_st svec4_round(struct vec4_st v0);
struct vec4_st svec4_max(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_min(struct vec4_st v0, struct vec4_st v1);
struct vec4_st svec4_clamp(struct vec4_st v0, struct vec4_st v1, struct vec4_st v2);
struct vec4_st svec4_normalize(struct vec4_st v0);
struct vec4_st svec4_lerp(struct vec4_st v0, struct vec4_st v1, mfloat_t f);
bool squat_is_zero(struct quat_st q0);
bool squat_is_equal(struct quat_st q0, struct quat_st q1);
struct quat_st squat(mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
struct quat_st squat_assign(struct quat_st q0);
struct quat_st squat_zero(void);
struct quat_st squat_null(void);
struct quat_st squat_multiply(struct quat_st q0, struct quat_st q1);
struct quat_st squat_multiply_f(struct quat_st q0, mfloat_t f);
struct quat_st squat_divide(struct quat_st q0, struct quat_st q1);
struct quat_st squat_divide_f(struct quat_st q0, mfloat_t f);
struct quat_st squat_negative(struct quat_st q0);
struct quat_st squat_conjugate(struct quat_st q0);
struct quat_st squat_inverse(struct quat_st q0);
struct quat_st squat_normalize(struct quat_st q0);
mfloat_t squat_dot(struct quat_st q0, struct quat_st q1);
struct quat_st squat_power(struct quat_st q0, mfloat_t exponent);
struct quat_st squat_from_axis_angle(struct vec3_st v0, mfloat_t angle);
struct quat_st squat_from_vec3(struct vec3_st v0, struct vec3_st v1);
struct quat_st squat_from_mat4(struct mat4_st m0);
struct quat_st squat_lerp(struct quat_st q0, struct quat_st q1, mfloat_t f);
struct quat_st squat_slerp(struct quat_st q0, struct quat_st q1, mfloat_t f);
mfloat_t squat_length(struct quat_st q0);
mfloat_t squat_length_squared(struct quat_st q0);
mfloat_t squat_angle(struct quat_st q0, struct quat_st q1);
struct mat2_st smat2(mfloat_t m11, mfloat_t m12, mfloat_t m21, mfloat_t m22);
struct mat2_st smat2_zero(void);
struct mat2_st smat2_identity(void);
mfloat_t smat2_determinant(struct mat2_st m0);
struct mat2_st smat2_assign(struct mat2_st m0);
struct mat2_st smat2_negative(struct mat2_st m0);
struct mat2_st smat2_transpose(struct mat2_st m0);
struct mat2_st smat2_cofactor(struct mat2_st m0);
struct mat2_st smat2_adjugate(struct mat2_st m0);
struct mat2_st smat2_multiply(struct mat2_st m0, struct mat2_st m1);
struct mat2_st smat2_multiply_f(struct mat2_st m0, mfloat_t f);
struct mat2_st smat2_inverse(struct mat2_st m0);
struct mat2_st smat2_scaling(struct vec2_st v0);
struct mat2_st smat2_scale(struct mat2_st m0, struct vec2_st v0);
struct mat2_st smat2_rotation_z(mfloat_t f);
struct mat2_st smat2_lerp(struct mat2_st m0, struct mat2_st m1, mfloat_t f);
struct mat3_st smat3(mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m31, mfloat_t m32, mfloat_t m33);
struct mat3_st smat3_zero(void);
struct mat3_st smat3_identity(void);
mfloat_t smat3_determinant(struct mat3_st m0);
struct mat3_st smat3_assign(struct mat3_st m0);
struct mat3_st smat3_negative(struct mat3_st m0);
struct mat3_st smat3_transpose(struct mat3_st m0);
struct mat3_st smat3_cofactor(struct mat3_st m0);
struct mat3_st smat3_multiply(struct mat3_st m0, struct mat3_st m1);
struct mat3_st smat3_multiply_f(struct mat3_st m0, mfloat_t f);
struct mat3_st smat3_inverse(struct mat3_st m0);
struct mat3_st smat3_scaling(struct vec3_st v0);
struct mat3_st smat3_scale(struct mat3_st m0, struct vec3_st v0);
struct mat3_st smat3_rotation_x(mfloat_t f);
struct mat3_st smat3_rotation_y(mfloat_t f);
struct mat3_st smat3_rotation_z(mfloat_t f);
struct mat3_st smat3_rotation_axis(struct vec3_st v0, mfloat_t f);
struct mat3_st smat3_rotation_quat(struct quat_st q0);
struct mat3_st smat3_lerp(struct mat3_st m0, struct mat3_st m1, mfloat_t f);
struct mat4_st smat4(mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m14, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m24, mfloat_t m31, mfloat_t m32, mfloat_t m33, mfloat_t m34, mfloat_t m41, mfloat_t m42, mfloat_t m43, mfloat_t m44);
struct mat4_st smat4_zero(void);
struct mat4_st smat4_identity(void);
mfloat_t smat4_determinant(struct mat4_st m0);
struct mat4_st smat4_assign(struct mat4_st m0);
struct mat4_st smat4_negative(struct mat4_st m0);
struct mat4_st smat4_transpose(struct mat4_st m0);
struct mat4_st smat4_cofactor(struct mat4_st m0);
struct mat4_st smat4_rotation_x(mfloat_t f);
struct mat4_st smat4_rotation_y(mfloat_t f);
struct mat4_st smat4_rotation_z(mfloat_t f);
struct mat4_st smat4_rotation_axis(struct vec3_st v0, mfloat_t f);
struct mat4_st smat4_rotation_quat(struct quat_st q0);
struct mat4_st smat4_translation(struct mat4_st m0, struct vec3_st v0);
struct mat4_st smat4_translate(struct mat4_st m0, struct vec3_st v0);
struct mat4_st smat4_scaling(struct mat4_st m0, struct vec3_st v0);
struct mat4_st smat4_scale(struct mat4_st m0, struct vec3_st v0);
struct mat4_st smat4_multiply(struct mat4_st m0, struct mat4_st m1);
struct mat4_st smat4_multiply_f(struct mat4_st m0, mfloat_t f);
struct mat4_st smat4_inverse(struct mat4_st m0);
struct mat4_st smat4_lerp(struct mat4_st m0, struct mat4_st m1, mfloat_t f);
struct mat4_st smat4_look_at(struct vec3_st position, struct vec3_st target, struct vec3_st up);
struct mat4_st smat4_ortho(mfloat_t l, mfloat_t r, mfloat_t b, mfloat_t t, mfloat_t n, mfloat_t f);
struct mat4_st smat4_perspective(mfloat_t fov_y, mfloat_t aspect, mfloat_t n, mfloat_t f);
struct mat4_st smat4_perspective_fov(mfloat_t fov, mfloat_t w, mfloat_t h, mfloat_t n, mfloat_t f);
struct mat4_st smat4_perspective_infinite(mfloat_t fov_y, mfloat_t aspect, mfloat_t n);
#endif
#endif

#if defined(MATHC_USE_POINTER_STRUCT_FUNCTIONS)
#if defined(MATHC_USE_INT)
bool psvec2i_is_zero(struct vec2i_st *v0);
bool psvec2i_is_equal(struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i(struct vec2i_st *result, mint_t x, mint_t y);
struct vec2i_st *psvec2i_assign(struct vec2i_st *result, struct vec2i_st *v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec2i_st *psvec2i_assign_vec2(struct vec2i_st *result, struct vec2_st *v0);
#endif
struct vec2i_st *psvec2i_zero(struct vec2i_st *result);
struct vec2i_st *psvec2i_one(struct vec2i_st *result);
struct vec2i_st *psvec2i_sign(struct vec2i_st *result, struct vec2i_st *v0);
struct vec2i_st *psvec2i_add(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_add_i(struct vec2i_st *result, struct vec2i_st *v0, mint_t i);
struct vec2i_st *psvec2i_subtract(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_subtract_i(struct vec2i_st *result, struct vec2i_st *v0, mint_t i);
struct vec2i_st *psvec2i_multiply(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_multiply_i(struct vec2i_st *result, struct vec2i_st *v0, mint_t i);
struct vec2i_st *psvec2i_divide(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_divide_i(struct vec2i_st *result, struct vec2i_st *v0, mint_t i);
struct vec2i_st *psvec2i_snap(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_snap_i(struct vec2i_st *result, struct vec2i_st *v0, mint_t i);
struct vec2i_st *psvec2i_negative(struct vec2i_st *result, struct vec2i_st *v0);
struct vec2i_st *psvec2i_abs(struct vec2i_st *result, struct vec2i_st *v0);
struct vec2i_st *psvec2i_max(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_min(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1);
struct vec2i_st *psvec2i_clamp(struct vec2i_st *result, struct vec2i_st *v0, struct vec2i_st *v1, struct vec2i_st *v2);
struct vec2i_st *psvec2i_tangent(struct vec2i_st *result, struct vec2i_st *v0);
bool psvec3i_is_zero(struct vec3i_st *v0);
bool psvec3i_is_equal(struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i(struct vec3i_st *result, mint_t x, mint_t y, mint_t z);
struct vec3i_st *psvec3i_assign(struct vec3i_st *result, struct vec3i_st *v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec3i_st *psvec3i_assign_vec3(struct vec3i_st *result, struct vec3_st *v0);
#endif
struct vec3i_st *psvec3i_zero(struct vec3i_st *result);
struct vec3i_st *psvec3i_one(struct vec3i_st *result);
struct vec3i_st *psvec3i_sign(struct vec3i_st *result, struct vec3i_st *v0);
struct vec3i_st *psvec3i_add(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_add_i(struct vec3i_st *result, struct vec3i_st *v0, mint_t i);
struct vec3i_st *psvec3i_subtract(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_subtract_i(struct vec3i_st *result, struct vec3i_st *v0, mint_t i);
struct vec3i_st *psvec3i_multiply(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_multiply_i(struct vec3i_st *result, struct vec3i_st *v0, mint_t i);
struct vec3i_st *psvec3i_divide(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_divide_i(struct vec3i_st *result, struct vec3i_st *v0, mint_t i);
struct vec3i_st *psvec3i_snap(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_snap_i(struct vec3i_st *result, struct vec3i_st *v0, mint_t i);
struct vec3i_st *psvec3i_cross(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_negative(struct vec3i_st *result, struct vec3i_st *v0);
struct vec3i_st *psvec3i_abs(struct vec3i_st *result, struct vec3i_st *v0);
struct vec3i_st *psvec3i_max(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_min(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1);
struct vec3i_st *psvec3i_clamp(struct vec3i_st *result, struct vec3i_st *v0, struct vec3i_st *v1, struct vec3i_st *v2);
bool psvec4i_is_zero(struct vec4i_st *v0);
bool psvec4i_is_equal(struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i(struct vec4i_st *result, mint_t x, mint_t y, mint_t z, mint_t w);
struct vec4i_st *psvec4i_assign(struct vec4i_st *result, struct vec4i_st *v0);
#if defined(MATHC_USE_FLOATING_POINT)
struct vec4i_st *psvec4i_assign_vec4(struct vec4i_st *result, struct vec4_st *v0);
#endif
struct vec4i_st *psvec4i_zero(struct vec4i_st *result);
struct vec4i_st *psvec4i_one(struct vec4i_st *result);
struct vec4i_st *psvec4i_sign(struct vec4i_st *result, struct vec4i_st *v0);
struct vec4i_st *psvec4i_add(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_add_i(struct vec4i_st *result, struct vec4i_st *v0, mint_t i);
struct vec4i_st *psvec4i_subtract(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_subtract_i(struct vec4i_st *result, struct vec4i_st *v0, mint_t i);
struct vec4i_st *psvec4i_multiply(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_multiply_i(struct vec4i_st *result, struct vec4i_st *v0, mint_t i);
struct vec4i_st *psvec4i_divide(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_divide_i(struct vec4i_st *result, struct vec4i_st *v0, mint_t i);
struct vec4i_st *psvec4i_snap(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_snap_i(struct vec4i_st *result, struct vec4i_st *v0, mint_t i);
struct vec4i_st *psvec4i_negative(struct vec4i_st *result, struct vec4i_st *v0);
struct vec4i_st *psvec4i_abs(struct vec4i_st *result, struct vec4i_st *v0);
struct vec4i_st *psvec4i_max(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_min(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1);
struct vec4i_st *psvec4i_clamp(struct vec4i_st *result, struct vec4i_st *v0, struct vec4i_st *v1, struct vec4i_st *v2);
#endif
#if defined(MATHC_USE_FLOATING_POINT)
bool psvec2_is_zero(struct vec2_st *v0);
bool psvec2_is_equal(struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2(struct vec2_st *result, mfloat_t x, mfloat_t y);
struct vec2_st *psvec2_assign(struct vec2_st *result, struct vec2_st *v0);
#if defined(MATHC_USE_INT)
struct vec2_st *psvec2_assign_vec2i(struct vec2_st *result, struct vec2i_st *v0);
#endif
struct vec2_st *psvec2_zero(struct vec2_st *result);
struct vec2_st *psvec2_one(struct vec2_st *result);
struct vec2_st *psvec2_sign(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_add(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_add_f(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_subtract(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_subtract_f(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_multiply(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_multiply_f(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_multiply_mat2(struct vec2_st *result, struct vec2_st *v0, struct mat2_st *m0);
struct vec2_st *psvec2_divide(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_divide_f(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_snap(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_snap_f(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_negative(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_abs(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_floor(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_ceil(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_round(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_max(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_min(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_clamp(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1, struct vec2_st *v2);
struct vec2_st *psvec2_normalize(struct vec2_st *result, struct vec2_st *v0);
mfloat_t psvec2_dot(struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_project(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1);
struct vec2_st *psvec2_slide(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *normal);
struct vec2_st *psvec2_reflect(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *normal);
struct vec2_st *psvec2_tangent(struct vec2_st *result, struct vec2_st *v0);
struct vec2_st *psvec2_rotate(struct vec2_st *result, struct vec2_st *v0, mfloat_t f);
struct vec2_st *psvec2_lerp(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1, mfloat_t f);
struct vec2_st *psvec2_bezier3(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1, struct vec2_st *v2, mfloat_t f);
struct vec2_st *psvec2_bezier4(struct vec2_st *result, struct vec2_st *v0, struct vec2_st *v1, struct vec2_st *v2, struct vec2_st *v3, mfloat_t f);
mfloat_t psvec2_angle(struct vec2_st *v0);
mfloat_t psvec2_length(struct vec2_st *v0);
mfloat_t psvec2_length_squared(struct vec2_st *v0);
mfloat_t psvec2_distance(struct vec2_st *v0, struct vec2_st *v1);
mfloat_t psvec2_distance_squared(struct vec2_st *v0, struct vec2_st *v1);
bool psvec3_is_zero(struct vec3_st *v0);
bool psvec3_is_equal(struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3(struct vec3_st *result, mfloat_t x, mfloat_t y, mfloat_t z);
struct vec3_st *psvec3_assign(struct vec3_st *result, struct vec3_st *v0);
#if defined(MATHC_USE_INT)
struct vec3_st *psvec3_assign_vec3i(struct vec3_st *result, struct vec3i_st *v0);
#endif
struct vec3_st *psvec3_zero(struct vec3_st *result);
struct vec3_st *psvec3_one(struct vec3_st *result);
struct vec3_st *psvec3_sign(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_add(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_add_f(struct vec3_st *result, struct vec3_st *v0, mfloat_t f);
struct vec3_st *psvec3_subtract(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_subtract_f(struct vec3_st *result, struct vec3_st *v0, mfloat_t f);
struct vec3_st *psvec3_multiply(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_multiply_f(struct vec3_st *result, struct vec3_st *v0, mfloat_t f);
struct vec3_st *psvec3_multiply_mat3(struct vec3_st *result, struct vec3_st *v0, struct mat3_st *m0);
struct vec3_st *psvec3_divide(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_divide_f(struct vec3_st *result, struct vec3_st *v0, mfloat_t f);
struct vec3_st *psvec3_snap(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_snap_f(struct vec3_st *result, struct vec3_st *v0, mfloat_t f);
struct vec3_st *psvec3_negative(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_abs(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_floor(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_ceil(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_round(struct vec3_st *result, struct vec3_st *v0);
struct vec3_st *psvec3_max(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_min(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_clamp(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1, struct vec3_st *v2);
struct vec3_st *psvec3_cross(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_normalize(struct vec3_st *result, struct vec3_st *v0);
mfloat_t psvec3_dot(struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_project(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct vec3_st *psvec3_slide(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *normal);
struct vec3_st *psvec3_reflect(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *normal);
struct vec3_st *psvec3_rotate(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *ra, mfloat_t f);
struct vec3_st *psvec3_lerp(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1, mfloat_t f);
struct vec3_st *psvec3_bezier3(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1, struct vec3_st *v2, mfloat_t f);
struct vec3_st *psvec3_bezier4(struct vec3_st *result, struct vec3_st *v0, struct vec3_st *v1, struct vec3_st *v2, struct vec3_st *v3, mfloat_t f);
mfloat_t psvec3_length(struct vec3_st *v0);
mfloat_t psvec3_length_squared(struct vec3_st *v0);
mfloat_t psvec3_distance(struct vec3_st *v0, struct vec3_st *v1);
mfloat_t psvec3_distance_squared(struct vec3_st *v0, struct vec3_st *v1);
bool psvec4_is_zero(struct vec4_st *v0);
bool psvec4_is_equal(struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4(struct vec4_st *result, mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
struct vec4_st *psvec4_assign(struct vec4_st *result, struct vec4_st *v0);
#if defined(MATHC_USE_INT)
struct vec4_st *psvec4_assign_vec4i(struct vec4_st *result, struct vec4i_st *v0);
#endif
struct vec4_st *psvec4_zero(struct vec4_st *result);
struct vec4_st *psvec4_one(struct vec4_st *result);
struct vec4_st *psvec4_sign(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_add(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_add_f(struct vec4_st *result, struct vec4_st *v0, mfloat_t f);
struct vec4_st *psvec4_subtract(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_subtract_f(struct vec4_st *result, struct vec4_st *v0, mfloat_t f);
struct vec4_st *psvec4_multiply(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_multiply_f(struct vec4_st *result, struct vec4_st *v0, mfloat_t f);
struct vec4_st *psvec4_multiply_mat4(struct vec4_st *result, struct vec4_st *v0, struct mat4_st *m0);
struct vec4_st *psvec4_divide(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_divide_f(struct vec4_st *result, struct vec4_st *v0, mfloat_t f);
struct vec4_st *psvec4_snap(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_snap_f(struct vec4_st *result, struct vec4_st *v0, mfloat_t f);
struct vec4_st *psvec4_negative(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_abs(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_floor(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_ceil(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_round(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_max(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_min(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1);
struct vec4_st *psvec4_clamp(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1, struct vec4_st *v2);
struct vec4_st *psvec4_normalize(struct vec4_st *result, struct vec4_st *v0);
struct vec4_st *psvec4_lerp(struct vec4_st *result, struct vec4_st *v0, struct vec4_st *v1, mfloat_t f);
bool psquat_is_zero(struct quat_st *q0);
bool psquat_is_equal(struct quat_st *q0, struct quat_st *q1);
struct quat_st *psquat(struct quat_st *result, mfloat_t x, mfloat_t y, mfloat_t z, mfloat_t w);
struct quat_st *psquat_assign(struct quat_st *result, struct quat_st *q0);
struct quat_st *psquat_zero(struct quat_st *result);
struct quat_st *psquat_null(struct quat_st *result);
struct quat_st *psquat_multiply(struct quat_st *result, struct quat_st *q0, struct quat_st *q1);
struct quat_st *psquat_multiply_f(struct quat_st *result, struct quat_st *q0, mfloat_t f);
struct quat_st *psquat_divide(struct quat_st *result, struct quat_st *q0, struct quat_st *q1);
struct quat_st *psquat_divide_f(struct quat_st *result, struct quat_st *q0, mfloat_t f);
struct quat_st *psquat_negative(struct quat_st *result, struct quat_st *q0);
struct quat_st *psquat_conjugate(struct quat_st *result, struct quat_st *q0);
struct quat_st *psquat_inverse(struct quat_st *result, struct quat_st *q0);
struct quat_st *psquat_normalize(struct quat_st *result, struct quat_st *q0);
mfloat_t psquat_dot(struct quat_st *q0, struct quat_st *q1);
struct quat_st *psquat_power(struct quat_st *result, struct quat_st *q0, mfloat_t exponent);
struct quat_st *psquat_from_axis_angle(struct quat_st *result, struct vec3_st *v0, mfloat_t angle);
struct quat_st *psquat_from_vec3(struct quat_st *result, struct vec3_st *v0, struct vec3_st *v1);
struct quat_st *psquat_from_mat4(struct quat_st *result, struct mat4_st *m0);
struct quat_st *psquat_lerp(struct quat_st *result, struct quat_st *q0, struct quat_st *q1, mfloat_t f);
struct quat_st *psquat_slerp(struct quat_st *result, struct quat_st *q0, struct quat_st *q1, mfloat_t f);
mfloat_t psquat_length(struct quat_st *q0);
mfloat_t psquat_length_squared(struct quat_st *q0);
mfloat_t psquat_angle(struct quat_st *q0, struct quat_st *q1);
struct mat2_st *psmat2(struct mat2_st *result, mfloat_t m11, mfloat_t m12, mfloat_t m21, mfloat_t m22);
struct mat2_st *psmat2_zero(struct mat2_st *result);
struct mat2_st *psmat2_identity(struct mat2_st *result);
mfloat_t psmat2_determinant(struct mat2_st *m0);
struct mat2_st *psmat2_assign(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_negative(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_transpose(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_cofactor(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_adjugate(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_multiply(struct mat2_st *result, struct mat2_st *m0, struct mat2_st *m1);
struct mat2_st *psmat2_multiply_f(struct mat2_st *result, struct mat2_st *m0, mfloat_t f);
struct mat2_st *psmat2_inverse(struct mat2_st *result, struct mat2_st *m0);
struct mat2_st *psmat2_scaling(struct mat2_st *result, struct vec2_st *v0);
struct mat2_st *psmat2_scale(struct mat2_st *result, struct mat2_st *m0, struct vec2_st *v0);
struct mat2_st *psmat2_rotation_z(struct mat2_st *result, mfloat_t f);
struct mat2_st *psmat2_lerp(struct mat2_st *result, struct mat2_st *m0, struct mat2_st *m1, mfloat_t f);
struct mat3_st *psmat3(struct mat3_st *result, mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m31, mfloat_t m32, mfloat_t m33);
struct mat3_st *psmat3_zero(struct mat3_st *result);
struct mat3_st *psmat3_identity(struct mat3_st *result);
mfloat_t psmat3_determinant(struct mat3_st *m0);
struct mat3_st *psmat3_assign(struct mat3_st *result, struct mat3_st *m0);
struct mat3_st *psmat3_negative(struct mat3_st *result, struct mat3_st *m0);
struct mat3_st *psmat3_transpose(struct mat3_st *result, struct mat3_st *m0);
struct mat3_st *psmat3_cofactor(struct mat3_st *result, struct mat3_st *m0);
struct mat3_st *psmat3_multiply(struct mat3_st *result, struct mat3_st *m0, struct mat3_st *m1);
struct mat3_st *psmat3_multiply_f(struct mat3_st *result, struct mat3_st *m0, mfloat_t f);
struct mat3_st *psmat3_inverse(struct mat3_st *result, struct mat3_st *m0);
struct mat3_st *psmat3_scaling(struct mat3_st *result, struct vec3_st *v0);
struct mat3_st *psmat3_scale(struct mat3_st *result, struct mat3_st *m0, struct vec3_st *v0);
struct mat3_st *psmat3_rotation_x(struct mat3_st *result, mfloat_t f);
struct mat3_st *psmat3_rotation_y(struct mat3_st *result, mfloat_t f);
struct mat3_st *psmat3_rotation_z(struct mat3_st *result, mfloat_t f);
struct mat3_st *psmat3_rotation_axis(struct mat3_st *result, struct vec3_st *v0, mfloat_t f);
struct mat3_st *psmat3_rotation_quat(struct mat3_st *result, struct quat_st *q0);
struct mat3_st *psmat3_lerp(struct mat3_st *result, struct mat3_st *m0, struct mat3_st *m1, mfloat_t f);
struct mat4_st *psmat4(struct mat4_st *result, mfloat_t m11, mfloat_t m12, mfloat_t m13, mfloat_t m14, mfloat_t m21, mfloat_t m22, mfloat_t m23, mfloat_t m24, mfloat_t m31, mfloat_t m32, mfloat_t m33, mfloat_t m34, mfloat_t m41, mfloat_t m42, mfloat_t m43, mfloat_t m44);
struct mat4_st *psmat4_zero(struct mat4_st *result);
struct mat4_st *psmat4_identity(struct mat4_st *result);
mfloat_t psmat4_determinant(struct mat4_st *m0);
struct mat4_st *psmat4_assign(struct mat4_st *result, struct mat4_st *m0);
struct mat4_st *psmat4_negative(struct mat4_st *result, struct mat4_st *m0);
struct mat4_st *psmat4_transpose(struct mat4_st *result, struct mat4_st *m0);
struct mat4_st *psmat4_cofactor(struct mat4_st *result, struct mat4_st *m0);
struct mat4_st *psmat4_rotation_x(struct mat4_st *result, mfloat_t f);
struct mat4_st *psmat4_rotation_y(struct mat4_st *result, mfloat_t f);
struct mat4_st *psmat4_rotation_z(struct mat4_st *result, mfloat_t f);
struct mat4_st *psmat4_rotation_axis(struct mat4_st *result, struct vec3_st *v0, mfloat_t f);
struct mat4_st *psmat4_rotation_quat(struct mat4_st *result, struct quat_st *q0);
struct mat4_st *psmat4_translation(struct mat4_st *result, struct mat4_st *m0, struct vec3_st *v0);
struct mat4_st *psmat4_translate(struct mat4_st *result, struct mat4_st *m0, struct vec3_st *v0);
struct mat4_st *psmat4_scaling(struct mat4_st *result, struct mat4_st *m0, struct vec3_st *v0);
struct mat4_st *psmat4_scale(struct mat4_st *result, struct mat4_st *m0, struct vec3_st *v0);
struct mat4_st *psmat4_multiply(struct mat4_st *result, struct mat4_st *m0, struct mat4_st *m1);
struct mat4_st *psmat4_multiply_f(struct mat4_st *result, struct mat4_st *m0, mfloat_t f);
struct mat4_st *psmat4_inverse(struct mat4_st *result, struct mat4_st *m0);
struct mat4_st *psmat4_lerp(struct mat4_st *result, struct mat4_st *m0, struct mat4_st *m1, mfloat_t f);
struct mat4_st *psmat4_look_at(struct mat4_st *result, struct vec3_st *position, struct vec3_st *target, struct vec3_st *up);
struct mat4_st *psmat4_ortho(struct mat4_st *result, mfloat_t l, mfloat_t r, mfloat_t b, mfloat_t t, mfloat_t n, mfloat_t f);
struct mat4_st *psmat4_perspective(struct mat4_st *result, mfloat_t fov_y, mfloat_t aspect, mfloat_t n, mfloat_t f);
struct mat4_st *psmat4_perspective_fov(struct mat4_st *result, mfloat_t fov, mfloat_t w, mfloat_t h, mfloat_t n, mfloat_t f);
struct mat4_st *psmat4_perspective_infinite(struct mat4_st *result, mfloat_t fov_y, mfloat_t aspect, mfloat_t n);
#endif
#endif

#if defined(MATHC_USE_FLOATING_POINT) && defined(MATHC_USE_EASING_FUNCTIONS)
mfloat_t quadratic_ease_out(mfloat_t f);
mfloat_t quadratic_ease_in(mfloat_t f);
mfloat_t quadratic_ease_in_out(mfloat_t f);
mfloat_t cubic_ease_out(mfloat_t f);
mfloat_t cubic_ease_in(mfloat_t f);
mfloat_t cubic_ease_in_out(mfloat_t f);
mfloat_t quartic_ease_out(mfloat_t f);
mfloat_t quartic_ease_in(mfloat_t f);
mfloat_t quartic_ease_in_out(mfloat_t f);
mfloat_t quintic_ease_out(mfloat_t f);
mfloat_t quintic_ease_in(mfloat_t f);
mfloat_t quintic_ease_in_out(mfloat_t f);
mfloat_t sine_ease_out(mfloat_t f);
mfloat_t sine_ease_in(mfloat_t f);
mfloat_t sine_ease_in_out(mfloat_t f);
mfloat_t circular_ease_out(mfloat_t f);
mfloat_t circular_ease_in(mfloat_t f);
mfloat_t circular_ease_in_out(mfloat_t f);
mfloat_t exponential_ease_out(mfloat_t f);
mfloat_t exponential_ease_in(mfloat_t f);
mfloat_t exponential_ease_in_out(mfloat_t f);
mfloat_t elastic_ease_out(mfloat_t f);
mfloat_t elastic_ease_in(mfloat_t f);
mfloat_t elastic_ease_in_out(mfloat_t f);
mfloat_t back_ease_out(mfloat_t f);
mfloat_t back_ease_in(mfloat_t f);
mfloat_t back_ease_in_out(mfloat_t f);
mfloat_t bounce_ease_out(mfloat_t f);
mfloat_t bounce_ease_in(mfloat_t f);
mfloat_t bounce_ease_in_out(mfloat_t f);
#endif

#endif
