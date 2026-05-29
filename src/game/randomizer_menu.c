#include <PR/ultratypes.h>

#include "area.h"
#include "audio/external.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "print.h"
#include "randomizer.h"
#include "randomizer_menu.h"
#include "randomizer_save.h"
#include "randomizer_star_data.h"
#include "save_file.h"
#include "sm64.h"
#include "sounds.h"

enum RandomizerMenuPage {
    RANDO_MENU_MAIN,
    RANDO_MENU_PRESETS,
    RANDO_MENU_OPTIONS,
    RANDO_MENU_SAVE,
    RANDO_MENU_CUSTOM,
    RANDO_MENU_UNLOCKS,
    RANDO_MENU_STAGE
};

#define RANDO_MAIN_ITEM_COUNT 6
#define RANDO_PRESET_ITEM_COUNT 3
#define RANDO_OPTIONS_ITEM_COUNT 5
#define RANDO_SAVE_ITEM_COUNT 3
#define RANDO_UNLOCK_ITEM_COUNT 4
#define RANDO_STAGE_ITEM_COUNT 4
#define RANDO_CUSTOM_VISIBLE_STARS 4
#define RANDO_CUSTOM_STAR_NAME_WIDTH 11
#define RANDO_CUSTOM_SCROLL_GAP 3

static s8 sRandoMenuPage = RANDO_MENU_MAIN;
static s8 sRandoMenuSelection = 0;
static s8 sRandoCustomCourse = COURSE_BOB;
static s8 sRandoCustomSelection = 0;
static s8 sRandoConfigFile = 0;
static s8 sRandoCloseRequested = FALSE;
static s8 sRandoStartRequestedFile = -1;

static const s16 sRandoCoursePages[] = {
    COURSE_BOB, COURSE_WF, COURSE_JRB, COURSE_CCM, COURSE_BBH, COURSE_HMC, COURSE_LLL,
    COURSE_SSL, COURSE_DDD, COURSE_SL, COURSE_WDW, COURSE_TTM, COURSE_THI, COURSE_TTC,
    COURSE_RR, COURSE_BITDW, COURSE_BITFS, COURSE_BITS, COURSE_PSS, COURSE_COTMC,
    COURSE_TOTWC, COURSE_VCUTM, COURSE_WMOTR, COURSE_SA, COURSE_NONE
};

static void randomizer_menu_play_move(void) {
    play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
}

static void randomizer_menu_play_decide(void) {
    play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
}

static s32 randomizer_menu_course_page_count(void) {
    return ARRAY_COUNT(sRandoCoursePages);
}

static s32 randomizer_menu_course_page_index(s32 courseNum) {
    s32 i;

    for (i = 0; i < randomizer_menu_course_page_count(); i++) {
        if (sRandoCoursePages[i] == courseNum) {
            return i;
        }
    }

    return 0;
}

static s32 randomizer_menu_current_course(void) {
    return sRandoCoursePages[randomizer_menu_course_page_index(sRandoCustomCourse)];
}

static const char *randomizer_menu_course_short_name(s32 course) {
    switch (course) {
        case COURSE_BOB: return "BOB";
        case COURSE_WF: return "WF";
        case COURSE_JRB: return "JRB";
        case COURSE_CCM: return "CCM";
        case COURSE_BBH: return "BBH";
        case COURSE_HMC: return "HMC";
        case COURSE_LLL: return "LLL";
        case COURSE_SSL: return "SSL";
        case COURSE_DDD: return "DDD";
        case COURSE_SL: return "SL";
        case COURSE_WDW: return "WDW";
        case COURSE_TTM: return "TTM";
        case COURSE_THI: return "THI";
        case COURSE_TTC: return "TTC";
        case COURSE_RR: return "RR";
        case COURSE_BITDW: return "BITDW";
        case COURSE_BITFS: return "BITFS";
        case COURSE_BITS: return "BITS";
        case COURSE_PSS: return "PSS";
        case COURSE_COTMC: return "COTMC";
        case COURSE_TOTWC: return "TOTWC";
        case COURSE_VCUTM: return "VCUTM";
        case COURSE_WMOTR: return "WMOTR";
        case COURSE_SA: return "SA";
        case COURSE_NONE: return "CSS";
    }

    return "???";
}

