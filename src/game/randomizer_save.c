#include <PR/ultratypes.h>

#include "course_table.h"
#include "randomizer_save.h"
#include "randomizer_star_data.h"
#include "save_file.h"
#include "sm64.h"

#define RANDO_FLAG_ACTIVE             (1 << 0)
#define RANDO_FLAG_COMPLETE           (1 << 1)
#define RANDO_FLAG_UNLOCK_CAPS        (1 << 2)
#define RANDO_FLAG_UNLOCK_CANNONS     (1 << 3)
#define RANDO_FLAG_OPEN_DOORS         (1 << 4)
#define RANDO_FLAG_DIFFICULTY_WEIGHTS (1 << 5)
#define RANDO_FLAG_ORDER_BY_STAGE     (1 << 6)

#define RANDO_PACKED_SEED_MASK        0x03FF
#define RANDO_WDW_WATER_SHIFT         10
#define RANDO_THI_SIZE_SHIFT          12
#define RANDO_TTC_SPEED_SHIFT         14

#define RANDO_REQUIRED_DOOR_FLAGS \
    (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR \
     | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR | SAVE_FLAG_DDD_MOVED_BACK | SAVE_FLAG_MOAT_DRAINED \
     | SAVE_FLAG_UNLOCKED_PSS_DOOR | SAVE_FLAG_UNLOCKED_WF_DOOR | SAVE_FLAG_UNLOCKED_CCM_DOOR \
     | SAVE_FLAG_UNLOCKED_JRB_DOOR | SAVE_FLAG_UNLOCKED_BITDW_DOOR | SAVE_FLAG_UNLOCKED_BITFS_DOOR \
     | SAVE_FLAG_UNLOCKED_50_STAR_DOOR)

#define RANDO_CAP_FLAGS \
    (SAVE_FLAG_HAVE_WING_CAP | SAVE_FLAG_HAVE_METAL_CAP | SAVE_FLAG_HAVE_VANISH_CAP)

#define RANDO_LOST_CAP_FLAGS \
    (SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI \
     | SAVE_FLAG_CAP_ON_MR_BLIZZARD)

struct RandomizerStageConfigSlot {
    u8 course;
    u8 firstBit;
    u8 bitCount;
};

static const struct RandomizerStageConfigSlot sRandomizerStageConfigSlots[] = {
    { COURSE_BITDW, 1, 7 },
    { COURSE_BITFS, 1, 7 },
    { COURSE_BITS,  1, 7 },
    { COURSE_PSS,   2, 6 },
    { COURSE_COTMC, 1, 7 },
    { COURSE_TOTWC, 1, 7 },
    { COURSE_VCUTM, 1, 1 },
};

static struct SaveFile *randomizer_save_file(s32 fileIndex) {
    return &gSaveBuffer.files[fileIndex][0];
}

static s32 randomizer_save_stage_slot_for_star(s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);
    s32 firstStar;

    if (star == NULL) {
        return -1;
    }

    firstStar = randomizer_star_first_in_course(star->course);
    if (firstStar < 0) {
        return -1;
    }

    switch (star->course) {
        case COURSE_WDW:
            return starIndex - firstStar;
        case COURSE_THI:
            return 7 + starIndex - firstStar;
        case COURSE_TTC:
            return 14 + starIndex - firstStar;
    }

    return -1;
}

static u8 randomizer_save_default_stage_setting(s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);

    if (star == NULL) {
        return 0;
    }

    switch (star->course) {
        case COURSE_WDW: return 2;
        case COURSE_THI: return 0;
        case COURSE_TTC: return 0;
    }

    return 0;
}

static u8 randomizer_save_stage_setting_count(s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);

    if (star == NULL) {
        return 1;
    }

    return star->course == COURSE_THI ? 3 : 4;
}

static s32 randomizer_save_stage_config_bit_location(s32 bitOffset, s32 *courseIndex, u8 *mask) {
    s32 i;
    s32 slotBits;

    for (i = 0; i < (s32) ARRAY_COUNT(sRandomizerStageConfigSlots); i++) {
        slotBits = sRandomizerStageConfigSlots[i].bitCount;
        if (bitOffset < slotBits) {
            *courseIndex = COURSE_NUM_TO_INDEX(sRandomizerStageConfigSlots[i].course);
            *mask = 1 << (sRandomizerStageConfigSlots[i].firstBit + bitOffset);
            return TRUE;
        }
        bitOffset -= slotBits;
    }

    return FALSE;
}

