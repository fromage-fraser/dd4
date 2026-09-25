#ifndef DD4_RNG_H
#define DD4_RNG_H

void rng_init(void);

int number_range(int from, int to);
int number_percent(void);
int number_door(void);
int number_bits(int width);

#endif