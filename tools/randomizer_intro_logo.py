#!/usr/bin/env python3
"""Prepare the randomizer intro logo texture for the SM64 build.

The game source includes levels/intro/rando_logo.rgba16.png, and the normal
decomp Makefile converts that PNG into rando_logo.rgba16.inc.c during the ROM
build. This helper only normalizes an input PNG into the expected 32x32 source
asset.
"""

from __future__ import annotations

import argparse
from pathlib import Path

try:
    from PIL import Image
except ImportError as exc:
    raise SystemExit("Pillow is required: python -m pip install Pillow") from exc


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "levels" / "intro" / "rando_logo.rgba16.png"
TARGET_SIZE = 32


def resampling_filter(use_nearest: bool) -> int:
    filters = getattr(Image, "Resampling", Image)
    if use_nearest:
        return filters.NEAREST
    return filters.LANCZOS


def center_crop_square(image: Image.Image) -> Image.Image:
    width, height = image.size
    if width == height:
        return image

    side = min(width, height)
    left = (width - side) // 2
    top = (height - side) // 2
    return image.crop((left, top, left + side, top + side))


def quantize_rgba16_alpha(image: Image.Image) -> Image.Image:
    pixels = bytearray(image.tobytes())
    for alpha_index in range(3, len(pixels), 4):
        pixels[alpha_index] = 255 if pixels[alpha_index] >= 128 else 0
    return Image.frombytes("RGBA", image.size, bytes(pixels))


def convert_logo(input_path: Path, output_path: Path, strict_square: bool, use_nearest: bool) -> None:
    with Image.open(input_path) as source:
        image = source.convert("RGBA")

    if strict_square and image.width != image.height:
        raise SystemExit(f"{input_path} must be square, got {image.width}x{image.height}")

    image = center_crop_square(image)
    if image.size != (TARGET_SIZE, TARGET_SIZE):
        image = image.resize((TARGET_SIZE, TARGET_SIZE), resampling_filter(use_nearest))
    image = quantize_rgba16_alpha(image)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    image.save(output_path)


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert a square PNG into levels/intro/rando_logo.rgba16.png."
    )
    parser.add_argument("input", type=Path, help="Source PNG. Larger images are downscaled to 32x32.")
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT,
        help=f"Output asset path. Defaults to {DEFAULT_OUTPUT.relative_to(ROOT)}.",
    )
    parser.add_argument(
        "--strict-square",
        action="store_true",
        help="Reject non-square input instead of center-cropping it.",
    )
    parser.add_argument(
        "--nearest",
        action="store_true",
        help="Use nearest-neighbor resizing for pixel-art logos.",
    )
    args = parser.parse_args()

    output = args.output
    if not output.is_absolute():
        output = ROOT / output

    convert_logo(args.input, output, args.strict_square, args.nearest)
    print(f"Wrote {output.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
