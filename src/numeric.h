#pragma once

#ifdef __cplusplus
#define USE_DECIMAL 0
//#include <decimal/decimal.h>
//typedef __d64 _Decimal64;
#else
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

