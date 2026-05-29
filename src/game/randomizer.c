#include <PR/ultratypes.h>

#include "area.h"
#include "audio/external.h"
#include "camera.h"
#include "config.h"
#include "dialog_ids.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "mario.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "randomizer.h"
#include "randomizer_save.h"
#include "randomizer_star_data.h"
#include "randomizer_timer.h"
#include "randomizer_voice.h"
#include "save_file.h"
#include "sm64.h"
#include "sounds.h"

static s32 sRandomizerFileIndex = -1;
static s32 sRandomizerActive = FALSE;
static s32 sRandomizerComplete = FALSE;
static s32 sRandomizerCurrentStar = -1;
static s32 sRandomizerForceStarExit = FALSE;
static s32 sRandomizerAnnouncePending = FALSE;
static s32 sRandomizerStartVoiceOnLevelInit = FALSE;
static s32 sRandomizerHoldLevelStart = FALSE;
static s32 sRandomizerResetAttemptPending = FALSE;

static u16 randomizer_next_seed(u16 seed) {
    return seed * 25173 + 13849;
}

static s32 randomizer_weight_for_star(const struct RandomizerStar *star, const struct RandomizerSaveState *state) {
    if (!state->difficultyWeights) {
        return 1;
    }

    return 6 - star->difficulty;
}

static s32 randomizer_remaining_count(s32 fileIndex) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && !randomizer_save_is_completed(fileIndex, i)) {
            count++;
        }
    }

    return count;
}

static s32 randomizer_remaining_count_outside_course(s32 fileIndex, s32 course) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && !randomizer_save_is_completed(fileIndex, i)
            && randomizer_star_get(i)->course != course) {
            count++;
        }
    }

    return count;
}

static s32 randomizer_lowest_remaining_course(s32 fileIndex) {
    s32 i;
    s32 bestCourse = 0x7FFF;
    s32 course;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && !randomizer_save_is_completed(fileIndex, i)) {
            course = randomizer_star_course_order(i);
            if (course < bestCourse) {
                bestCourse = course;
            }
        }
    }

    return bestCourse;
}

static s32 randomizer_castle_area_for_star(const struct RandomizerStar *star) {
    if (star->kind == RANDOMIZER_STAR_TOAD) {
        return star->starIndex == STAR_INDEX_ACT_1 ? 3 : 2;
    }
    if (star->kind == RANDOMIZER_STAR_MIPS) {
        return 3;
    }

    return star->area;
}

static s32 randomizer_pick_next_star(s32 fileIndex, struct RandomizerSaveState *state) {
    s32 i;
    s32 targetCourse;
    s32 avoidCourse = COURSE_NONE;
    s32 avoidCurrentCourse = FALSE;
    s32 totalWeight = 0;
    s32 roll;
    s32 weight;
    const struct RandomizerStar *star;

    if (state->maxStars != 0
        && randomizer_save_count_completed_enabled(fileIndex) >= state->maxStars) {
        return -1;
    }

    if (randomizer_remaining_count(fileIndex) == 0) {
        return -1;
    }

    targetCourse = state->orderByStage ? randomizer_lowest_remaining_course(fileIndex) : -1;
    if (!state->orderByStage && sRandomizerCurrentStar >= 0) {
        avoidCourse = randomizer_star_get(sRandomizerCurrentStar)->course;
        avoidCurrentCourse = randomizer_remaining_count_outside_course(fileIndex, avoidCourse) != 0;
    }

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && !randomizer_save_is_completed(fileIndex, i)
            && (!state->orderByStage || randomizer_star_course_order(i) == targetCourse)
            && (!avoidCurrentCourse || randomizer_star_get(i)->course != avoidCourse)) {
            totalWeight += randomizer_weight_for_star(randomizer_star_get(i), state);
        }
    }

    if (totalWeight == 0) {
        return -1;
    }

    state->seed = randomizer_next_seed(state->seed);
    roll = state->seed % totalWeight;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (randomizer_save_is_enabled(fileIndex, i) && !randomizer_save_is_completed(fileIndex, i)
            && (!state->orderByStage || randomizer_star_course_order(i) == targetCourse)
            && (!avoidCurrentCourse || randomizer_star_get(i)->course != avoidCourse)) {
            star = randomizer_star_get(i);
            weight = randomizer_weight_for_star(star, state);
            if (roll < weight) {
                return i;
            }
            roll -= weight;
        }
    }

    return -1;
}