static void randomizer_menu_change_file(s32 direction) {
    sRandoConfigFile += direction;
    if (sRandoConfigFile < 0) {
        sRandoConfigFile = NUM_SAVE_FILES - 1;
    }
    if (sRandoConfigFile >= NUM_SAVE_FILES) {
        sRandoConfigFile = 0;
    }
    randomizer_save_ensure(sRandoConfigFile);
    randomizer_menu_play_move();
}

static void randomizer_menu_apply_main_action(void) {
    randomizer_save_ensure(sRandoConfigFile);

    switch (sRandoMenuSelection) {
        case 0:
            randomizer_start_run_for_file(sRandoConfigFile);
            sRandoStartRequestedFile = sRandoConfigFile;
            break;
        case 1:
            sRandoMenuPage = RANDO_MENU_PRESETS;
            sRandoMenuSelection = 0;
            break;
        case 2:
            sRandoMenuPage = RANDO_MENU_CUSTOM;
            sRandoCustomSelection = 0;
            break;
        case 3:
            sRandoMenuPage = RANDO_MENU_OPTIONS;
            sRandoMenuSelection = 0;
            break;
        case 4:
            sRandoMenuPage = RANDO_MENU_SAVE;
            sRandoMenuSelection = 0;
            break;
        case 5:
            sRandoCloseRequested = TRUE;
            break;
    }

    randomizer_menu_play_decide();
}

static void randomizer_menu_apply_preset_action(void) {
    switch (sRandoMenuSelection) {
        case 0:
            randomizer_save_apply_70_preset(sRandoConfigFile);
            break;
        case 1:
            randomizer_save_apply_120_preset(sRandoConfigFile);
            break;
        case 2:
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 1;
            break;
    }

    randomizer_menu_play_decide();
}

static void randomizer_menu_apply_options_action(void) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(sRandoConfigFile, &state);
    switch (sRandoMenuSelection) {
        case 0:
            sRandoMenuPage = RANDO_MENU_UNLOCKS;
            sRandoMenuSelection = 0;
            break;
        case 1:
            sRandoMenuPage = RANDO_MENU_STAGE;
            sRandoMenuSelection = 0;
            break;
        case 2:
            state.difficultyWeights = !state.difficultyWeights;
            randomizer_save_store_state(sRandoConfigFile, &state);
            break;
        case 3:
            state.orderByStage = !state.orderByStage;
            randomizer_save_store_state(sRandoConfigFile, &state);
            break;
        case 4:
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 3;
            break;
    }

    randomizer_menu_play_decide();
}

static void randomizer_menu_apply_stage_action(void) {
    switch (sRandoMenuSelection) {
        case 0:
            randomizer_save_advance_wdw_water_level(sRandoConfigFile);
            break;
        case 1:
            randomizer_save_advance_thi_size(sRandoConfigFile);
            break;
        case 2:
            randomizer_save_advance_ttc_speed(sRandoConfigFile);
            break;
        case 3:
            sRandoMenuPage = RANDO_MENU_OPTIONS;
            sRandoMenuSelection = 1;
            break;
    }

    randomizer_menu_play_decide();
}

static void randomizer_menu_apply_save_action(void) {
    switch (sRandoMenuSelection) {
        case 0:
            randomizer_save_commit(sRandoConfigFile);
            break;
        case 1:
            randomizer_save_reset_config(sRandoConfigFile);
            break;
        case 2:
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 4;
            break;
    }

    randomizer_menu_play_decide();
}

