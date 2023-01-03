#include "hash.h"

// https://github.com/skeeto/hash-prospector

// exact bias: 0.020829410544597495
uint32_t triple32inc(uint32_t x)
{
    x++;
    x ^= x >> 17;
    x *= UINT32_C(0xed5ad4bb);
    x ^= x >> 11;
    x *= UINT32_C(0xac4c1b51);
    x ^= x >> 15;
    x *= UINT32_C(0x31848bab);
    x ^= x >> 14;
    return x;
}

// inverse
uint32_t triple32inc_r(uint32_t x)
{
    x ^= x >> 14 ^ x >> 28;
    x *= UINT32_C(0x32b21703);
    x ^= x >> 15 ^ x >> 30;
    x *= UINT32_C(0x469e0db1);
    x ^= x >> 11 ^ x >> 22;
    x *= UINT32_C(0x79a85073);
    x ^= x >> 17;
    x--;
    return x;
}
