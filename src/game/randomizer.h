#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <PR/ultratypes.h>

struct MarioState;
struct Object;

void randomizer_select_file(s32 fileIndex);
void randomizer_start_run_for_file(s32 fileIndex);
s32 randomizer_override_initial_level(s32 levelNum);
void randomizer_apply_current_act(void);
s32 randomizer_should_skip_act_selector(void);
void randomizer_on_level_init(void);
void randomizer_on_level_update(void);
void randomizer_on_star_collected(s32 courseNum, s32 levelNum, s32 starIndex);
s32 randomizer_should_force_star_exit(void);
s32 randomizer_handle_star_exit_warp(void);
s32 randomizer_handle_quick_retry(void);
void randomizer_transition_update(s16 *timer);
s32 randomizer_should_block_level_start(void);
void randomizer_begin_blocking_level_start(void);
s32 randomizer_finish_blocking_level_start(void);
void randomizer_render_hud(void);
s32 randomizer_is_active(void);
s32 randomizer_is_complete(void);
s32 randomizer_current_star_index(void);
s32 randomizer_current_star_matches(s32 courseNum, s32 levelNum, s32 starIndex);
s32 randomizer_current_star_uses_red_coins(void);
s32 randomizer_current_star_is_100_coin(void);
s32 randomizer_wdw_water_level(void);
s32 randomizer_current_star_is_toad_dialog(s32 dialogID);
s32 randomizer_current_star_is_mips(s32 mipsBp);

#endif // RANDOMIZER_H
