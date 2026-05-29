#ifndef RANDOMIZER_TIMER_H
#define RANDOMIZER_TIMER_H

#include <PR/ultratypes.h>

void randomizer_timer_set_seconds(u16 seconds);
void randomizer_timer_start(void);
void randomizer_timer_stop(void);
void randomizer_timer_reset(void);
void randomizer_timer_update(void);
u16 randomizer_timer_get_seconds(void);
s32 randomizer_timer_is_running(void);
void randomizer_timer_render(void);

#endif // RANDOMIZER_TIMER_H