static void randomizer_menu_apply_unlock_action(void) {
    struct RandomizerSaveState state;

    randomizer_save_load_state(sRandoConfigFile, &state);
    switch (sRandoMenuSelection) {
        case 0:
            state.unlockCaps = !state.unlockCaps;
            break;
        case 1:
            state.unlockCannons = !state.unlockCannons;
            break;
        case 2:
            state.openDoors = !state.openDoors;
            break;
        case 3:
            sRandoMenuPage = RANDO_MENU_OPTIONS;
            sRandoMenuSelection = 0;
            break;
    }
    randomizer_save_store_state(sRandoConfigFile, &state);
    randomizer_save_apply_unlocks(sRandoConfigFile);
    randomizer_menu_play_decide();
}

static void randomizer_menu_toggle_custom_star(void) {
    s32 course = randomizer_menu_current_course();
    s32 firstStar = randomizer_star_first_in_course(course);
    s32 count = randomizer_star_count_in_course(course);
    s32 starIndex;

    if (firstStar < 0 || count == 0) {
        return;
    }

    if (sRandoCustomSelection >= count) {
        sRandoCustomSelection = count - 1;
    }

    starIndex = firstStar + sRandoCustomSelection;
    randomizer_save_set_enabled(
        sRandoConfigFile, starIndex, !randomizer_save_is_enabled(sRandoConfigFile, starIndex));
    randomizer_menu_play_decide();
}

static s32 randomizer_menu_current_custom_star(void) {
    s32 course = randomizer_menu_current_course();
    s32 firstStar = randomizer_star_first_in_course(course);
    s32 count = randomizer_star_count_in_course(course);

    if (firstStar < 0 || count == 0) {
        return -1;
    }

    if (sRandoCustomSelection >= count) {
        sRandoCustomSelection = count - 1;
    }

    return firstStar + sRandoCustomSelection;
}

static void randomizer_menu_handle_custom_input(void) {
    s32 coursePage;
    s32 courseCount;
    s32 starCount;
    s32 starIndex;
    u16 buttons = gPlayer3Controller->buttonPressed;

    if (buttons & B_BUTTON) {
        sRandoMenuPage = RANDO_MENU_MAIN;
        sRandoMenuSelection = 2;
        randomizer_menu_play_move();
        return;
    }

    if (buttons & A_BUTTON) {
        randomizer_menu_toggle_custom_star();
    }

    if (buttons & (L_CBUTTONS | R_CBUTTONS | U_CBUTTONS | D_CBUTTONS)) {
        starIndex = randomizer_menu_current_custom_star();
        if (randomizer_save_star_has_stage_setting(starIndex)) {
            randomizer_save_step_star_stage_setting(
                sRandoConfigFile, starIndex, (buttons & (L_CBUTTONS | D_CBUTTONS)) ? -1 : 1);
            randomizer_menu_play_decide();
        }
    }

    starCount = randomizer_star_count_in_course(randomizer_menu_current_course());
    if ((buttons & U_JPAD) && sRandoCustomSelection > 0) {
        sRandoCustomSelection--;
        randomizer_menu_play_move();
    }
    if ((buttons & D_JPAD) && sRandoCustomSelection < starCount - 1) {
        sRandoCustomSelection++;
        randomizer_menu_play_move();
    }

    coursePage = randomizer_menu_course_page_index(sRandoCustomCourse);
    courseCount = randomizer_menu_course_page_count();
    if (buttons & L_JPAD) {
        coursePage--;
        if (coursePage < 0) {
            coursePage = courseCount - 1;
        }
        sRandoCustomCourse = sRandoCoursePages[coursePage];
        sRandoCustomSelection = 0;
        randomizer_menu_play_move();
    }
    if (buttons & R_JPAD) {
        coursePage++;
        if (coursePage >= courseCount) {
            coursePage = 0;
        }
        sRandoCustomCourse = sRandoCoursePages[coursePage];
        sRandoCustomSelection = 0;
        randomizer_menu_play_move();
    }
}

