#ifndef RANDOMIZER_SAVE_H
#define RANDOMIZER_SAVE_H

#include <PR/ultratypes.h>

#define RANDO_SAVE_MAGIC 0x52

struct RandomizerSaveState {
    u8 active;
    u8 complete;
    u8 unlockCaps;
    u8 unlockCannons;
    u8 openDoors;
    u8 difficultyWeights;
    u8 orderByStage;
    u8 maxStars;
    u8 wdwWaterLevel;
    u8 thiSize;
    u8 ttcSpeed;
    u8 currentStarId;
    u16 timerSeconds;
    u16 seed;
};

void randomizer_save_ensure(s32 fileIndex);
void randomizer_save_load_state(s32 fileIndex, struct RandomizerSaveState *state);
void randomizer_save_store_state(s32 fileIndex, const struct RandomizerSaveState *state);
void randomizer_save_commit(s32 fileIndex);
void randomizer_save_reset_config(s32 fileIndex);
void randomizer_save_apply_70_preset(s32 fileIndex);
void randomizer_save_apply_120_preset(s32 fileIndex);
void randomizer_save_set_enabled(s32 fileIndex, s32 starIndex, s32 enabled);
s32 randomizer_save_is_enabled(s32 fileIndex, s32 starIndex);
void randomizer_save_set_completed(s32 fileIndex, s32 starIndex, s32 completed);
s32 randomizer_save_is_completed(s32 fileIndex, s32 starIndex);
void randomizer_save_clear_completed(s32 fileIndex);
s32 randomizer_save_count_enabled(s32 fileIndex);
s32 randomizer_save_count_completed_enabled(s32 fileIndex);
void randomizer_save_apply_unlocks(s32 fileIndex);
void randomizer_save_advance_wdw_water_level(s32 fileIndex);
void randomizer_save_advance_thi_size(s32 fileIndex);
void randomizer_save_advance_ttc_speed(s32 fileIndex);
s32 randomizer_save_star_has_stage_setting(s32 starIndex);
u8 randomizer_save_get_star_stage_setting(s32 fileIndex, s32 starIndex);
void randomizer_save_advance_star_stage_setting(s32 fileIndex, s32 starIndex);
void randomizer_save_step_star_stage_setting(s32 fileIndex, s32 starIndex, s32 direction);

#endif // RANDOMIZER_SAVE_H