static s32 randomizer_save_get_stage_config_bit(s32 fileIndex, s32 bitOffset) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 courseIndex;
    u8 mask;

    if (!randomizer_save_stage_config_bit_location(bitOffset, &courseIndex, &mask)) {
        return FALSE;
    }

    return (saveFile->courseStars[courseIndex] & mask) != 0;
}

static void randomizer_save_set_stage_config_bit(s32 fileIndex, s32 bitOffset, s32 value) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 courseIndex;
    u8 mask;

    if (!randomizer_save_stage_config_bit_location(bitOffset, &courseIndex, &mask)) {
        return;
    }

    if (value) {
        saveFile->courseStars[courseIndex] |= mask;
    } else {
        saveFile->courseStars[courseIndex] &= ~mask;
    }

    saveFile->flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

static void randomizer_save_set_star_stage_setting(s32 fileIndex, s32 starIndex, u8 value) {
    s32 slot = randomizer_save_stage_slot_for_star(starIndex);
    s32 bitOffset;

    if (slot < 0) {
        return;
    }

    bitOffset = slot * 2;
    randomizer_save_set_stage_config_bit(fileIndex, bitOffset, value & 1);
    randomizer_save_set_stage_config_bit(fileIndex, bitOffset + 1, (value >> 1) & 1);
}

static void randomizer_save_reset_stage_settings(s32 fileIndex) {
    s32 i;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_star_has_stage_setting(i)) {
            randomizer_save_set_star_stage_setting(fileIndex, i, randomizer_save_default_stage_setting(i));
        }
    }
}

static void randomizer_save_set_course_stage_settings(s32 fileIndex, s32 course, u8 value) {
    s32 firstStar = randomizer_star_first_in_course(course);
    s32 count = randomizer_star_count_in_course(course);
    s32 i;

    if (firstStar < 0) {
        return;
    }

    for (i = 0; i < count; i++) {
        randomizer_save_set_star_stage_setting(fileIndex, firstStar + i, value);
    }
}

static u8 randomizer_state_to_flags(const struct RandomizerSaveState *state) {
    u8 flags = 0;

    if (state->active) {
        flags |= RANDO_FLAG_ACTIVE;
    }
    if (state->complete) {
        flags |= RANDO_FLAG_COMPLETE;
    }
    if (state->unlockCaps) {
        flags |= RANDO_FLAG_UNLOCK_CAPS;
    }
    if (state->unlockCannons) {
        flags |= RANDO_FLAG_UNLOCK_CANNONS;
    }
    if (state->openDoors) {
        flags |= RANDO_FLAG_OPEN_DOORS;
    }
    if (state->difficultyWeights) {
        flags |= RANDO_FLAG_DIFFICULTY_WEIGHTS;
    }
    if (state->orderByStage) {
        flags |= RANDO_FLAG_ORDER_BY_STAGE;
    }

    return flags;
}

void randomizer_save_load_state(s32 fileIndex, struct RandomizerSaveState *state) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    u16 packedRun = (u16) saveFile->capPos[2];
    u16 packedSeed = (u16) saveFile->capPos[1];

    state->active = (saveFile->capArea & RANDO_FLAG_ACTIVE) != 0;
    state->complete = (saveFile->capArea & RANDO_FLAG_COMPLETE) != 0;
    state->unlockCaps = (saveFile->capArea & RANDO_FLAG_UNLOCK_CAPS) != 0;
    state->unlockCannons = (saveFile->capArea & RANDO_FLAG_UNLOCK_CANNONS) != 0;
    state->openDoors = (saveFile->capArea & RANDO_FLAG_OPEN_DOORS) != 0;
    state->difficultyWeights = (saveFile->capArea & RANDO_FLAG_DIFFICULTY_WEIGHTS) != 0;
    state->orderByStage = (saveFile->capArea & RANDO_FLAG_ORDER_BY_STAGE) != 0;
    state->maxStars = packedRun & 0xFF;
    state->currentStarId = (packedRun >> 8) & 0xFF;
    state->timerSeconds = (u16) saveFile->capPos[0];
    state->seed = packedSeed & RANDO_PACKED_SEED_MASK;
    state->wdwWaterLevel = (packedSeed >> RANDO_WDW_WATER_SHIFT) & 0x03;
    state->thiSize = (packedSeed >> RANDO_THI_SIZE_SHIFT) & 0x03;
    state->ttcSpeed = (packedSeed >> RANDO_TTC_SPEED_SHIFT) & 0x03;
}

