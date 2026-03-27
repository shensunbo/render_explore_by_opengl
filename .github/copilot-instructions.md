# Copilot Instructions

## Build System

This project supports two build systems: **CMake** (primary) and **Bazel**.

### CMake
```bash
mkdir build && cd build
cmake ..
make -j6

# Run desktop test
./src/android_test

# Android NDK cross-compile
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-29 ..
make -j$(nproc)
```

### Bazel
```bash
# Build and run main renderer test
bazel run //src:refactor_test

# Debug build (preserves symbols for perf/gdb)
bazel build --cxxopt="-g" --strip=never //src:refactor_test

# Individual utility tests
bazel run //test:eglTest
bazel run //test:mesaTest
bazel run //test:ktxTest
bazel run //test:multithreadLoadTest
```

Resources must be available in the working directory. CMake handles this via the `copy_resources` custom target; with Bazel, resources are referenced from the repo root.

## Architecture

### High-Level Pipeline

```
RendererAPI (public C interface)
  └─ Renderer::create()
       ├─ Load shaders (Shader: .vs/.fs pairs)
       ├─ Load model (ModelLoader via Assimp → BufferObjectData per mesh)
       ├─ Pre-load all textures → TextureCache
       └─ Build RenderGraph
            ├─ ScenePass   – main mesh/material rendering
            ├─ SkyboxPass  – cubemap background
            └─ PostPass    – optional FBO → fullscreen quad
```

Each frame, the caller fills a `FrameParams` struct (MVP matrices, eye position, flags) and calls `RendererAPI::drawFrame()`, which drives `RenderGraph::execute()`.

### Key Classes

| Class | Role |
|---|---|
| `RendererAPI` | Public C-style API; lifecycle + per-frame entry point |
| `Renderer` | Core orchestrator; creates graph, manages shader selection |
| `RenderGraph` / `IRenderPass` | Composable render-pass sequencer |
| `Shader` | GLSL program wrapper with typed uniform setters |
| `ModelLoader` | Assimp-based importer; produces `BufferObjectData` per mesh |
| `TextureCache` | Path-keyed GLuint cache; prevents duplicate GPU uploads |
| `BufferObjectData` | RAII owner of VAO/VBO/EBO/UBO for one mesh |
| `Skybox` | Cubemap texture + geometry |
| `FboHandler` | Offscreen framebuffer for post-processing |
| `ConfigParser` | JSON-driven mesh/material classification and overrides |
| `VirCamera` | View matrix management |

### Platform Abstraction

- Desktop: OpenGL + GLAD loader + GLFW window
- Android: OpenGL ES 3 (GLESv3) + EGL context; no GLFW
- `include/gl/gl_headers.h` centralizes the platform-conditional GL includes

## Conventions

### Naming
- **Classes/structs**: `PascalCase` — `Renderer`, `TextureCache`
- **Methods**: `camelCase` — `execute()`, `loadModel()`, `bindVao()`
- **Private members**: `snake_case_` (trailing underscore) — `width_`, `shader_`
- **Enums / config structs**: `PascalCase` — `RendererConfig`, `FrameParams`, `UboMat`
- **All identifiers and comments must be in English** (enforced by `.agents/skills/`)

### Memory Management
- Use `std::make_unique<>` / `std::make_shared<>`; no raw `new`/`delete`
- OpenGL resources (textures, buffers, programs) are owned by RAII classes whose destructors call the matching `glDelete*()` function
- `BufferObjectData` implements move constructor/assignment for efficient transfers; copy is deleted

### OpenGL Resource Pattern
Every GL object gets a wrapper class:
1. Constructor allocates (`glGen*` / `glCreate*`)
2. Destructor releases (`glDelete*`)
3. No copy; move-only where transfer is needed
4. Error checking via the `CHECK_GLES_STATUS` macro after compile/link steps

### Shaders
- Paired files in `res/shader/`: `<name>.vs` (vertex) + `<name>.fs` (fragment)
- Current shaders: `with_texture` (Phong/legacy), `pbr` (PBR), `skybox`, `fbo_rect` (post-process quad)
- Loaded by `Shader` at startup; runtime toggling between legacy and PBR is supported via `FrameParams` flags

### Texture Types
Seven slots are defined: **diffuse, specular, normal, AO, roughness, metallic, emissive**. Binding slots 0–9 are reserved for scene textures; 10+ for skybox. Color textures (diffuse, emissive) are uploaded as sRGB.

### Configuration
- `res/config/render_config.json` — resolution, shader paths, texture-enable flags, exposure, FPS cap
- Per-model JSON configs (alongside model files) — mesh classification (doors, wheels, paint, lights), per-mesh texture overrides, rotation transforms
- `ConfigParser` reads both; do not hard-code resource paths or material parameters

### Comments
Use Doxygen style for all non-trivial declarations:
```cpp
/**
 * @brief One-line description.
 * @param path File path of the texture.
 * @param srgb Whether to treat as sRGB color space.
 * @return OpenGL texture ID.
 */
GLuint getOrCreate(const std::string& path, const imageParam& imgData, bool srgb = false);
```

## Dependencies (in `dependency/`)

| Library | Purpose |
|---|---|
| Assimp | 3D model import (FBX, glTF, OBJ, …) |
| GLM | Math (vec3, mat4, transforms) |
| GLAD | OpenGL function loader (desktop) |
| ImGui | Debug/control overlay in test apps |
| stb_image | PNG/JPG/HDR image decoding |
| nlohmann/json | JSON config parsing |
| KTX | Khronos texture format |
| EGL | Off-screen context (Mesa/Android) |

GLFW is a system library (linked as `-lglfw`); all other dependencies are vendored under `dependency/`.

## Build verification

Always run: bazel build //src:refactor_test after making any code changes to verify the repository builds.
