# KTX Texture Support Plan

## 1. Current State

| Item | Detail |
|---|---|
| Texture format | PNG / JPG (via `stb_image`) |
| Total textures | 51 in `res/model/halo/textures/` + 6 skybox PNGs |
| Loading pipeline | Multi-threaded CPU decode → main-thread GPU upload via `TextureCache::getOrCreate` |
| KTX library | Already vendored: `dependency/ktx/lib/libktx.so.4.4.2`, header `dependency/ktx/include/ktx.h` |
| Existing KTX code | `ModelLoader::Impl::TextureFromKTXFile` is **fully implemented** but **commented out** in `LoadTextures` dispatch |
| Skybox | Loaded separately in `Skybox::Init` via stb — needs independent update |

## 2. Why KTX2

- **GPU-native compressed formats** (ETC2, ASTC, BC7) stored in the file → no CPU decompression, reduced VRAM footprint
- **Mipmaps pre-baked** into the file → `glGenerateMipmap` calls eliminated
- **Supercompression** (Basis LZ / UASTC / Zstd) reduces file size further; transcoder runs once at load time
- **`ktxTexture_GLUpload`** does everything in one call: creates the GL texture, selects the right format for the platform, uploads all mip levels

## 3. Format Conversion

### 3.1 Tool: `toktx` from KTX-Software

`toktx` is the reference CLI tool for creating KTX2 files.

**Install (Ubuntu/Debian):**
```bash
# Download pre-built release from GitHub
KTX_VER=v4.4.0
wget https://github.com/KhronosGroup/KTX-Software/releases/download/${KTX_VER}/KTX-Software-${KTX_VER}-Linux-x86_64.deb
sudo dpkg -i KTX-Software-${KTX_VER}-Linux-x86_64.deb
# Verify
toktx --version
```

Alternatively build from source:
```bash
git clone --depth 1 --branch v4.4.0 https://github.com/KhronosGroup/KTX-Software.git
cd KTX-Software && cmake -B build -DKTX_FEATURE_TOOLS=ON && cmake --build build -j$(nproc)
export PATH="$PWD/build/tools:$PATH"
```

### 3.2 Encoding choice

| Encoding | Flag | Quality | Use case |
|---|---|---|---|
| ETC1S (Basis LZ) | `--encode etc1s` | Lower | Mobile / broad compatibility |
| UASTC | `--encode uastc` | Higher | Desktop, can transcode to BC7 |

**Recommended**: UASTC for desktop (this project targets desktop OpenGL). The vendored `libktx.so.4.4.2` supports both.

### 3.3 sRGB vs Linear

Textures must declare the correct colour space at conversion time:

| Texture type (`typeName`) | Slot | sRGB? | Assimp type |
|---|---|---|---|
| `texture_diffuse` (BaseColor) | 0 | **Yes** | `aiTextureType_DIFFUSE` |
| `texture_specular` | 1 | **Yes** | `aiTextureType_SPECULAR` |
| `texture_normal` | 2 | No | `aiTextureType_NORMALS` |
| `texture_ao` | 3 | No | `aiTextureType_AMBIENT_OCCLUSION` |
| `texture_alpha` | 4 | No | `aiTextureType_OPACITY` |
| `texture_roughness` | 5 | No | `aiTextureType_DIFFUSE_ROUGHNESS` |
| `texture_metallic` | 6 | No | `aiTextureType_METALNESS` |

The current code already determines sRGB correctly:
```cpp
const bool isSrgb = (type == aiTextureType_DIFFUSE || type == aiTextureType_SPECULAR);
```
The conversion script must replicate this logic.

### 3.4 Batch conversion script

Save as `scripts/convert_to_ktx2.sh`:

