#pragma once

#include <stdint.h>

#define RAND_MAX 0xffff;
#define RAND_POLY_MASK_32 0xB4BCD35C
#define RAND_POLY_MASK_31 0x7A5BC2E3

#ifdef __cplusplus
extern "C" {
#endif

void _srand(uint32_t seed);
uint32_t _rand(void);		// 0 to RAND_MAX
double _drand(void);		// 0.0 to 1.0

#ifdef __cplusplus
}
#endif