/*
 * Lexicon: a collection of name and phrase generation methods.
 */

#pragma once

/* Filter bits for refining word and phrase searches */
#define LEXICON_FILTER_GENERAL  (0x001)
#define LEXICON_FILTER_NEUTRAL  (0x002)
#define LEXICON_FILTER_GOOD     (0x004)
#define LEXICON_FILTER_EVIL     (0x008)
#define LEXICON_FILTER_LAWFUL   (0x010)
#define LEXICON_FILTER_CHAOTIC  (0x020)
#define LEXICON_FILTER_VIOLENT  (0x040)

typedef struct {
        const char *text;
        unsigned int filter;
} LEXICON_ENTRY;

const char *lexicon_random_adjective(unsigned int filter);
const char *lexicon_random_collective_noun(unsigned int filter);
const char *lexicon_random_abstract_noun(unsigned int filter);
const char *lexicon_random_concrete_noun(unsigned int filter);
