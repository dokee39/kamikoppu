#pragma once

#include <stdint.h>

// typedef signed char int8_t;
// typedef signed short int int16_t;
// typedef signed int int32_t;
// typedef signed long long int64_t;

/* exact-width unsigned integer types */
// typedef unsigned char uint8_t;
// typedef unsigned short int uint16_t;
// typedef unsigned int uint32_t;
// typedef unsigned long long uint64_t;

typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#if __has_attribute(__fallthrough__)
#define fallthrough __attribute__((__fallthrough__))
#else
#define fallthrough do {} while (0) /* fallthrough */
#endif

#define barrier()	   __asm__ __volatile__ ("" : : : "memory")

#define ACCESS_ONCE(x) (*(volatile __typeof(x) *)&(x))

#define WRITE_ONCE(x,v) \
	do { \
		barrier(); \
		ACCESS_ONCE(x) = (v); \
		barrier(); \
	} while (0)

#define READ_ONCE(x) \
	({ \
		__typeof(x) __var = ({ \
			barrier(); \
			ACCESS_ONCE(x); \
		}); \
		barrier(); \
		__var; \
	})

#define lockless_dereference(p) READ_ONCE(p)

#define __same_type(a, b)		__builtin_types_compatible_p(typeof(a), typeof(b))
#define __must_be_array(a)		__same_type(a, &(a)[0])

#define sizeof_field(_s, _m)	sizeof(((_s *)0)->_m)

#define container_of(ptr, type, member) \
	({ \
		__typeof(((type *)0)->member) *_p = (ptr); \
		(type *)((char *)_p - offsetof(type, member)); \
	})

#define struct_size(ptr, field, num) \
	(offsetof(__typeof(*(ptr)), field) + sizeof((ptr)->field[0]) * (num))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define DECLARE_FLEX_ARRAY(_t, _n) \
	struct { \
		struct { } __dummy_ ## _n; \
		_t _n[0]; \
	}

#define min(x, y)			((x) < (y) ? (x) : (y))
#define max(x, y)			((x) > (y) ? (x) : (y))

#define min3(a, b, c)		min(a, min(b, c))
#define max3(a, b, c)		max(a, max(b, c))

#define min4(a, b, c, d)	min(min(a, b), min(c, d))
#define max4(a, b, c, d)	max(max(a, b), max(b, d))

#define min5(a, b, c, d, e) min3(min(a, b), min(c, d), e)
#define max5(a, b, c, d, e) max3(max(a, b), max(b, d), e)