static void randomizer_get_star_destination(const struct RandomizerStar *star, s32 *area, s32 *warpNode) {
    u8 thiSize;

    *area = star->area;
    *warpNode = star->warpNode;

    if (sRandomizerFileIndex < 0) {
        return;
    }

    if (star->level == LEVEL_THI) {
        thiSize = randomizer_save_get_star_stage_setting(sRandomizerFileIndex, sRandomizerCurrentStar);
        if (thiSize == 1) {
            *area = 2;
            *warpNode = WARP_NODE_0A;
        } else if (thiSize == 2) {
            *area = 3;
            *warpNode = WARP_NODE_0A;
        }
    }
}

static void randomizer_load_runtime_from_save(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(fileIndex, &state);
    sRandomizerFileIndex = fileIndex;
    sRandomizerActive = state.active;
    sRandomizerComplete = state.complete;
    sRandomizerCurrentStar = state.currentStarId == 0 ? -1 : state.currentStarId - 1;
    sRandomizerForceStarExit = FALSE;
    sRandomizerStartVoiceOnLevelInit = FALSE;
    randomizer_timer_set_seconds(state.timerSeconds);
    if (state.active && !state.complete) {
        randomizer_timer_start();
    } else {
        randomizer_timer_stop();
    }
}

static void randomizer_store_runtime_to_save(void) {
    struct RandomizerSaveState state;

    if (sRandomizerFileIndex < 0) {
        return;
    }

    randomizer_save_load_state(sRandomizerFileIndex, &state);
    state.active = sRandomizerActive;
    state.complete = sRandomizerComplete;
    state.currentStarId = sRandomizerCurrentStar < 0 ? 0 : sRandomizerCurrentStar + 1;
    state.timerSeconds = randomizer_timer_get_seconds();
    randomizer_save_store_state(sRandomizerFileIndex, &state);
}

static void randomizer_queue_attempt_reset(void) {
    sRandomizerResetAttemptPending = TRUE;
}

static void randomizer_schedule_voice_on_next_level_init(void) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    if (sRandomizerAnnouncePending) {
        randomizer_voice_stop();
        sRandomizerStartVoiceOnLevelInit = TRUE;
        sRandomizerAnnouncePending = FALSE;
    }
#else
    sRandomizerAnnouncePending = FALSE;
    sRandomizerStartVoiceOnLevelInit = FALSE;
    randomizer_voice_stop();
#endif
}

static void randomizer_start_pending_voice_on_level_init(void) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    if (!sRandomizerStartVoiceOnLevelInit) {
        return;
    }

    sRandomizerStartVoiceOnLevelInit = FALSE;
    if (sRandomizerActive && sRandomizerCurrentStar >= 0) {
        randomizer_voice_begin(randomizer_star_get(sRandomizerCurrentStar));
    }
#else
    sRandomizerStartVoiceOnLevelInit = FALSE;
    randomizer_voice_stop();
#endif
}

static void randomizer_reset_attempt_state(void) {
    if (!sRandomizerActive) {
        return;
    }

    gMarioState->numCoins = 0;
    gMarioState->health = 0x880;
    gMarioState->healCounter = 0;
    gMarioState->hurtCounter = 0;
    gMarioState->invincTimer = 0;
    gMarioState->numLives = 4;
    gHudDisplay.coins = 0;
    gHudDisplay.wedges = 8;
    gHudDisplay.lives = 4;
}

static void randomizer_set_current_star_from_pick(s32 fileIndex, struct RandomizerSaveState *state) {
    s32 pickedStar = randomizer_pick_next_star(fileIndex, state);

    if (pickedStar < 0) {
        state->active = FALSE;
        state->complete = TRUE;
        state->currentStarId = 0;
        sRandomizerActive = FALSE;
        sRandomizerComplete = TRUE;
        sRandomizerCurrentStar = -1;
        sRandomizerAnnouncePending = FALSE;
        sRandomizerStartVoiceOnLevelInit = FALSE;
        randomizer_voice_stop();
        randomizer_timer_stop();
    } else {
        state->active = TRUE;
        state->complete = FALSE;
        state->currentStarId = pickedStar + 1;
        sRandomizerActive = TRUE;
        sRandomizerComplete = FALSE;
        sRandomizerCurrentStar = pickedStar;
        sRandomizerAnnouncePending = TRUE;
        sRandomizerStartVoiceOnLevelInit = FALSE;
        randomizer_queue_attempt_reset();
        randomizer_voice_stop();
    }

    state->timerSeconds = randomizer_timer_get_seconds();
    randomizer_save_store_state(fileIndex, state);
}

