#!/usr/bin/env python3
"""Prepare randomizer objective voice assets for the SM64 audio pipeline.

The normal build converts AIFF samples to N64 ADPCM with:

    aiff_extract_codebook -> vadpcm_enc -> assemble_sound.py

This helper prepares all 120 objective announcements. By default it writes the
generated audio metadata into assets/randomizer_voice so the normal ROM build
stays boot-safe. Pass --register to install the generated sequence, sound banks,
and samples into sound/ for experimental playback builds.
"""

from __future__ import annotations

import argparse
import json
import math
import re
import struct
import wave
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
STAR_DATA = ROOT / "src/game/randomizer_star_data.c"
VOICE_BUILD_SAMPLE_DIR = ROOT / "sound/samples/rando_voice"
VOICE_STAGED_SAMPLE_DIR = ROOT / "assets/randomizer_voice/generated_samples/rando_voice"
VOICE_RAW_DIR = ROOT / "assets/randomizer_voice/raw"
SEQUENCES_JSON = ROOT / "sound/sequences.json"
VOICE_BUILD_SEQUENCE_FILE = ROOT / "sound/sequences/23_randomizer_voice.s"
VOICE_STAGED_SEQUENCE_FILE = ROOT / "assets/randomizer_voice/generated_sequences/23_randomizer_voice.s"
VOICE_STAGED_BANK_DIR = ROOT / "assets/randomizer_voice/generated_sound_banks"
LEGACY_VOICE_SEQUENCE_INC = ROOT / "sound/sequences/randomizer_voice.inc"
VOICE_DURATION_INC = ROOT / "src/game/randomizer_voice_table.inc"
LEGACY_VOICE_DURATION_INC = ROOT / "src/game/randomizer_voice_table.inc.c"
VOICE_MANIFEST = ROOT / "assets/randomizer_voice/raw/README.md"

TARGET_RATE = 16000
DEFAULT_PLACEHOLDER_SECONDS = 0.12
DEFAULT_PLACEHOLDER_FRAMES = 24
VOICE_END_PAD_FRAMES = 18
VOICE_BANK_START = 0x26
VOICE_BANK_CHUNK_SIZE = 8
VOICE_SEQUENCE_KEY = "23_randomizer_voice"
VOICE_MAX_BANK_SET_BYTES = 0x100
VOICE_MAX_SOUND_BANKS = 0x40

COURSE_SLUGS = {
    "Bob-omb Battlefield": "bob",
    "Whomp's Fortress": "wf",
    "Jolly Roger Bay": "jrb",
    "Cool Cool Mountain": "ccm",
    "Big Boo's Haunt": "bbh",
    "Hazy Maze Cave": "hmc",
    "Lethal Lava Land": "lll",
    "Shifting Sand Land": "ssl",
    "Dire Dire Docks": "ddd",
    "Snowman's Land": "sl",
    "Wet-Dry World": "wdw",
    "Tall Tall Mountain": "ttm",
    "Tiny-Huge Island": "thi",
    "Tick Tock Clock": "ttc",
    "Rainbow Ride": "rr",
    "Bowser in the Dark World": "bitdw",
    "Bowser in the Fire Sea": "bitfs",
    "Bowser in the Sky": "bits",
    "Princess's Secret Slide": "pss",
    "Cavern of the Metal Cap": "cotmc",
    "Tower of the Wing Cap": "totwc",
    "Vanish Cap Under the Moat": "vcutm",
    "Wing Mario Over the Rainbow": "wmotr",
    "Secret Aquarium": "sa",
    "Castle Secret Stars": "castle",
}

RED_COIN_NAME_OVERRIDES = {
    "8-Coin Puzzle with 15 Pieces",
    "Coins Amassed in a Maze",
}


def slugify(text: str) -> str:
    text = text.lower()
    text = text.replace("100", "one_hundred")
    text = re.sub(r"[^a-z0-9]+", "_", text)
    text = re.sub(r"_+", "_", text).strip("_")
    return text