void randomizer_menu_open(s32 fileIndex) {
    sRandoMenuPage = RANDO_MENU_MAIN;
    sRandoMenuSelection = 0;
    sRandoConfigFile = fileIndex;
    if (sRandoConfigFile < 0 || sRandoConfigFile >= NUM_SAVE_FILES) {
        sRandoConfigFile = 0;
    }
    sRandoCloseRequested = FALSE;
    sRandoStartRequestedFile = -1;
    randomizer_save_ensure(sRandoConfigFile);
}

void randomizer_menu_handle_input(void) {
    s32 maxItem;

    if (gPlayer3Controller->buttonPressed & L_TRIG) {
        randomizer_menu_change_file(-1);
    }
    if (gPlayer3Controller->buttonPressed & R_TRIG) {
        randomizer_menu_change_file(1);
    }

    if (sRandoMenuPage == RANDO_MENU_CUSTOM) {
        randomizer_menu_handle_custom_input();
        return;
    }

    switch (sRandoMenuPage) {
        case RANDO_MENU_PRESETS:
            maxItem = RANDO_PRESET_ITEM_COUNT;
            break;
        case RANDO_MENU_OPTIONS:
            maxItem = RANDO_OPTIONS_ITEM_COUNT;
            break;
        case RANDO_MENU_SAVE:
            maxItem = RANDO_SAVE_ITEM_COUNT;
            break;
        case RANDO_MENU_UNLOCKS:
            maxItem = RANDO_UNLOCK_ITEM_COUNT;
            break;
        case RANDO_MENU_STAGE:
            maxItem = RANDO_STAGE_ITEM_COUNT;
            break;
        default:
            maxItem = RANDO_MAIN_ITEM_COUNT;
            break;
    }
    if ((gPlayer3Controller->buttonPressed & U_JPAD) && sRandoMenuSelection > 0) {
        sRandoMenuSelection--;
        randomizer_menu_play_move();
    }
    if ((gPlayer3Controller->buttonPressed & D_JPAD) && sRandoMenuSelection < maxItem - 1) {
        sRandoMenuSelection++;
        randomizer_menu_play_move();
    }

    if (gPlayer3Controller->buttonPressed & B_BUTTON) {
        if (sRandoMenuPage == RANDO_MENU_UNLOCKS) {
            sRandoMenuPage = RANDO_MENU_OPTIONS;
            sRandoMenuSelection = 0;
        } else if (sRandoMenuPage == RANDO_MENU_STAGE) {
            sRandoMenuPage = RANDO_MENU_OPTIONS;
            sRandoMenuSelection = 1;
        } else if (sRandoMenuPage == RANDO_MENU_PRESETS) {
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 1;
        } else if (sRandoMenuPage == RANDO_MENU_OPTIONS) {
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 3;
        } else if (sRandoMenuPage == RANDO_MENU_SAVE) {
            sRandoMenuPage = RANDO_MENU_MAIN;
            sRandoMenuSelection = 4;
        } else {
            sRandoCloseRequested = TRUE;
        }
        randomizer_menu_play_move();
    }
    if (gPlayer3Controller->buttonPressed & A_BUTTON) {
        if (sRandoMenuPage == RANDO_MENU_UNLOCKS) {
            randomizer_menu_apply_unlock_action();
        } else if (sRandoMenuPage == RANDO_MENU_STAGE) {
            randomizer_menu_apply_stage_action();
        } else if (sRandoMenuPage == RANDO_MENU_PRESETS) {
            randomizer_menu_apply_preset_action();
        } else if (sRandoMenuPage == RANDO_MENU_OPTIONS) {
            randomizer_menu_apply_options_action();
        } else if (sRandoMenuPage == RANDO_MENU_SAVE) {
            randomizer_menu_apply_save_action();
        } else {
            randomizer_menu_apply_main_action();
        }
    }
}

