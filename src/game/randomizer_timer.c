#include <PR/ultratypes.h>

#include "gfx_dimensions.h"
#include "print.h"
#include "randomizer_timer.h"
#include "sm64.h"

static u32 sRandomizerTimerFrames = 0;
static s32 sRandomizerTimerRunning = FALSE;

#define RANDO_TIMER_MAX_SECONDS 65535

void randomizer_timer_set_seconds(u16 seconds) {
    sRandomizerTimerFrames = (u32) seconds * 30;
}

void randomizer_timer_start(void) {
    sRandomizerTimerRunning = TRUE;
}

void randomizer_timer_stop(void) {
    sRandomizerTimerRunning = FALSE;
}

void randomizer_timer_reset(void) {
    sRandomizerTimerFrames = 0;
}

void randomizer_timer_update(void) {
    if (sRandomizerTimerRunning && sRandomizerTimerFrames < (u32) RANDO_TIMER_MAX_SECONDS * 30) {
        sRandomizerTimerFrames++;
    }
}

u16 randomizer_timer_get_seconds(void) {
    return sRandomizerTimerFrames / 30;
}

s32 randomizer_timer_is_running(void) {
    return sRandomizerTimerRunning;
}

void randomizer_timer_render(void) {
    u16 totalSeconds = randomizer_timer_get_seconds();
    u16 hours = totalSeconds / 3600;
    u16 mins = (totalSeconds / 60) % 60;
    u16 secs = totalSeconds % 60;
    s16 y = 185;

    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(106), y, "%02d", hours);
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(82), y, "'");
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(70), y, "%02d", mins);
    print_text(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(46), y, "'");
    print_text_fmt_int(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(34), y, "%02d", secs);
}
