#!/bin/bash
# Convert PNG textures to KTX2 (UASTC supercompression) for GPU-native loading.
# Output goes to textures_ktx/ alongside the original textures/ folder.
#
# Usage: ./scripts/convert_to_ktx2.sh [texture_dir]
#   texture_dir defaults to res/model/halo/textures

set -euo pipefail

SRC_DIR="${1:-res/model/halo/textures}"
DST_DIR="$(dirname "$SRC_DIR")/textures_ktx"

if ! command -v ktx &>/dev/null; then
    echo "ERROR: 'ktx' tool not found. Install KTX-Software." >&2
    exit 1
fi

if ! command -v identify &>/dev/null; then
    echo "ERROR: 'identify' (ImageMagick) not found." >&2
    exit 1
fi

mkdir -p "$DST_DIR"

# Textures that represent color data and must be tagged sRGB:
is_srgb() {
    local name="$1"
    [[ "$name" == *BaseColor* ]] || [[ "$name" == *Colour* ]] || \
    [[ "$name" == *Specular* ]] || [[ "$name" == *Emissive* ]]
}

shopt -s nullglob
ok=0; fail=0

for src in "$SRC_DIR"/*.png "$SRC_DIR"/*.PNG; do
    name=$(basename "$src" .png)
    name=$(basename "$name" .PNG)
    out="$DST_DIR/${name}.ktx2"

    # Detect alpha channel via ImageMagick identify
    channels=$(identify -format "%[channels]" "$src" 2>/dev/null | head -1)
    has_alpha=false
    [[ "$channels" == *"a"* ]] && has_alpha=true

    # Select VkFormat based on color space and alpha
    if is_srgb "$name"; then
        $has_alpha && fmt="R8G8B8A8_SRGB" || fmt="R8G8B8_SRGB"
        tf_flag=""
    else
        $has_alpha && fmt="R8G8B8A8_UNORM" || fmt="R8G8B8_UNORM"
        tf_flag="--assign-tf linear"
    fi

    echo "[ktx2] $name  fmt=$fmt"
    if ktx create --encode uastc --format "$fmt" $tf_flag \
            --generate-mipmap "$src" "$out" 2>/dev/null; then
        (( ok++ )) || true
    else
        echo "  FAILED: $src" >&2
        (( fail++ )) || true
    fi
done

echo ""
echo "Done. Converted: $ok  Failed: $fail"
echo "Output: $DST_DIR/"