void randomizer_start_run_for_file(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_ensure(fileIndex);
    randomizer_save_load_state(fileIndex, &state);
    randomizer_save_clear_completed(fileIndex);
    state.active = TRUE;
    state.complete = FALSE;
    state.currentStarId = 0;
    state.timerSeconds = 0;
    state.seed = (u16) (gGlobalTimer ^ ((fileIndex + 1) * 0x3457));
    randomizer_timer_reset();
    randomizer_timer_start();
    randomizer_save_apply_unlocks(fileIndex);
    randomizer_set_current_star_from_pick(fileIndex, &state);
    randomizer_save_commit(fileIndex);
    randomizer_load_runtime_from_save(fileIndex);
}

void randomizer_select_file(s32 fileIndex) {
    struct RandomizerSaveState state;

    randomizer_save_ensure(fileIndex);
    randomizer_save_load_state(fileIndex, &state);

    if (!state.active || state.complete || state.currentStarId == 0) {
        randomizer_start_run_for_file(fileIndex);
        return;
    }

    randomizer_save_apply_unlocks(fileIndex);
    randomizer_save_commit(fileIndex);
    randomizer_load_runtime_from_save(fileIndex);
    if (sRandomizerActive && sRandomizerCurrentStar >= 0) {
        sRandomizerAnnouncePending = TRUE;
        randomizer_queue_attempt_reset();
    }
}

s32 randomizer_override_initial_level(s32 levelNum) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return levelNum;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    randomizer_apply_current_act();
    randomizer_queue_attempt_reset();
    randomizer_schedule_voice_on_next_level_init();

    return star->level;
}

void randomizer_apply_current_act(void) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    gCurrActNum = star->act;
    gDialogCourseActNum = star->act;
}

s32 randomizer_should_skip_act_selector(void) {
    return sRandomizerActive && sRandomizerCurrentStar >= 0;
}

static void randomizer_apply_castle_spawn_override(void) {
    const struct RandomizerStar *star;
    s32 area;
    s32 warpNode;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    if (star->kind == RANDOMIZER_STAR_TOAD) {
        if (star->starIndex == STAR_INDEX_ACT_1) {
            gPlayerSpawnInfos[0].areaIndex = 3;
            vec3s_set(gPlayerSpawnInfos[0].startPos, -1382, -819, -4150);
            vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, -0x8000, 0);
        } else {
            gPlayerSpawnInfos[0].areaIndex = 2;
            vec3s_set(gPlayerSpawnInfos[0].startPos, -659, 1613, -350);
            vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, -0x8000, 0);
        }
    } else if (star->kind == RANDOMIZER_STAR_MIPS) {
        gPlayerSpawnInfos[0].areaIndex = 3;
        vec3s_set(gPlayerSpawnInfos[0].startPos, -1382, -819, -4150);
        vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, -0x8000, 0);
    } else {
        randomizer_get_star_destination(star, &area, &warpNode);
        if (star->level == LEVEL_THI && area != 1) {
            gPlayerSpawnInfos[0].areaIndex = area;
            if (area == 2) {
                vec3s_set(gPlayerSpawnInfos[0].startPos, -2211, 110, 2212);
                vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, (s16) (149 << 8), 0);
            } else {
                vec3s_set(gPlayerSpawnInfos[0].startPos, 512, 1024, 2150);
                vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, (s16) (180 << 8), 0);
            }
        }
    }
}

void randomizer_on_level_init(void) {
    randomizer_apply_castle_spawn_override();
    randomizer_apply_current_act();
    randomizer_start_pending_voice_on_level_init();
    if (sRandomizerResetAttemptPending) {
        randomizer_reset_attempt_state();
        sRandomizerResetAttemptPending = FALSE;
    }
    if (sRandomizerActive && sRandomizerFileIndex >= 0 && sRandomizerCurrentStar >= 0
        && randomizer_star_get(sRandomizerCurrentStar)->level == LEVEL_TTC) {
        gTTCSpeedSetting = randomizer_save_get_star_stage_setting(sRandomizerFileIndex, sRandomizerCurrentStar) & 0x03;
    }
}

void randomizer_on_level_update(void) {
    randomizer_timer_update();
    if (randomizer_voice_active()) {
        randomizer_voice_update();
    }
    if (sRandomizerFileIndex >= 0 && (gGlobalTimer & 0x3F) == 0) {
        randomizer_store_runtime_to_save();
    }
}

