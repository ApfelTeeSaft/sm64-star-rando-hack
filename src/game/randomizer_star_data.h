#ifndef RANDOMIZER_STAR_DATA_H
#define RANDOMIZER_STAR_DATA_H

#include <PR/ultratypes.h>

#include "course_table.h"
#include "level_table.h"

#define RANDOMIZER_STAR_COUNT 120

enum RandomizerStarKind {
    RANDOMIZER_STAR_NORMAL,
    RANDOMIZER_STAR_100_COINS,
    RANDOMIZER_STAR_BOWSER_RED_COINS,
    RANDOMIZER_STAR_SECRET,
    RANDOMIZER_STAR_TOAD,
    RANDOMIZER_STAR_MIPS
};

struct RandomizerStar {
    u8 id;
    u8 course;
    u8 level;
    u8 area;
    u8 warpNode;
    u8 act;
    u8 starIndex;
    u8 difficulty;
    u16 estimatedSeconds;
    u8 kind;
    const char *courseName;
    const char *name;
};

extern const struct RandomizerStar gRandomizerStars[RANDOMIZER_STAR_COUNT];

const struct RandomizerStar *randomizer_star_get(s32 starIndex);
s32 randomizer_star_index_from_course_star(s32 courseNum, s32 levelNum, s32 starIndex);
s32 randomizer_star_course_order(s32 starIndex);
s32 randomizer_star_count_in_course(s32 courseNum);
s32 randomizer_star_first_in_course(s32 courseNum);
const char *randomizer_star_course_name(s32 courseNum);

#endif // RANDOMIZER_STAR_DATA_H