def write_text_lf(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as out:
        out.write(text)


@dataclass
class StarVoice:
    index: int
    star_id: int
    star_index: str
    kind: str
    course_name: str
    name: str

    @property
    def course_slug(self) -> str:
        return COURSE_SLUGS.get(self.course_name, slugify(self.course_name))

    @property
    def legacy_slug(self) -> str:
        return slugify(self.name) or f"star_{self.star_id:03d}"

    @property
    def red_coin_objective(self) -> bool:
        name = self.name.lower()
        return (
            self.kind == "RANDOMIZER_STAR_BOWSER_RED_COINS"
            or "red coin" in name
            or self.name in RED_COIN_NAME_OVERRIDES
        )

    @property
    def coin_objective(self) -> bool:
        return self.kind == "RANDOMIZER_STAR_100_COINS" or self.red_coin_objective

    @property
    def slug(self) -> str:
        if self.coin_objective:
            return f"{self.course_slug}_{self.legacy_slug}"
        return self.legacy_slug

    @property
    def stem(self) -> str:
        return f"{self.index:03d}_{self.slug}"

    @property
    def legacy_stem(self) -> str:
        return f"{self.index:03d}_{self.legacy_slug}"

    @property
    def recording_label(self) -> str:
        if self.coin_objective:
            return f"{self.course_name} - {self.name}"
        return self.name


def parse_stars(path: Path = STAR_DATA) -> list[StarVoice]:
    text = path.read_text(encoding="utf-8")
    pattern = re.compile(
        r"RSTAR\(\s*(\d+),\s*[^,]+,\s*[^,]+,\s*\d+,\s*([^,\s]+),\s*\d+,\s*\d+,\s*([^,\s]+),\s*"
        r"\"([^\"]+)\"\s*,\s*\"([^\"]+)\"\)"
    )
    stars: list[StarVoice] = []

    for match in pattern.finditer(text):
        star_id = int(match.group(1))
        stars.append(StarVoice(
            len(stars),
            star_id,
            match.group(2),
            match.group(3),
            match.group(4),
            match.group(5),
        ))

    if len(stars) != 120:
        raise SystemExit(f"expected 120 randomizer stars, found {len(stars)} in {path}")

    return stars


def read_wav_mono(path: Path) -> tuple[int, list[float]]:
    with wave.open(str(path), "rb") as wav:
        channels = wav.getnchannels()
        sample_width = wav.getsampwidth()
        rate = wav.getframerate()
        frames = wav.getnframes()
        raw = wav.readframes(frames)

    if sample_width not in (1, 2, 3, 4):
        raise SystemExit(f"unsupported WAV sample width {sample_width} in {path}")

    samples: list[float] = []
    frame_size = channels * sample_width
    for frame_start in range(0, len(raw), frame_size):
        total = 0.0
        for channel in range(channels):
            pos = frame_start + channel * sample_width
            chunk = raw[pos : pos + sample_width]
            if sample_width == 1:
                value = (chunk[0] - 128) / 128.0
            elif sample_width == 2:
                value = struct.unpack("<h", chunk)[0] / 32768.0
            elif sample_width == 3:
                value_int = int.from_bytes(chunk, "little", signed=False)
                if value_int & 0x800000:
                    value_int -= 0x1000000
                value = value_int / 8388608.0
            else:
                value = struct.unpack("<i", chunk)[0] / 2147483648.0
            total += value
        samples.append(total / channels)

    return rate, samples


def trim_silence(samples: list[float], rate: int, threshold: float = 0.0015, pad_ms: int = 35) -> list[float]:
    if not samples:
        return samples

    first = 0
    last = len(samples) - 1
    while first < len(samples) and abs(samples[first]) < threshold:
        first += 1
    while last > first and abs(samples[last]) < threshold:
        last -= 1

    if first >= len(samples):
        return samples[: max(1, min(len(samples), rate // 10))]

    pad = int(rate * pad_ms / 1000)
    first = max(0, first - pad)
    last = min(len(samples) - 1, last + pad)
    return samples[first : last + 1]


def resample_linear(samples: list[float], src_rate: int, dst_rate: int) -> list[float]:
    if src_rate == dst_rate:
        return samples[:]
    if not samples:
        return []

    dst_len = max(1, int(round(len(samples) * dst_rate / src_rate)))
    scale = src_rate / dst_rate
    out: list[float] = []

    for i in range(dst_len):
        src_pos = i * scale
        left = int(src_pos)
        frac = src_pos - left
        if left >= len(samples) - 1:
            value = samples[-1]
        else:
            value = samples[left] * (1.0 - frac) + samples[left + 1] * frac
        out.append(value)

    return out


def float_to_s16(samples: list[float], normalize: bool) -> list[int]:
    if normalize:
        peak = max((abs(s) for s in samples), default=0.0)
        gain = 0.95 / peak if peak > 0.00001 else 1.0
    else:
        gain = 1.0

    out: list[int] = []
    for sample in samples:
        value = int(max(-1.0, min(1.0, sample * gain)) * 32767.0)
        out.append(value)
    return out


def extended80(value: float) -> bytes:
    if value <= 0:
        return b"\0" * 10

    mantissa, exponent = math.frexp(value)
    exponent += 16382
    mantissa = math.ldexp(mantissa, 32)
    hi = int(mantissa)
    mantissa = math.ldexp(mantissa - hi, 32)
    lo = int(mantissa)
    return struct.pack(">HII", exponent, hi, lo)


def write_aiff_pcm16(path: Path, samples: list[int], rate: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    sound_data = b"".join(struct.pack(">h", sample) for sample in samples)
    comm = struct.pack(">hIh", 1, len(samples), 16) + extended80(float(rate))
    ssnd = struct.pack(">II", 0, 0) + sound_data

    chunks = [(b"COMM", comm), (b"SSND", ssnd)]
    form_size = 4
    for _, payload in chunks:
        form_size += 8 + len(payload) + (len(payload) & 1)

    with path.open("wb") as out:
        out.write(b"FORM")
        out.write(struct.pack(">I", form_size))
        out.write(b"AIFF")
        for chunk_id, payload in chunks:
            out.write(chunk_id)
            out.write(struct.pack(">I", len(payload)))
            out.write(payload)
            if len(payload) & 1:
                out.write(b"\0")


def placeholder_samples(seconds: float = DEFAULT_PLACEHOLDER_SECONDS, tone_hz: float = 880.0) -> list[int]:
    count = max(64, int(TARGET_RATE * seconds))
    attack = max(1, count // 12)
    release = max(1, count // 4)
    samples: list[int] = []

    for i in range(count):
        if i < attack:
            env = i / attack
        elif i > count - release:
            env = max(0.0, (count - i) / release)
        else:
            env = 1.0
        samples.append(int(math.sin(2.0 * math.pi * tone_hz * i / TARGET_RATE) * 12000.0 * env))

    return samples


def source_candidates(star: StarVoice, raw_dir: Path) -> list[Path]:
    candidates = [
        raw_dir / f"{star.stem}.wav",
        raw_dir / f"{star.star_id:03d}_{star.slug}.wav",
        raw_dir / f"{star.index:03d}.wav",
        raw_dir / f"{star.star_id:03d}.wav",
        raw_dir / f"star_{star.star_id:03d}.wav",
    ]

    if star.kind == "RANDOMIZER_STAR_100_COINS":
        candidates.extend([
            raw_dir / f"{star.course_slug}_100_coins.wav",
            raw_dir / f"{star.course_slug}_one_hundred_coins.wav",
        ])
    elif star.red_coin_objective:
        candidates.append(raw_dir / f"{star.course_slug}_red_coins.wav")

    if star.coin_objective:
        candidates.extend([
            raw_dir / f"{star.legacy_stem}.wav",
            raw_dir / f"{star.star_id:03d}_{star.legacy_slug}.wav",
        ])
    else:
        candidates.append(raw_dir / f"{star.slug}.wav")

    return candidates


def source_lookup(raw_dir: Path) -> dict[str, Path]:
    if not raw_dir.exists():
        return {}
    return {path.name.lower(): path for path in raw_dir.glob("*.wav")}


def find_source_wav(star: StarVoice, raw_dir: Path, lookup: dict[str, Path]) -> Path | None:
    for candidate in source_candidates(star, raw_dir):
        if candidate.exists():
            return candidate
        lowered = candidate.name.lower()
        if lowered in lookup:
            return lookup[lowered]
    return None


def voice_bank_count(stars: list[StarVoice]) -> int:
    return (len(stars) + VOICE_BANK_CHUNK_SIZE - 1) // VOICE_BANK_CHUNK_SIZE


def voice_bank_name(bank_index: int) -> str:
    return f"{VOICE_BANK_START + bank_index:02X}_rando_voice_{bank_index:02d}"


def voice_bank_path(bank_index: int, bank_dir: Path) -> Path:
    return bank_dir / f"{voice_bank_name(bank_index)}.json"


def voice_bank_names(stars: list[StarVoice]) -> list[str]:
    return [voice_bank_name(index) for index in range(voice_bank_count(stars))]


def remove_stale_generated_samples(stars: list[StarVoice], out_dir: Path) -> None:
    expected = {f"{star.stem}.aiff" for star in stars}
    if not out_dir.exists():
        return

    for path in out_dir.glob("*.aiff"):
        if path.name not in expected:
            path.unlink()


def convert_voice_samples(stars: list[StarVoice], raw_dir: Path, out_dir: Path,
                          placeholders: bool, normalize: bool, no_trim: bool) -> list[int]:
    lookup = source_lookup(raw_dir)
    durations: list[int] = []
    missing: list[str] = []
    out_dir.mkdir(parents=True, exist_ok=True)
    remove_stale_generated_samples(stars, out_dir)

    for star in stars:
        source = find_source_wav(star, raw_dir, lookup)
        out_path = out_dir / f"{star.stem}.aiff"

        if source is None:
            if not placeholders:
                missing.append(star.stem)
                durations.append(0)
                continue
            samples_s16 = placeholder_samples(tone_hz=740.0 + (star.index % 12) * 20.0)
        else:
            rate, samples = read_wav_mono(source)
            if not no_trim:
                samples = trim_silence(samples, rate)
            samples = resample_linear(samples, rate, TARGET_RATE)
            samples_s16 = float_to_s16(samples, normalize)

        write_aiff_pcm16(out_path, samples_s16, TARGET_RATE)
        frames = int(math.ceil(len(samples_s16) * 30.0 / TARGET_RATE)) + VOICE_END_PAD_FRAMES
        durations.append(max(DEFAULT_PLACEHOLDER_FRAMES, min(300, frames)))

    if missing:
        formatted = "\n".join(f"  - {stem}.wav" for stem in missing)
        raise SystemExit(f"missing {len(missing)} voice WAV files and placeholders are disabled:\n{formatted}")

    return durations


def cleanup_old_voice_metadata(clean_registered_samples: bool) -> None:
    if LEGACY_VOICE_SEQUENCE_INC.exists():
        LEGACY_VOICE_SEQUENCE_INC.unlink()

    for pattern in ("*rando_voice*.json", "*randomizer_voice*.json"):
        for path in (ROOT / "sound/sound_banks").glob(pattern):
            if path.exists():
                path.unlink()
    for pattern in ("*rando_voice*.s", "*randomizer_voice*.s"):
        for path in (ROOT / "sound/sequences").glob(pattern):
            if path.exists():
                path.unlink()
    for path in VOICE_BUILD_SAMPLE_DIR.glob("*.aifc"):
        if path.exists():
            path.unlink()
    if clean_registered_samples:
        for path in VOICE_BUILD_SAMPLE_DIR.glob("*.aiff"):
            if path.exists():
                path.unlink()


def write_sound_bank(bank_index: int, group_stars: list[StarVoice], bank_dir: Path) -> None:
    path = voice_bank_path(bank_index, bank_dir)
    path.parent.mkdir(parents=True, exist_ok=True)
    instruments: dict[str, dict[str, object]] = {}
    instrument_list: list[str] = []

    for local_index, star in enumerate(group_stars):
        inst_name = f"inst{local_index:02d}"
        instruments[inst_name] = {
            "release_rate": 208,
            "envelope": "voice_envelope",
            "sound": star.stem,
        }
        instrument_list.append(inst_name)

    data = {
        "date": "2026-05-29",
        "sample_bank": "rando_voice",
        "envelopes": {
            "voice_envelope": [
                [2, 32700],
                [1, 32700],
                [32700, 29430],
                "hang",
            ],
        },
        "instruments": instruments,
        "instrument_list": instrument_list,
    }

    write_text_lf(path, json.dumps(data, indent=4) + "\n")


def write_sequence_file(stars: list[StarVoice], durations: list[int], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    lines: list[str] = [
        "// Generated by tools/randomizer_voice_assets.py.",
        '#include "seq_macros.inc"',
        "",
        ".section .rodata",
        ".align 0",
        "",
        "sequence_start:",
        "seq_setmutebhv 0x60",
        "seq_setmutescale 0",
        "seq_setvol 127",
        "seq_settempo 120",
        "seq_initchannels 0x1",
        "seq_startchannel 0, .voice_channel",
        ".seq_loop:",
        "seq_delay 20000",
        "seq_jump .seq_loop",
        "",
        ".voice_channel:",
        "chan_largenoteson",
        "chan_setinstr 0",
        "chan_setnotepriority 14",
        "chan_setval 0",
        "chan_iowriteval 5",
        "chan_setmutebhv 0x0",
        "chan_stereoheadseteffects 1",
        "chan_setdyntable .voice_table",
        ".voice_wait:",
        "chan_delay1",
        "chan_ioreadval 0",
        "chan_bltz .voice_wait",
        ".voice_start:",
        "chan_freelayer 0",
        "chan_freelayer 1",
        "chan_freelayer 2",
        "chan_setval 0",
        "chan_iowriteval 5",
        "chan_setreverb 0",
        "chan_setpan 64",
        "chan_setpanmix 127",
        "chan_ioreadval 4",
        "chan_dyncall",
        ".voice_poll:",
        "chan_delay1",
        "chan_ioreadval 0",
        "chan_bltz .voice_skip",
        "  chan_beqz .voice_stop",
        "  chan_unreservenotes",
        "  chan_jump .voice_start",
        ".voice_skip:",
        "chan_testlayerfinished 0",
        "chan_beqz .voice_poll",
        "chan_unreservenotes",
        "chan_jump .voice_wait",
        ".voice_stop:",
        "chan_freelayer 0",
        "chan_freelayer 1",
        "chan_freelayer 2",
        "chan_unreservenotes",
        "chan_jump .voice_wait",
        "",
        ".voice_table:",
    ]

    for star in stars:
        lines.append(f"sound_ref .sound_rando_voice_{star.index:03d}")

    for star in stars:
        bank_index = star.index // VOICE_BANK_CHUNK_SIZE
        local_index = star.index % VOICE_BANK_CHUNK_SIZE
        frames = durations[star.index]
        ticks = max(48, min(600, frames * 2))
        lines.extend([
            "",
            f".sound_rando_voice_{star.index:03d}:",
            f"chan_setbank {bank_index}",
            f"chan_setinstr {local_index}",
            f"chan_setlayer 0, .layer_rando_voice_{star.index:03d}",
            "chan_end",
            "",
            f".layer_rando_voice_{star.index:03d}:",
            f"layer_note1_long 39, {ticks}, 127",
            "layer_end",
        ])

    write_text_lf(path, "\n".join(lines) + "\n")


def update_sequences_json(stars: list[StarVoice], register_assets: bool) -> None:
    data = json.loads(SEQUENCES_JSON.read_text(encoding="utf-8"))

    for key in list(data):
        if "_rando_voice_" in key:
            del data[key]

    data[VOICE_SEQUENCE_KEY] = voice_bank_names(stars) if register_assets else None
    write_text_lf(SEQUENCES_JSON, json.dumps(data, indent=4) + "\n")


def estimate_bank_set_size(sequences: dict[str, object]) -> int:
    max_index = -1
    for key in sequences:
        try:
            index = int(key.split("_", 1)[0], 16)
        except ValueError:
            continue
        max_index = max(max_index, index)

    size = (max_index + 1) * 2
    for index in range(max_index + 1):
        prefix = f"{index:02X}_"
        banks: list[str] = []
        for key, value in sequences.items():
            if key.upper().startswith(prefix):
                if isinstance(value, list):
                    banks = value
                elif isinstance(value, dict) and isinstance(value.get("banks"), list):
                    banks = value["banks"]
                break
        size += 1 + len(banks)

    return (size + 15) & ~15


def audit_voice_layout(stars: list[StarVoice]) -> None:
    bank_files = [path for path in (ROOT / "sound/sound_banks").glob("*.json")]
    if len(bank_files) > VOICE_MAX_SOUND_BANKS:
        raise SystemExit(
            f"sound bank count is {len(bank_files)}, but the US/JP runtime status table supports "
            f"{VOICE_MAX_SOUND_BANKS}"
        )

    sequences = json.loads(SEQUENCES_JSON.read_text(encoding="utf-8"))
    bank_set_size = estimate_bank_set_size(sequences)
    if bank_set_size > VOICE_MAX_BANK_SET_BYTES:
        raise SystemExit(
            f"bank set data is {bank_set_size} bytes, but src/audio/load.c currently copies "
            f"only {VOICE_MAX_BANK_SET_BYTES} bytes at boot"
        )

    if voice_bank_count(stars) > 32:
        raise SystemExit("too many randomizer voice banks; increase VOICE_BANK_CHUNK_SIZE")


def clean_staged_voice_metadata() -> None:
    for pattern in ("*rando_voice*.json", "*randomizer_voice*.json"):
        for path in VOICE_STAGED_BANK_DIR.glob(pattern):
            if path.exists():
                path.unlink()
    for pattern in ("*rando_voice*.s", "*randomizer_voice*.s"):
        for path in VOICE_STAGED_SEQUENCE_FILE.parent.glob(pattern):
            if path.exists():
                path.unlink()


def write_voice_metadata(stars: list[StarVoice], durations: list[int], register_assets: bool) -> None:
    cleanup_old_voice_metadata(clean_registered_samples=not register_assets)
    clean_staged_voice_metadata()

    bank_dir = ROOT / "sound/sound_banks" if register_assets else VOICE_STAGED_BANK_DIR
    sequence_path = VOICE_BUILD_SEQUENCE_FILE if register_assets else VOICE_STAGED_SEQUENCE_FILE

    for bank_index in range(voice_bank_count(stars)):
        start = bank_index * VOICE_BANK_CHUNK_SIZE
        end = min(len(stars), start + VOICE_BANK_CHUNK_SIZE)
        write_sound_bank(bank_index, stars[start:end], bank_dir)

    write_sequence_file(stars, durations, sequence_path)
    update_sequences_json(stars, register_assets)
    if register_assets:
        audit_voice_layout(stars)


def write_duration_table(stars: list[StarVoice], durations: list[int], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if LEGACY_VOICE_DURATION_INC.exists():
        LEGACY_VOICE_DURATION_INC.unlink()

    lines = ["    // Generated by tools/randomizer_voice_assets.py."]
    for star in stars:
        bank_index = star.index // VOICE_BANK_CHUNK_SIZE
        lines.append(f"    {{ {bank_index:2d}, {star.index:3d}, {durations[star.index]:3d} }},")
    write_text_lf(path, "\n".join(lines) + "\n")


def write_recording_manifest(stars: list[StarVoice], raw_dir: Path) -> None:
    raw_dir.mkdir(parents=True, exist_ok=True)
    lines = [
        "# Randomizer Voice Recordings",
        "",
        "Place recorded WAV files in this directory, then run:",
        "",
        "```",
        "python tools/randomizer_voice_assets.py --normalize",
        "```",
        "",
        "By default this stages generated assets under assets/randomizer_voice so",
        "the normal ROM build does not register the experimental voice banks.",
        "Use --register only when testing the native audio playback integration.",
        "",
        "Recommended source format: mono WAV, 16-bit or 24-bit PCM, 22050 Hz to 48000 Hz.",
        "The converter trims silence, mixes to mono, resamples to 16000 Hz, writes AIFF,",
        "and the normal SM64 build converts those samples to N64 VADPCM.",
        "",
        "100-coin and red-coin objectives include the stage abbreviation in the filename.",
        "This keeps every course-specific coin objective on its own recorded line.",
        "",
        "Canonical filenames:",
        "",
    ]

    for star in stars:
        lines.append(f"- `{star.stem}.wav` - {star.recording_label}")

    write_text_lf(VOICE_MANIFEST, "\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--raw-dir", type=Path, default=VOICE_RAW_DIR,
                        help="directory containing recorded WAV files")
    parser.add_argument("--sample-dir", type=Path, default=None,
                        help="AIFF output directory; defaults to staged assets, or sound/samples when --register is used")
    parser.add_argument("--register", action="store_true",
                        help="install generated sequence, banks, and samples into sound/ build inputs")
    parser.add_argument("--no-placeholders", action="store_true",
                        help="fail instead of creating short placeholder AIFFs for missing WAVs")
    parser.add_argument("--normalize", action="store_true",
                        help="peak-normalize converted WAVs to about -0.4 dB")
    parser.add_argument("--no-trim", action="store_true",
                        help="preserve leading/trailing silence in WAVs")
    args = parser.parse_args()
    sample_dir = args.sample_dir
    if sample_dir is None:
        sample_dir = VOICE_BUILD_SAMPLE_DIR if args.register else VOICE_STAGED_SAMPLE_DIR

    stars = parse_stars()
    write_recording_manifest(stars, args.raw_dir)
    durations = convert_voice_samples(
        stars,
        args.raw_dir,
        sample_dir,
        placeholders=not args.no_placeholders,
        normalize=args.normalize,
        no_trim=args.no_trim,
    )
    write_voice_metadata(stars, durations, args.register)
    write_duration_table(stars, durations, VOICE_DURATION_INC)

    print(f"Wrote {len(stars)} randomizer voice slots.")
    print(f"Wrote {voice_bank_count(stars)} randomizer voice banks for one sequence.")
    print("Registered voice assets into sound/." if args.register else "Staged voice assets outside the ROM build.")
    print(f"Put recordings in: {args.raw_dir}")
    print(f"AIFF output: {sample_dir}")


if __name__ == "__main__":
    main()