void randomizer_on_star_collected(s32 courseNum, s32 levelNum, s32 starIndex) {
    struct RandomizerSaveState state;
    s32 collectedStar;

    if (!sRandomizerActive || sRandomizerFileIndex < 0) {
        return;
    }

    collectedStar = randomizer_star_index_from_course_star(courseNum, levelNum, starIndex);
    if (collectedStar < 0) {
        return;
    }

    randomizer_save_set_completed(sRandomizerFileIndex, collectedStar, TRUE);

    if (collectedStar == sRandomizerCurrentStar) {
        randomizer_save_load_state(sRandomizerFileIndex, &state);
        state.currentStarId = 0;
        sRandomizerForceStarExit = TRUE;
        randomizer_set_current_star_from_pick(sRandomizerFileIndex, &state);
        if (sRandomizerComplete) {
            play_sound(SOUND_MENU_THANK_YOU_PLAYING_MY_GAME, gGlobalSoundSource);
        }
    }

    randomizer_store_runtime_to_save();
    randomizer_save_commit(sRandomizerFileIndex);
}

s32 randomizer_should_force_star_exit(void) {
    return sRandomizerForceStarExit;
}

s32 randomizer_handle_star_exit_warp(void) {
    const struct RandomizerStar *star;
    s32 area;
    s32 warpNode;

    if (sRandomizerComplete) {
        initiate_warp(LEVEL_CASTLE_GROUNDS, 1, WARP_NODE_0A, 0);
        play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 16, 255, 255, 255);
        level_set_transition(45, NULL);
        return TRUE;
    }

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return FALSE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    randomizer_apply_current_act();
    randomizer_schedule_voice_on_next_level_init();
    if (star->level == LEVEL_CASTLE) {
        initiate_warp_force_level(LEVEL_CASTLE, randomizer_castle_area_for_star(star), WARP_NODE_32, 0);
    } else {
        randomizer_get_star_destination(star, &area, &warpNode);
        initiate_warp_force_level(star->level, area, warpNode, 0);
    }
    if (gWarpTransition.pauseRendering) {
        level_set_transition(1, NULL);
    } else {
        if (!gWarpTransition.isActive) {
            play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 8, 0, 0, 0);
        }
        level_set_transition(8, NULL);
    }
    sRandomizerForceStarExit = FALSE;
    return TRUE;
}

s32 randomizer_handle_quick_retry(void) {
    const struct RandomizerStar *star;
    s32 area;
    s32 warpNode;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return FALSE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    randomizer_apply_current_act();
    randomizer_queue_attempt_reset();
    if (star->level == LEVEL_CASTLE) {
        initiate_warp_force_level(LEVEL_CASTLE, randomizer_castle_area_for_star(star), WARP_NODE_32, 0);
    } else {
        randomizer_get_star_destination(star, &area, &warpNode);
        initiate_warp_force_level(star->level, area, warpNode, 0);
    }
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 6, 0, 0, 0);
    level_set_transition(8, NULL);
    play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
    return TRUE;
}

void randomizer_transition_update(s16 *timer) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    if (randomizer_voice_update()) {
        *timer = 0;
    } else {
        *timer = 2;
    }
#else
    *timer = 0;
#endif
}

s32 randomizer_should_block_level_start(void) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    return randomizer_voice_active();
#else
    return FALSE;
#endif
}

void randomizer_begin_blocking_level_start(void) {
    sRandomizerHoldLevelStart = TRUE;
}

s32 randomizer_finish_blocking_level_start(void) {
    if (!sRandomizerHoldLevelStart || randomizer_voice_active()) {
        return FALSE;
    }

    sRandomizerHoldLevelStart = FALSE;
    return TRUE;
}

static s32 randomizer_hud_append_number(char *buffer, s32 pos, s32 value) {
    char digits[5];
    s32 len = 0;

    if (value <= 0) {
        buffer[pos++] = '0';
        return pos;
    }

    while (value > 0 && len < (s32) sizeof(digits)) {
        digits[len++] = '0' + (value % 10);
        value /= 10;
    }

    while (len > 0) {
        buffer[pos++] = digits[--len];
    }

    return pos;
}

static s32 randomizer_required_star_count(void) {
    struct RandomizerSaveState state;
    s32 enabled;
    s32 required;

    if (sRandomizerFileIndex < 0) {
        return 0;
    }

    randomizer_save_load_state(sRandomizerFileIndex, &state);
    enabled = randomizer_save_count_enabled(sRandomizerFileIndex);
    required = enabled;
    if (state.maxStars != 0 && state.maxStars < required) {
        required = state.maxStars;
    }

    return required;
}