static void randomizer_menu_print_bool(s16 x, s16 y, s32 value) {
    print_text(x, y, value ? "ON" : "OFF");
}

static void randomizer_menu_print_slot(void) {
    print_text(36, 205, "SLOT");
    print_text_fmt_int(82, 205, "%d", sRandoConfigFile + 1);
    print_text(105, 205, "L R");
}

static void randomizer_menu_print_enabled_count(void) {
    print_text(176, 205, "ON");
    print_text_fmt_int(210, 205, "%d", randomizer_save_count_enabled(sRandoConfigFile));
}

static void randomizer_menu_print_clipped(s16 x, s16 y, const char *text, s32 maxLen) {
    char buffer[25];
    s32 i;
    s32 textContinues = FALSE;

    if (maxLen > (s32) sizeof(buffer) - 1) {
        maxLen = sizeof(buffer) - 1;
    }

    for (i = 0; i < maxLen && text[i] != '\0'; i++) {
        buffer[i] = text[i];
    }
    textContinues = text[i] != '\0';
    if (textContinues && i > 0) {
        buffer[i - 1] = '.';
    }
    buffer[i] = '\0';

    print_text(x, y, buffer);
}

static s32 randomizer_menu_text_length(const char *text) {
    s32 len = 0;

    while (text[len] != '\0') {
        len++;
    }

    return len;
}

static void randomizer_menu_print_scrolled(s16 x, s16 y, const char *text, s32 maxLen, s32 active) {
    char buffer[16];
    s32 len;
    s32 offset;
    s32 source;
    s32 i;

    if (maxLen > (s32) sizeof(buffer) - 1) {
        maxLen = sizeof(buffer) - 1;
    }

    len = randomizer_menu_text_length(text);
    if (!active || len <= maxLen) {
        randomizer_menu_print_clipped(x, y, text, maxLen);
        return;
    }

    offset = (gGlobalTimer / 8) % (len + RANDO_CUSTOM_SCROLL_GAP);
    for (i = 0; i < maxLen; i++) {
        source = offset + i;
        if (source < len) {
            buffer[i] = text[source];
        } else if (source < len + RANDO_CUSTOM_SCROLL_GAP) {
            buffer[i] = ' ';
        } else {
            buffer[i] = text[source - len - RANDO_CUSTOM_SCROLL_GAP];
        }
    }
    buffer[maxLen] = '\0';

    print_text(x, y, buffer);
}

