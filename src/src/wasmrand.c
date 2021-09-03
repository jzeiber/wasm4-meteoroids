#include "wasmrand.h"

/* 
	random
	https://www.maximintegrated.com/en/design/technical-documents/app-notes/4/4400.html
*/

uint32_t rand_lfsr32, rand_lfsr31;

uint32_t rand_shift_lfsr(uint32_t *lfsr, uint32_t polynomial_mask)
{
	uint32_t feedback;
	
	feedback = *lfsr & 1;
	*lfsr >>= 1;
	if(feedback == 1)
	{
		*lfsr ^= polynomial_mask;
	}
	return *lfsr;
}

void rand_init_lfsrs(uint32_t seed)
{
	rand_lfsr32 = 0xABCDE ^ seed;
	rand_lfsr31 = 0x23456789;
}

void _srand(uint32_t seed)
{
	rand_init_lfsrs(seed);
}

uint32_t _rand(void)
{
	rand_shift_lfsr(&rand_lfsr32, RAND_POLY_MASK_32);
	return (rand_shift_lfsr(&rand_lfsr32, RAND_POLY_MASK_32) ^ rand_shift_lfsr(&rand_lfsr31, RAND_POLY_MASK_31)) & 0xffff;
}

double _drand(void)
{
	return (double)_rand()/(double)RAND_MAX;
}
