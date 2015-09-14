#pragma once

// -------------------------------------------------------------------------
#define ZERO_MEM(a)			memset(a, 0, sizeof(a))
#define SIZEOF_ARRAY(a)		(sizeof(a) / sizeof(a[0]))
#define SAFE_FREE(p)		if (p) {delete p; p = NULL;}
#define SAFE_FREE_ARRAY(p)	if (p) {delete[] p; p = NULL;}

#define SET_BIT(item, bit) (item) |= (1 << (bit))
#define CLEAR_BIT(item, bit) (item) &= ~(1 << (bit))

typedef unsigned int uint;
typedef unsigned char uchar;