void randomizer_save_store_state(s32 fileIndex, const struct RandomizerSaveState *state) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);

    saveFile->capLevel = RANDO_SAVE_MAGIC;
    saveFile->capArea = randomizer_state_to_flags(state);
    saveFile->capPos[0] = state->timerSeconds;
    saveFile->capPos[1] = (state->seed & RANDO_PACKED_SEED_MASK)
                         | ((state->wdwWaterLevel & 0x03) << RANDO_WDW_WATER_SHIFT)
                         | ((state->thiSize & 0x03) << RANDO_THI_SIZE_SHIFT)
                         | ((state->ttcSpeed & 0x03) << RANDO_TTC_SPEED_SHIFT);
    saveFile->capPos[2] = ((u16) state->currentStarId << 8) | state->maxStars;
    saveFile->flags &= ~RANDO_LOST_CAP_FLAGS;
    saveFile->flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

void randomizer_save_commit(s32 fileIndex) {
    save_file_do_save(fileIndex);
}

void randomizer_save_set_enabled(s32 fileIndex, s32 starIndex, s32 enabled) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 byteIndex = starIndex >> 3;
    u8 mask = 1 << (starIndex & 7);

    // In randomizer save files, courseCoinScores is reserved for the 120-star
    // enabled bitset. Vanilla coin-score writes are blocked in save_file.c.
    if (byteIndex < 0 || byteIndex >= COURSE_STAGES_COUNT) {
        return;
    }

    if (enabled) {
        saveFile->courseCoinScores[byteIndex] |= mask;
    } else {
        saveFile->courseCoinScores[byteIndex] &= ~mask;
    }

    saveFile->flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

s32 randomizer_save_is_enabled(s32 fileIndex, s32 starIndex) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 byteIndex = starIndex >> 3;
    u8 mask = 1 << (starIndex & 7);

    if (byteIndex < 0 || byteIndex >= COURSE_STAGES_COUNT) {
        return FALSE;
    }

    return (saveFile->courseCoinScores[byteIndex] & mask) != 0;
}

void randomizer_save_set_completed(s32 fileIndex, s32 starIndex, s32 completed) {
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    const struct RandomizerStar *star = randomizer_star_get(starIndex);
    s32 courseIndex;
    u32 starFlag;

    if (star == NULL) {
        return;
    }

    starFlag = 1 << star->starIndex;

    if (star->course == COURSE_NONE) {
        if (completed) {
            saveFile->flags |= STAR_FLAG_TO_SAVE_FLAG(starFlag);
        } else {
            saveFile->flags &= ~STAR_FLAG_TO_SAVE_FLAG(starFlag);
        }
    } else {
        courseIndex = COURSE_NUM_TO_INDEX(star->course);
        if (completed) {
            saveFile->courseStars[courseIndex] |= starFlag;
        } else {
            saveFile->courseStars[courseIndex] &= ~starFlag;
        }
    }

    saveFile->flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

s32 randomizer_save_is_completed(s32 fileIndex, s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);

    if (star == NULL) {
        return FALSE;
    }

    if (star->course == COURSE_NONE) {
        return (save_file_get_star_flags(fileIndex, COURSE_NUM_TO_INDEX(COURSE_NONE))
                & (1 << star->starIndex)) != 0;
    }

    return (save_file_get_star_flags(fileIndex, COURSE_NUM_TO_INDEX(star->course))
            & (1 << star->starIndex)) != 0;
}

void randomizer_save_clear_completed(s32 fileIndex) {
    s32 i;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        randomizer_save_set_completed(fileIndex, i, FALSE);
    }
}

s32 randomizer_save_count_enabled(s32 fileIndex) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i)) {
            count++;
        }
    }

    return count;
}

s32 randomizer_save_count_completed_enabled(s32 fileIndex) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && randomizer_save_is_completed(fileIndex, i)) {
            count++;
        }
    }

    return count;
}

void randomizer_save_apply_unlocks(s32 fileIndex) {
    struct RandomizerSaveState state;
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 course;

    randomizer_save_load_state(fileIndex, &state);

    if (state.openDoors) {
        saveFile->flags |= RANDO_REQUIRED_DOOR_FLAGS;
    }
    if (state.unlockCaps) {
        saveFile->flags |= RANDO_CAP_FLAGS;
    }
    if (state.unlockCannons) {
        for (course = COURSE_MIN; course <= COURSE_STAGES_MAX; course++) {
            saveFile->courseStars[COURSE_NUM_TO_INDEX(course)] |= (1 << 7);
        }
    }

    saveFile->flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

void randomizer_save_apply_120_preset(s32 fileIndex) {
    struct RandomizerSaveState state;
    s32 i;

    randomizer_save_load_state(fileIndex, &state);
    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        randomizer_save_set_enabled(fileIndex, i, TRUE);
    }
    state.maxStars = RANDOMIZER_STAR_COUNT;
    state.complete = FALSE;
    randomizer_save_store_state(fileIndex, &state);
}

