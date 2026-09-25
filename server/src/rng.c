#include "rng.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#if CHAR_BIT != 8 || INT_MAX != 2147483647 || INT_MIN != (-2147483647 - 1)
#error "DD4 RNG requires 8-bit bytes and a 32-bit signed int."
#endif

/*
 * Get one full-width random value. Never use the buffer after failure.
 */
static uint32_t rng_u32(void)
{
        uint32_t value;

        if (RAND_bytes((unsigned char *)&value, (int)sizeof(value)) != 1)
        {
                fputs("FATAL: OpenSSL RAND_bytes failed; stopping DD4.\n",
                      stderr);
                ERR_print_errors_fp(stderr);
                exit(EXIT_FAILURE);
        }

        return value;
}

/*
 * Exercise the generator at startup so configuration/entropy failures
 * are detected before loading the world. OpenSSL handles its own seeding.
 */
void rng_init(void)
{
        (void)rng_u32();
}

/*
 * Uniform value in [0, upper_bound - 1].
 * Reject the incomplete group at the top before applying modulo.
 */
static uint32_t rng_uniform(uint32_t upper_bound)
{
        uint64_t limit;
        uint32_t value;

        if (upper_bound == 0)
        {
                fputs("FATAL: RNG received a zero upper bound.\n", stderr);
                exit(EXIT_FAILURE);
        }

        if (upper_bound == 1)
                return 0;

        limit = UINT64_C(1) << 32;
        limit -= limit % upper_bound;

        do
        {
                value = rng_u32();
        }
        while ((uint64_t)value >= limit);

        return value % upper_bound;
}

/*
 * Inclusive endpoints. Preserve the old convention for equal/reversed
 * endpoints, while avoiding signed overflow when calculating the width.
 */
int number_range(int from, int to)
{
        uint64_t span;
        uint32_t offset;

        if (from >= to)
                return from;

        span = (uint64_t)((int64_t)to - (int64_t)from) + UINT64_C(1);

        /* All 2^32 possible values: no bounded reduction is needed. */
        if (span == (UINT64_C(1) << 32))
                offset = rng_u32();
        else
                offset = rng_uniform((uint32_t)span);

        return (int)((int64_t)from + (int64_t)offset);
}

int number_percent(void)
{
        return 1 + (int)rng_uniform(100);
}

int number_door(void)
{
        return (int)rng_uniform(6);
}

/*
 * Return 0 through (2^width - 1), inclusive. Width must be 0..31.
 */
int number_bits(int width)
{
        uint32_t mask;

        if (width < 0 || width > 31)
        {
                fprintf(stderr,
                        "FATAL: number_bits(%d): expected width 0..31.\n",
                        width);
                exit(EXIT_FAILURE);
        }

        if (width == 0)
                return 0;

        mask = (UINT32_C(1) << width) - UINT32_C(1);
        return (int)(rng_u32() & mask);
}