static void randomizer_render_star_progress_hud(void) {
    char buffer[12];
    s32 completed;
    s32 required;
    s32 len = 0;
    s16 y = 169;

    if (sRandomizerFileIndex < 0) {
        return;
    }

    required = randomizer_required_star_count();
    if (required <= 0) {
        return;
    }

    completed = randomizer_save_count_completed_enabled(sRandomizerFileIndex);
    if (completed > required) {
        completed = required;
    }

    len = randomizer_hud_append_number(buffer, len, completed);
    buffer[len++] = '`';
    len = randomizer_hud_append_number(buffer, len, required);
    buffer[len] = '\0';

    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(10 + len * 12), y, buffer);
}

void randomizer_render_hud(void) {
    if (sRandomizerActive || sRandomizerComplete) {
        randomizer_timer_render();
        randomizer_render_star_progress_hud();
    }
}

s32 randomizer_is_active(void) {
    return sRandomizerActive;
}

s32 randomizer_is_complete(void) {
    return sRandomizerComplete;
}

s32 randomizer_current_star_index(void) {
    return sRandomizerCurrentStar;
}

s32 randomizer_current_star_matches(s32 courseNum, UNUSED s32 levelNum, s32 starIndex) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return TRUE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    return star->course == courseNum && star->starIndex == starIndex;
}

s32 randomizer_current_star_is_100_coin(void) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return FALSE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    return star->kind == RANDOMIZER_STAR_100_COINS;
}

s32 randomizer_current_star_uses_red_coins(void) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return TRUE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    if (star->kind == RANDOMIZER_STAR_100_COINS || star->kind == RANDOMIZER_STAR_BOWSER_RED_COINS) {
        return TRUE;
    }
    if (star->kind == RANDOMIZER_STAR_SECRET) {
        return star->course != COURSE_PSS;
    }

    switch (star->course) {
        case COURSE_BOB: return star->starIndex == STAR_INDEX_ACT_4;
        case COURSE_WF:  return star->starIndex == STAR_INDEX_ACT_4;
        case COURSE_JRB: return star->starIndex == STAR_INDEX_ACT_4;
        case COURSE_CCM: return star->starIndex == STAR_INDEX_ACT_4;
        case COURSE_BBH: return star->starIndex == STAR_INDEX_ACT_4;
        case COURSE_HMC: return star->starIndex == STAR_INDEX_ACT_2;
        case COURSE_LLL: return star->starIndex == STAR_INDEX_ACT_3;
        case COURSE_SSL: return star->starIndex == STAR_INDEX_ACT_5;
        case COURSE_DDD: return star->starIndex == STAR_INDEX_ACT_3;
        case COURSE_SL:  return star->starIndex == STAR_INDEX_ACT_5;
        case COURSE_WDW: return star->starIndex == STAR_INDEX_ACT_5;
        case COURSE_TTM: return star->starIndex == STAR_INDEX_ACT_3;
        case COURSE_THI: return star->starIndex == STAR_INDEX_ACT_5;
        case COURSE_TTC: return star->starIndex == STAR_INDEX_ACT_6;
        case COURSE_RR:  return star->starIndex == STAR_INDEX_ACT_3;
    }

    return FALSE;
}

s32 randomizer_wdw_water_level(void) {
    u8 waterLevel;

    if (!sRandomizerActive || sRandomizerFileIndex < 0) {
        return 0x7FFFFFFF;
    }

    waterLevel = randomizer_save_get_star_stage_setting(sRandomizerFileIndex, sRandomizerCurrentStar);
    switch (waterLevel & 0x03) {
        case 0: return 40;
        case 1: return 1024;
        case 2: return 1792;
        case 3: return 2560;
    }

    return 1792;
}

s32 randomizer_current_star_is_toad_dialog(s32 dialogID) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return FALSE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    if (star->kind != RANDOMIZER_STAR_TOAD) {
        return FALSE;
    }

    return (star->starIndex == STAR_INDEX_ACT_1 && dialogID == DIALOG_082)
        || (star->starIndex == STAR_INDEX_ACT_2 && dialogID == DIALOG_076)
        || (star->starIndex == STAR_INDEX_ACT_3 && dialogID == DIALOG_083);
}

s32 randomizer_current_star_is_mips(s32 mipsBp) {
    const struct RandomizerStar *star;

    if (!sRandomizerActive || sRandomizerCurrentStar < 0) {
        return FALSE;
    }

    star = randomizer_star_get(sRandomizerCurrentStar);
    if (star->kind != RANDOMIZER_STAR_MIPS) {
        return FALSE;
    }

    return (star->starIndex == STAR_INDEX_ACT_4 && mipsBp == 0)
        || (star->starIndex == STAR_INDEX_ACT_5 && mipsBp == 1);
}