void randomizer_save_apply_70_preset(s32 fileIndex) {
    struct RandomizerSaveState state;
    s32 i;

    randomizer_save_load_state(fileIndex, &state);
    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        randomizer_save_set_enabled(fileIndex, i, TRUE);
    }
    state.maxStars = 70;
    state.complete = FALSE;
    randomizer_save_store_state(fileIndex, &state);
}

void randomizer_save_reset_config(s32 fileIndex) {
    struct RandomizerSaveState state;
    struct SaveFile *saveFile = randomizer_save_file(fileIndex);
    s32 i;

    for (i = 0; i < COURSE_STAGES_COUNT; i++) {
        saveFile->courseCoinScores[i] = 0;
    }

    state.active = FALSE;
    state.complete = FALSE;
    state.unlockCaps = TRUE;
    state.unlockCannons = TRUE;
    state.openDoors = TRUE;
    state.difficultyWeights = FALSE;
    state.orderByStage = FALSE;
    state.maxStars = RANDOMIZER_STAR_COUNT;
    state.wdwWaterLevel = 2;
    state.thiSize = 0;
    state.ttcSpeed = 0;
    state.currentStarId = 0;
    state.timerSeconds = 0;
    state.seed = 0x1234;

    randomizer_save_store_state(fileIndex, &state);
    randomizer_save_apply_120_preset(fileIndex);
    randomizer_save_clear_completed(fileIndex);
    randomizer_save_reset_stage_settings(fileIndex);
    randomizer_save_apply_unlocks(fileIndex);
}

void randomizer_save_ensure(s32 fileIndex) {
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return;
    }

    if (randomizer_save_file(fileIndex)->capLevel != RANDO_SAVE_MAGIC) {
        randomizer_save_reset_config(fileIndex);
    }
}

void randomizer_save_advance_wdw_water_level(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(fileIndex, &state);
    state.wdwWaterLevel = (state.wdwWaterLevel + 1) & 0x03;
    randomizer_save_store_state(fileIndex, &state);
    randomizer_save_set_course_stage_settings(fileIndex, COURSE_WDW, state.wdwWaterLevel);
}

void randomizer_save_advance_thi_size(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(fileIndex, &state);
    state.thiSize++;
    if (state.thiSize > 2) {
        state.thiSize = 0;
    }
    randomizer_save_store_state(fileIndex, &state);
    randomizer_save_set_course_stage_settings(fileIndex, COURSE_THI, state.thiSize);
}

void randomizer_save_advance_ttc_speed(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(fileIndex, &state);
    state.ttcSpeed = (state.ttcSpeed + 1) & 0x03;
    randomizer_save_store_state(fileIndex, &state);
    randomizer_save_set_course_stage_settings(fileIndex, COURSE_TTC, state.ttcSpeed);
}

s32 randomizer_save_star_has_stage_setting(s32 starIndex) {
    return randomizer_save_stage_slot_for_star(starIndex) >= 0;
}

u8 randomizer_save_get_star_stage_setting(s32 fileIndex, s32 starIndex) {
    s32 slot = randomizer_save_stage_slot_for_star(starIndex);
    s32 bitOffset;
    u8 value;
    u8 count;

    if (slot < 0) {
        return randomizer_save_default_stage_setting(starIndex);
    }

    bitOffset = slot * 2;
    value = randomizer_save_get_stage_config_bit(fileIndex, bitOffset)
          | (randomizer_save_get_stage_config_bit(fileIndex, bitOffset + 1) << 1);
    count = randomizer_save_stage_setting_count(starIndex);
    if (value >= count) {
        value = randomizer_save_default_stage_setting(starIndex);
    }

    return value;
}

void randomizer_save_advance_star_stage_setting(s32 fileIndex, s32 starIndex) {
    randomizer_save_step_star_stage_setting(fileIndex, starIndex, 1);
}

void randomizer_save_step_star_stage_setting(s32 fileIndex, s32 starIndex, s32 direction) {
    u8 count;
    u8 value;

    if (!randomizer_save_star_has_stage_setting(starIndex)) {
        return;
    }

    count = randomizer_save_stage_setting_count(starIndex);
    value = randomizer_save_get_star_stage_setting(fileIndex, starIndex);
    if (direction < 0) {
        value = value == 0 ? count - 1 : value - 1;
    } else {
        value++;
        if (value >= count) {
            value = 0;
        }
    }

    randomizer_save_set_star_stage_setting(fileIndex, starIndex, value);
}