```bash
#!/bin/bash
# Converts all PNG/JPG textures in a directory to KTX2 (UASTC + Zstd).
# Usage: ./scripts/convert_to_ktx2.sh [texture_dir]
#   texture_dir defaults to res/model/halo/textures

set -euo pipefail
TEXDIR="${1:-res/model/halo/textures}"

if ! command -v toktx &>/dev/null; then
    echo "ERROR: toktx not found. Install KTX-Software first." >&2
    exit 1
fi

shopt -s nullglob
for src in "$TEXDIR"/*.png "$TEXDIR"/*.jpg "$TEXDIR"/*.PNG "$TEXDIR"/*.JPG; do
    base="${src%.*}"
    out="${base}.ktx2"

    # Classify by name fragment — must match vehicle_info.json assignments
    if [[ "$src" == *BaseColor* || "$src" == *Diffuse* || "$src" == *Emissive* || "$src" == *Colour* ]]; then
        CS="--srgb"
    else
        CS=""   # linear (normal, roughness, AO, metallic, specular data textures)
    fi

    echo "Converting: $src → $out  ($CS)"
    toktx --encode uastc --uastc_quality 3 --zcmp 20 --genmipmap $CS --t2 "$out" "$src"
done

echo "Done."
```

### 3.5 Where to put converted files

Place `.ktx2` files **alongside the originals** in the same directory:
```
res/model/halo/textures/
  Spartan_Arms_Mat_AO.png        ← keep (fallback, reference)
  Spartan_Arms_Mat_AO.ktx2       ← new
  Spartan_Arms_Mat_BaseColor.png
  Spartan_Arms_Mat_BaseColor.ktx2
  ...
```

Keeping originals avoids breaking existing non-KTX path and allows easy A/B comparison.

For skybox:
```
res/model/skybox/
  px.png, nx.png, ...   ← keep
  px.ktx2, nx.ktx2, ... ← new
```

## 4. Code Changes

### 4.1 `vehicle_info.json` texture path entries

The JSON config maps mesh names → texture paths. Change `.png` extensions to `.ktx2`:
```json
"diffuse": "res/model/halo/textures/Spartan_Arms_Mat_BaseColor.ktx2"
```
This is **the single toggle** that switches the whole model to KTX2 — no code change needed for the dispatch once the code is wired up.

A helper script can automate the rename:
```bash
sed -i 's/\.png"/.ktx2"/g; s/\.jpg"/.ktx2"/g' res/model/halo/vehicle_info.json
```

### 4.2 `ModelLoader.cpp` — enable KTX dispatch in `LoadTextures`

Replace the current stb-only block (lines ~620–642) with extension-aware dispatch:

```cpp
// Determine if this is a KTX/KTX2 file
auto isKtxPath = [](const std::string& p) {
    const auto& e = p;
    return e.size() >= 4 &&
           (e.compare(e.size()-4, 4, ".ktx") == 0 ||
            e.compare(e.size()-4, 4, ".KTX") == 0 ||
            (e.size() >= 5 &&
             (e.compare(e.size()-5, 5, ".ktx2") == 0 ||
              e.compare(e.size()-5, 5, ".KTX2") == 0)));
};

if (isKtxPath(textureName)) {
    // KTX path: load and upload directly (must be on main thread — already satisfied here)
    texture.id = TextureFromKTXFile(textureName.c_str(), "");  // path is absolute from config
} else {
    // Legacy stb path
    const bool isSrgb = (type == aiTextureType_DIFFUSE || type == aiTextureType_SPECULAR);
    auto texIt = textureData.find(textureName);
    if (texIt == textureData.end()) {
        LOG_W("Texture data missing for {}", textureName);
        texture.id = 0;
    } else {
        texture.id = textureCache.getOrCreate(textureName, texIt->second, isSrgb);
    }
}
```

### 4.3 `TextureCache` — add KTX ID cache

`TextureFromKTXFile` already uses `glGenTextures` internally (via `ktxTexture_GLUpload`), but there is no deduplication. Add a second map to `TextureCache`:

**`TextureCache.h`** — add:
```cpp
GLuint getOrCreateKtx(const std::string& path);   // new
```

**`TextureCache.cpp`** — implement:
```cpp
GLuint TextureCache::getOrCreateKtx(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) return it->second;

    ktxTexture* ktxTex = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(
        path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);
    if (result != KTX_SUCCESS) {
        LOG_E("KTX load failed: {} — {}", path, ktxErrorString(result));
        return 0;
    }

    GLuint texID = 0; GLenum target = 0, glerr = GL_NO_ERROR;
    result = ktxTexture_GLUpload(ktxTex, &texID, &target, &glerr);
    ktxTexture_Destroy(ktxTex);

    if (result != KTX_SUCCESS || glerr != GL_NO_ERROR) {
        LOG_E("KTX upload failed: {}", path);
        return 0;
    }

    // Mip-filter (ktxTexture_GLUpload may not set these)
    glBindTexture(target, texID);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textures_[path] = texID;
    return texID;
}
```

