#pragma once

#include <stdint.h>

#ifdef __cplusplus
//#include <decimal/decimal>
//typedef std::decimal::decimal64 num_cc;
//typedef __decfloat64 num_cc;
#define USE_DECIMAL 0
//#include <decimal/decimal.h>
//typedef __d64 _Decimal64;
#else
typedef _Decimal64 num_c;
#define USE_DECIMAL 1
#endif

#if USE_DECIMAL
typedef _Decimal64 num;
#define NUM_HUNDREDTH 0.01DD
#define NUM_TEN 10.0DD
#else
typedef double num;
#define NUM_HUNDREDTH 0.01
#define NUM_TEN 10.0
#endif


typedef struct { uint8_t bytes[8]; } num_s;