static void randomizer_menu_render_main(const struct RandomizerSaveState *state) {
    static const char *sMainItems[RANDO_MAIN_ITEM_COUNT] = {
        "START RUN",
        "PRESETS",
        "CUSTOM STARS",
        "OPTIONS",
        "SAVE DATA",
        "BACK"
    };
    s32 i;
    s16 y;

    (void) state;

    print_text(82, 33, "RANDOMIZER");
    randomizer_menu_print_slot();
    randomizer_menu_print_enabled_count();

    for (i = 0; i < RANDO_MAIN_ITEM_COUNT; i++) {
        y = 170 - i * 24;
        print_text(36, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(52, y, sMainItems[i]);
    }
}

static void randomizer_menu_render_presets(const struct RandomizerSaveState *state) {
    static const char *sPresetItems[RANDO_PRESET_ITEM_COUNT] = {
        "70 STARS",
        "120 STARS",
        "BACK"
    };
    s32 i;
    s16 y;

    print_text(100, 33, "PRESETS");
    randomizer_menu_print_slot();
    randomizer_menu_print_enabled_count();

    for (i = 0; i < RANDO_PRESET_ITEM_COUNT; i++) {
        y = 154 - i * 32;
        print_text(52, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(70, y, sPresetItems[i]);
        if (i == 0) {
            print_text(246, y, state->maxStars == 70 ? "*" : " ");
        } else if (i == 1) {
            print_text(246, y, state->maxStars == 120 ? "*" : " ");
        }
    }
}

static void randomizer_menu_render_options(const struct RandomizerSaveState *state) {
    static const char *sOptionItems[RANDO_OPTIONS_ITEM_COUNT] = {
        "UNLOCKS",
        "STAGE STATE",
        "DIFF WEIGHT",
        "COURSE ORDER",
        "BACK"
    };
    s32 i;
    s16 y;

    print_text(100, 33, "OPTIONS");
    randomizer_menu_print_slot();
    for (i = 0; i < RANDO_OPTIONS_ITEM_COUNT; i++) {
        y = 170 - i * 24;
        print_text(46, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(64, y, sOptionItems[i]);
        if (i == 2) {
            randomizer_menu_print_bool(244, y, state->difficultyWeights);
        } else if (i == 3) {
            randomizer_menu_print_bool(244, y, state->orderByStage);
        }
    }
}

static const char *randomizer_menu_wdw_water_name(u8 waterLevel) {
    switch (waterLevel & 0x03) {
        case 0: return "LOW";
        case 1: return "MID";
        case 2: return "HIGH";
        case 3: return "TOP";
    }

    return "HIGH";
}

static const char *randomizer_menu_thi_size_name(u8 thiSize) {
    switch (thiSize) {
        case 0: return "HUGE";
        case 1: return "TINY";
        case 2: return "WIG";
    }

    return "HUGE";
}

static const char *randomizer_menu_ttc_speed_name(u8 ttcSpeed) {
    switch (ttcSpeed & 0x03) {
        case 0: return "SLOW";
        case 1: return "FAST";
        case 2: return "RAND";
        case 3: return "STOP";
    }

    return "SLOW";
}

static const char *randomizer_menu_star_stage_name(s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);
    u8 value;

    if (star == NULL || !randomizer_save_star_has_stage_setting(starIndex)) {
        return "";
    }

    value = randomizer_save_get_star_stage_setting(sRandoConfigFile, starIndex);
    switch (star->course) {
        case COURSE_WDW:
            return randomizer_menu_wdw_water_name(value);
        case COURSE_THI:
            return randomizer_menu_thi_size_name(value);
        case COURSE_TTC:
            return randomizer_menu_ttc_speed_name(value);
    }

    return "";
}

static void randomizer_menu_render_stage(const struct RandomizerSaveState *state) {
    static const char *sStageItems[RANDO_STAGE_ITEM_COUNT] = {
        "WDW WATER",
        "THI SIZE",
        "TTC SPEED",
        "BACK"
    };
    s32 i;
    s16 y;

    print_text(86, 33, "STAGE STATE");
    randomizer_menu_print_slot();
    for (i = 0; i < RANDO_STAGE_ITEM_COUNT; i++) {
        y = 162 - i * 30;
        print_text(44, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(62, y, sStageItems[i]);
        if (i == 0) {
            print_text(226, y, randomizer_menu_wdw_water_name(state->wdwWaterLevel));
        } else if (i == 1) {
            print_text(226, y, randomizer_menu_thi_size_name(state->thiSize));
        } else if (i == 2) {
            print_text(226, y, randomizer_menu_ttc_speed_name(state->ttcSpeed));
        }
    }
}

static void randomizer_menu_render_save(void) {
    static const char *sSaveItems[RANDO_SAVE_ITEM_COUNT] = {
        "SAVE",
        "RESET",
        "BACK"
    };
    s32 i;
    s16 y;

    print_text(94, 33, "SAVE DATA");
    randomizer_menu_print_slot();
    randomizer_menu_print_enabled_count();
    for (i = 0; i < RANDO_SAVE_ITEM_COUNT; i++) {
        y = 154 - i * 32;
        print_text(52, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(70, y, sSaveItems[i]);
    }
}

static void randomizer_menu_render_unlocks(const struct RandomizerSaveState *state) {
    static const char *sUnlockItems[RANDO_UNLOCK_ITEM_COUNT] = {
        "CAPS",
        "CANNONS",
        "DOORS",
        "BACK"
    };
    s32 i;
    s16 y;

    print_text(106, 33, "UNLOCKS");
    randomizer_menu_print_slot();
    for (i = 0; i < RANDO_UNLOCK_ITEM_COUNT; i++) {
        y = 162 - i * 30;
        print_text(48, y, i == sRandoMenuSelection ? "-" : " ");
        print_text(66, y, sUnlockItems[i]);
        if (i == 0) {
            randomizer_menu_print_bool(226, y, state->unlockCaps);
        } else if (i == 1) {
            randomizer_menu_print_bool(226, y, state->unlockCannons);
        } else if (i == 2) {
            randomizer_menu_print_bool(226, y, state->openDoors);
        }
    }
}

static void randomizer_menu_render_custom(void) {
    s32 course = randomizer_menu_current_course();
    s32 firstStar = randomizer_star_first_in_course(course);
    s32 count = randomizer_star_count_in_course(course);
    s32 firstVisible;
    s32 visibleCount;
    s32 i;
    s16 y;
    const struct RandomizerStar *star;

    print_text(78, 33, "CUSTOM STARS");
    randomizer_menu_print_slot();
    randomizer_menu_print_enabled_count();
    print_text(28, 176, "L");
    print_text(58, 176, randomizer_menu_course_short_name(course));
    print_text(286, 176, "R");

    firstVisible = (sRandoCustomSelection / RANDO_CUSTOM_VISIBLE_STARS) * RANDO_CUSTOM_VISIBLE_STARS;
    visibleCount = count - firstVisible;
    if (visibleCount > RANDO_CUSTOM_VISIBLE_STARS) {
        visibleCount = RANDO_CUSTOM_VISIBLE_STARS;
    }

    for (i = 0; i < visibleCount; i++) {
        star = randomizer_star_get(firstStar + firstVisible + i);
        y = 142 - i * 30;
        print_text(36, y, (firstVisible + i) == sRandoCustomSelection ? "-" : " ");
        print_text(52, y,
                   randomizer_save_is_enabled(sRandoConfigFile, firstStar + firstVisible + i)
                       ? "ON"
                       : "OFF");
        print_text_fmt_int(86, y, "%d", firstVisible + i + 1);
        randomizer_menu_print_scrolled(112, y, star->name, RANDO_CUSTOM_STAR_NAME_WIDTH,
                                       (firstVisible + i) == sRandoCustomSelection);
        print_text(260, y, randomizer_menu_star_stage_name(firstStar + firstVisible + i));
    }
}

void randomizer_menu_render(UNUSED u8 alpha) {
    struct RandomizerSaveState state;

    randomizer_save_ensure(sRandoConfigFile);
    randomizer_save_load_state(sRandoConfigFile, &state);

    if (sRandoMenuPage == RANDO_MENU_CUSTOM) {
        randomizer_menu_render_custom();
    } else if (sRandoMenuPage == RANDO_MENU_PRESETS) {
        randomizer_menu_render_presets(&state);
    } else if (sRandoMenuPage == RANDO_MENU_OPTIONS) {
        randomizer_menu_render_options(&state);
    } else if (sRandoMenuPage == RANDO_MENU_SAVE) {
        randomizer_menu_render_save();
    } else if (sRandoMenuPage == RANDO_MENU_UNLOCKS) {
        randomizer_menu_render_unlocks(&state);
    } else if (sRandoMenuPage == RANDO_MENU_STAGE) {
        randomizer_menu_render_stage(&state);
    } else {
        randomizer_menu_render_main(&state);
    }
}

s32 randomizer_menu_should_close(void) {
    return sRandoCloseRequested;
}

void randomizer_menu_clear_close(void) {
    sRandoCloseRequested = FALSE;
}

s32 randomizer_menu_consume_start_file(void) {
    s32 file = sRandoStartRequestedFile;

    sRandoStartRequestedFile = -1;
    return file;
}
