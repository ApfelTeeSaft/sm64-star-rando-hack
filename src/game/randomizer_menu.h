#ifndef RANDOMIZER_MENU_H
#define RANDOMIZER_MENU_H

#include <PR/ultratypes.h>

void randomizer_menu_open(s32 fileIndex);
void randomizer_menu_handle_input(void);
void randomizer_menu_render(u8 alpha);
s32 randomizer_menu_should_close(void);
void randomizer_menu_clear_close(void);
s32 randomizer_menu_consume_start_file(void);

#endif // RANDOMIZER_MENU_H
