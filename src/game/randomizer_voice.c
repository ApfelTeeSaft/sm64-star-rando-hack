#include <PR/ultratypes.h>

#include "audio/data.h"
#include "audio/external.h"
#include "audio/seqplayer.h"
#include "config.h"
#include "randomizer_star_data.h"
#include "randomizer_voice.h"
#include "seq_ids.h"
#include "sm64.h"

#define RANDOMIZER_VOICE_DEFAULT_FRAMES 150
#define RANDOMIZER_VOICE_LOAD_TIMEOUT_FRAMES 90
#define RANDOMIZER_VOICE_TIMEOUT_PAD_FRAMES 30
#define RANDOMIZER_VOICE_FINISH_TAIL_FRAMES 8
#define RANDOMIZER_VOICE_PLAYER SEQ_PLAYER_ENV

struct RandomizerVoiceEntry {
    u8 bankIndex;
    u8 tableIndex;
    u16 frames;
};

#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
static const struct RandomizerVoiceEntry sRandomizerVoiceEntries[RANDOMIZER_STAR_COUNT] = {
#include "randomizer_voice_table.inc"
};
#endif

s16 gRandomizerVoiceBankIndex = -1;

static s16 sRandomizerVoiceFramesRemaining = 0;
static s16 sRandomizerVoiceTailFramesRemaining = 0;
static s16 sRandomizerVoiceLoadWaitFrames = 0;
static s16 sRandomizerVoiceSoundId = -1;
static s8 sRandomizerVoiceStartedSequence = FALSE;
static s8 sRandomizerVoiceTriggeredSound = FALSE;
static s8 sRandomizerVoiceSawLayer = FALSE;

#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
static s32 randomizer_voice_layer_finished(void) {
    struct SequenceChannel *channel;
    struct SequenceChannelLayer *layer;

    if (!gSequencePlayers[RANDOMIZER_VOICE_PLAYER].enabled
        || gSequencePlayers[RANDOMIZER_VOICE_PLAYER].seqId != SEQ_RANDOMIZER_VOICE) {
        return sRandomizerVoiceTriggeredSound;
    }

    channel = gSequencePlayers[RANDOMIZER_VOICE_PLAYER].channels[0];
    if (channel == &gSequenceChannelNone || channel == NULL) {
        return sRandomizerVoiceTriggeredSound;
    }

    layer = channel->layers[0];
    if (layer != NULL && layer != NO_LAYER) {
        sRandomizerVoiceSawLayer = TRUE;
        return layer->finished || !layer->enabled;
    }

    return sRandomizerVoiceSawLayer;
}

static void randomizer_voice_try_trigger_sound(void) {
    struct SequenceChannel *channel;

    if (sRandomizerVoiceSoundId < 0 || sRandomizerVoiceTriggeredSound) {
        return;
    }

    if (!gSequencePlayers[RANDOMIZER_VOICE_PLAYER].enabled
        || gSequencePlayers[RANDOMIZER_VOICE_PLAYER].seqId != SEQ_RANDOMIZER_VOICE) {
        return;
    }

    channel = gSequencePlayers[RANDOMIZER_VOICE_PLAYER].channels[0];
    if (channel == &gSequenceChannelNone || channel == NULL) {
        return;
    }

    channel->soundScriptIO[4] = sRandomizerVoiceSoundId;
    channel->soundScriptIO[0] = 1;
    sRandomizerVoiceTriggeredSound = TRUE;
}
#endif

void randomizer_voice_begin(const struct RandomizerStar *star) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    s32 starIndex;
    const struct RandomizerVoiceEntry *entry;
    u16 frames;

    if (star == NULL || star->id == 0 || star->id > RANDOMIZER_STAR_COUNT) {
        randomizer_voice_stop();
        return;
    }

    starIndex = star->id - 1;
    entry = &sRandomizerVoiceEntries[starIndex];
    frames = entry->frames;
    if (frames == 0) {
        frames = RANDOMIZER_VOICE_DEFAULT_FRAMES;
    }

    gRandomizerVoiceBankIndex = entry->bankIndex;
    sRandomizerVoiceSoundId = entry->tableIndex;
    sRandomizerVoiceStartedSequence = TRUE;
    sRandomizerVoiceTriggeredSound = FALSE;
    sRandomizerVoiceSawLayer = FALSE;
    sRandomizerVoiceFramesRemaining = frames + RANDOMIZER_VOICE_TIMEOUT_PAD_FRAMES;
    sRandomizerVoiceTailFramesRemaining = RANDOMIZER_VOICE_FINISH_TAIL_FRAMES;
    sRandomizerVoiceLoadWaitFrames = RANDOMIZER_VOICE_LOAD_TIMEOUT_FRAMES;
    play_music(RANDOMIZER_VOICE_PLAYER, SEQUENCE_ARGS(4, SEQ_RANDOMIZER_VOICE), 0);
    randomizer_voice_try_trigger_sound();
#else
    (void) star;
    randomizer_voice_stop();
#endif
}

s32 randomizer_voice_update(void) {
#if ENABLE_RANDOMIZER_VOICE_PLAYBACK
    randomizer_voice_try_trigger_sound();
#endif

    if (!sRandomizerVoiceTriggeredSound) {
        if (sRandomizerVoiceLoadWaitFrames > 0) {
            sRandomizerVoiceLoadWaitFrames--;
            return FALSE;
        }
        randomizer_voice_stop();
        return TRUE;
    }

    if (randomizer_voice_layer_finished()) {
        if (sRandomizerVoiceTailFramesRemaining > 0) {
            sRandomizerVoiceTailFramesRemaining--;
            return FALSE;
        }
        randomizer_voice_stop();
        return TRUE;
    }

    if (sRandomizerVoiceFramesRemaining > 0) {
        sRandomizerVoiceFramesRemaining--;
    }
    if (sRandomizerVoiceFramesRemaining <= 0) {
        randomizer_voice_stop();
        return TRUE;
    }
    return FALSE;
}

s32 randomizer_voice_active(void) {
    return sRandomizerVoiceFramesRemaining > 0 || sRandomizerVoiceLoadWaitFrames > 0
        || sRandomizerVoiceTailFramesRemaining > 0;
}

void randomizer_voice_stop(void) {
    if (sRandomizerVoiceStartedSequence
        && gSequencePlayers[RANDOMIZER_VOICE_PLAYER].seqId == SEQ_RANDOMIZER_VOICE) {
        sequence_player_disable(&gSequencePlayers[RANDOMIZER_VOICE_PLAYER]);
    }

    sRandomizerVoiceFramesRemaining = 0;
    sRandomizerVoiceTailFramesRemaining = 0;
    sRandomizerVoiceLoadWaitFrames = 0;
    gRandomizerVoiceBankIndex = -1;
    sRandomizerVoiceSoundId = -1;
    sRandomizerVoiceStartedSequence = FALSE;
    sRandomizerVoiceTriggeredSound = FALSE;
    sRandomizerVoiceSawLayer = FALSE;
}