Then the `ModelLoader` dispatch calls `textureCache.getOrCreateKtx(textureName)` for KTX paths.

### 4.4 `Renderer::Impl::create()` — skip KTX paths from stb pre-load

The existing multi-threaded pre-loader calls `Tool::ImageFromFile` for every path in `m_texture_paths`. KTX files must be excluded (stb cannot read them):

```cpp
for (auto const& texture_path : m_texture_paths) {
    // Skip KTX files — loaded directly on the GL thread via ktxTexture_GLUpload
    if (texture_path.ends_with(".ktx") || texture_path.ends_with(".ktx2")) continue;

    threads.emplace_back([this, &textureMutex, texture_path]() {
        // ... existing stb load ...
    });
}
```

### 4.5 `Skybox.cpp` — KTX2 cubemap support (optional, phase 2)

The skybox currently uses `stbi_load` per-face. To support KTX2 cubemaps:
- Use a single KTX2 file with 6 faces (`--cubemap` flag in `toktx`)
- `ktxTexture_GLUpload` uploads all 6 faces as `GL_TEXTURE_CUBE_MAP` automatically
- Change `Skybox::Init` signature to accept either 6 PNG paths or 1 KTX2 path

This can be done in a follow-up.

### 4.6 `TextureCache.h` — add `#include <ktx.h>`

```cpp
#include <ktx.h>   // for getOrCreateKtx
```

## 5. Summary of File Changes

| File | Change |
|---|---|
| `scripts/convert_to_ktx2.sh` | **New** — batch PNG→KTX2 conversion script |
| `res/model/halo/vehicle_info.json` | Change `.png` → `.ktx2` in texture paths |
| `src/core/texture/TextureCache.h` | Add `getOrCreateKtx` declaration + `#include <ktx.h>` |
| `src/core/texture/TextureCache.cpp` | Implement `getOrCreateKtx` |
| `src/core/mesh/ModelLoader.cpp` | Uncomment + modernise KTX dispatch in `LoadTextures`; call `textureCache.getOrCreateKtx` for KTX paths |
| `src/core/renderer/Renderer.cpp` | Skip KTX paths from stb multi-thread pre-loader |
| `src/core/scene/Skybox.cpp` | *(Phase 2)* KTX2 cubemap face loading |

## 6. Recommended Implementation Order

1. Install `toktx` → run conversion script → verify `.ktx2` files are created
2. Update `vehicle_info.json` paths to `.ktx2`
3. Add `getOrCreateKtx` to `TextureCache`
4. Update `LoadTextures` KTX dispatch in `ModelLoader.cpp`
5. Update `Renderer.cpp` stb pre-load to skip KTX paths
6. Build and test: `bazel build //src:refactor_test`
7. *(Optional)* Skybox KTX2 cubemap

## from user
1. ktx installed
```shensunbo@DLFGVL3T3:~/tools/KTX-Software/build$ ktx
ktx: Missing command.

Usage:
  ktx [<command>] [OPTION...]

  -h, --help     Print this usage message and exit
  -v, --version  Print the version number of this program and exit
      --testrun  Indicates test run. If enabled the tool will produce deterministic output whenever
                 possible

Available commands:
  convert    Convert another texture file type to a KTX2 file
  create     Create a KTX2 file from various input files
  deflate    Deflate (supercompress) a KTX2 file
  encode     Encode a KTX2 file
  extract    Extract selected images from a KTX2 file
  transcode  Transcode a KTX2 file
  info       Print information about a KTX2 file
  validate   Validate a KTX2 file
  compare    Compare two KTX2 files
  help       Display help information about the ktx tool

For detailed usage and description of each subcommand use 'ktx help <command>'
or 'ktx <command> --help'
```
2. I want to file reading using multiple thread, but the GPU uploading you can use single thread if the ktx library does not support multi-threaded uploading. So I want to read the file using multiple thread, and then upload the texture to GPU in single thread. Is it possible? If so, how to do it? If not, why?
3. skybox also use ktx format