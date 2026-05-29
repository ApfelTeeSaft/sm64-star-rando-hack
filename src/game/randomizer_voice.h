#ifndef RANDOMIZER_VOICE_H
#define RANDOMIZER_VOICE_H

#include <PR/ultratypes.h>

struct RandomizerStar;

extern s16 gRandomizerVoiceBankIndex;

void randomizer_voice_begin(const struct RandomizerStar *star);
s32 randomizer_voice_update(void);
s32 randomizer_voice_active(void);
void randomizer_voice_stop(void);

#endif // RANDOMIZER_VOICE_H
