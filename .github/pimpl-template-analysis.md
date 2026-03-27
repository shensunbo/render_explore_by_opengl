# PIMPL and Template Refactoring Opportunities

Analyzed classes in `src/core/` and `src/configParser/`.

---

## Quick Reference

| Class | PIMPL | Template | Priority |
|---|---|---|---|
| `ModelLoader` | ✅ YES | — | 🔴 Critical |
| `Renderer` | ✅ YES | — | 🟠 High |
| `Shader` | — | ✅ YES | 🟠 High |
| `TextureCache` | — | ✅ YES | 🟠 High |
| `BufferObjectData` | — | ✅ YES | 🟡 Medium |
| `MeshInfo` | ✅ YES | — | 🟡 Medium |
| `ConfigParser` | ✅ YES | — | 🟡 Medium |
| `ScenePass/SkyboxPass/PostPass` | — | ✅ CRTP | 🟢 Low |
| `RenderGraph` | — | ✅ YES | 🟢 Low |
| `FboHandler` | — | — | — |
| `Skybox` | — | — | — |
| `VirCamera` | — | — | — |

---

## PIMPL Candidates

### 1. `ModelLoader` — 🔴 Critical

**File:** `src/core/mesh/ModelLoader.h`

**Problem:** The header includes `<assimp/Importer.hpp>`, `<assimp/scene.h>`, and `<assimp/postprocess.h>` directly. Every file that transitively includes `ModelLoader.h` (including `MeshInfo.h` → `Renderer.h` → the entire codebase) pays the full Assimp parse cost on every incremental rebuild.

**PIMPL design:**
```cpp
// ModelLoader.h — only std includes, no assimp
class ModelLoader {
public:
    bool LoadModel(const std::string& resPath,
                   std::vector<BufferObjectData>& out,
                   ConfigParser& cfg,
                   const std::unordered_map<std::string, imageParam>& texData,
                   TextureCache& cache);
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// ModelLoader.cpp — assimp lives only here
struct ModelLoader::Impl {
    Assimp::Importer importer;
    void processNode(aiNode*, const aiScene*, ...);
    BufferObjectData processMesh(aiMesh*, const aiScene*, ...);
    // ...
};
```

**Pros:**
- Assimp headers no longer leak to any downstream consumer
- Estimated 30–50% reduction in incremental rebuild time for the entire renderer
- Free to change the loading strategy (switch Assimp version, swap for tinygltf) without recompiling clients

**Cons:**
- One extra heap allocation per `ModelLoader` instance
- Every public method needs a forwarding call to `pImpl_`
- Slightly harder to debug (extra indirection in call stack)

---

### 2. `Renderer` — 🟠 High

**File:** `src/core/renderer/Renderer.h`

**Problem:** The header pulls in `Shader.h`, `Skybox.h`, `MeshInfo.h` (→ Assimp), `FboHandler.h`, `TextureCache.h`, and `RenderGraph.h`. It also exposes implementation details as `public` members (`legacyShader_`, `pbrShader_`, `m_loaded_texture_data`, etc.) that no external consumer should touch.

**PIMPL design:**
```cpp
// Renderer.h — clean public API
class Renderer {
public:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void create(const RendererConfig& cfg);
    void destroy();
    void renderFrame(const FrameParams& params);
    void resize(unsigned int w, unsigned int h);
    void setPbrEnabled(bool enabled);
    bool isPbrEnabled() const;
    void setCubemapRotation(const glm::mat4& rotation);
    void setTimingEnabled(bool enabled);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// Renderer.cpp — all heavy members live here
struct Renderer::Impl {
    std::unique_ptr<Shader>       legacyShader_;
    std::unique_ptr<Shader>       pbrShader_;
    Shader*                       activeShader_{nullptr};
    std::unique_ptr<MeshInfo>     ourModel;
    std::shared_ptr<Skybox>       cubemap;
    std::shared_ptr<FboHandler>   fbo_;
    // ... rest of current private members
};
```

**Pros:**
- Header becomes 20 lines instead of 100
- Cuts transitively included headers (`Skybox.h`, `MeshInfo.h`, `FboHandler.h`) from all clients
- Enables binary-stable ABI — can add/remove members without recompiling `renderer_api.cpp`
- Forces removal of incorrectly-public members (`legacyShader_`, etc.)

**Cons:**
- Destructor must be defined in `.cpp` (because `Impl` is incomplete in the header)
- Slightly more typing for method forwarding
- Move semantics become non-trivial (must implement or explicitly delete)

---

### 3. `MeshInfo` — 🟡 Medium

**File:** `src/core/mesh/MeshInfo.h`

**Problem:** Includes `ModelLoader.h`, which pulls in Assimp transitively. Public members `meshes`, `textures_loaded`, `loader`, and `gammaCorrection` expose implementation details.

**PIMPL design:** Hide `loader` (the `ModelLoader` member) and internal texture lists behind an `Impl`. Expose only `const std::vector<BufferObjectData>& getMeshes()`.

**Pros:** Cuts Assimp from `MeshInfo.h` consumers; cleaner interface.

**Cons:** Extra indirection; low-value if `ModelLoader` itself gets PIMPL'd first (solves the header problem automatically).

> **Recommendation:** Do `ModelLoader` PIMPL first — it fixes `MeshInfo`'s header problem for free.

---

### 4. `ConfigParser` — 🟡 Medium

**File:** `src/configParser/ConfigParser.h`

**Problem:** Includes `json.hpp` (nlohmann JSON, a large header-only library) and exposes numerous internal map/set members (`m_fl_door_meshes`, `m_rotatable_meshes_transforms`, etc.) in the header.

**PIMPL design:**
```cpp
// ConfigParser.h
class ConfigParser {
public:
    bool parse(const std::string& path);
    std::string getName() const;
    ModelParam getParam() const;
    // ... only high-level query methods
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

// ConfigParser.cpp — json.hpp and all internal maps live here
```

**Pros:**
- `json.hpp` no longer leaks to `ModelLoader`, `Renderer`, and every downstream file
- Config data structures can change freely without recompiling consumers
- Simpler public interface

**Cons:**
- Requires designing a clean query API for the 12+ internal data structures currently accessed directly
- Significant refactor effort

---

## Template Candidates

### 1. `Shader` uniform setters — 🟠 High

**File:** `src/core/shader/Shader.h`

**Problem:** Nine near-identical methods that differ only in the GL call:
```cpp
void setBool (const std::string& name, bool value);
void setInt  (const std::string& name, int value);
void setFloat(const std::string& name, float value);
void setVec2 (const std::string& name, const glm::vec2&);
void setVec3 (const std::string& name, const glm::vec3&);
void setVec4 (const std::string& name, const glm::vec4&);
void setMat2 (const std::string& name, const glm::mat2&);
void setMat3 (const std::string& name, const glm::mat3&);
void setMat4 (const std::string& name, const glm::mat4&);
```

**Template design:**
```cpp
// Primary template — declare but don't define (causes linker error for unsupported types)
template <typename T>
void setUniform(const std::string& name, const T& value);

// Explicit specialisations in Shader.cpp
template <> void Shader::setUniform(const std::string& name, const bool&   v);
template <> void Shader::setUniform(const std::string& name, const int&    v);
template <> void Shader::setUniform(const std::string& name, const float&  v);
template <> void Shader::setUniform(const std::string& name, const glm::vec3& v);
template <> void Shader::setUniform(const std::string& name, const glm::mat4& v);
// ... etc.
```

Callers change from `shader->setMat4("uMVP", mvp)` → `shader->setUniform("uMVP", mvp)` (type deduced).

**Pros:**
- 9 methods → 1 interface; adding a new type (e.g., `uvec2`) is one specialisation
- Call sites read more uniformly; type is deduced from the argument
- Eliminates risk of calling the wrong setter by mistake

**Cons:**
- Specialisations must be in `.cpp` to avoid including GL headers everywhere, which requires explicit instantiation declarations in the header — slightly more boilerplate up front
- Slightly harder to grep for `setMat4` specifically when debugging uniforms
- Error messages for unsupported types come from the linker, not the compiler (worse DX)

---

### 2. `TextureCache` → `ResourceCache<T>` — 🟠 High

**File:** `src/core/texture/TextureCache.h`

**Problem:** The cache is hard-coded to `string → GLuint`. The same pattern will be needed for mesh caching, shader program caching, and material caching as the project grows.

**Template design:**
```cpp
template <typename Resource>
class ResourceCache {
public:
    bool has(const std::string& key) const;
    Resource get(const std::string& key) const;
    void put(const std::string& key, Resource res);
    void destroyAll(std::function<void(Resource)> deleter);
private:
    std::unordered_map<std::string, Resource> cache_;
};

// TextureCache remains as a concrete type with its GL-specific creation logic
class TextureCache : public ResourceCache<GLuint> {
public:
    GLuint getOrCreate(const std::string& path, const imageParam& img, bool srgb = false);
};
```

**Pros:**
- Generic container reused for shaders, materials, meshes with zero extra code
- Cache lookup/eviction logic written once
- `TextureCache` still has its GL-specific `getOrCreate`

**Cons:**
- Template in header means `<unordered_map>` is included by all consumers (already the case now)
- Over-engineering if only texture caching is ever needed
- Inheritance from a template base may cause surprises with object slicing

---

### 3. `BufferObjectData` — template on vertex type — 🟡 Medium

**File:** `src/core/mesh/BufferObjectData.h`

**Problem:** Buffer management logic (VAO/VBO/EBO/UBO setup, bind, draw) is valuable and correct, but hard-coded to `std::vector<Vertex>`. Supporting skinned meshes or instanced geometry requires a second near-identical class.

**Template design:**
```cpp
template <typename VertexType = Vertex>
class BufferObject {
public:
    explicit BufferObject(std::vector<VertexType> verts,
                          std::vector<unsigned int> idx,
                          std::vector<Texture> tex);
    // move-only (GL resources)
    BufferObject(BufferObject&&) noexcept;
    BufferObject& operator=(BufferObject&&) noexcept;
    BufferObject(const BufferObject&)            = delete;
    BufferObject& operator=(const BufferObject&) = delete;

    void bindVao() const;
    void bindUbo() const;
    unsigned int getIndicesSize() const;
    // ...
private:
    void setupMesh();
    GLuint VAO{0}, VBO{0}, EBO{0}, UBO{0};
    std::vector<VertexType>      vertices_;
    std::vector<unsigned int>    indices_;
    std::vector<Texture>         textures_;
};

using BufferObjectData = BufferObject<Vertex>; // backward-compatible alias
```

**Pros:**
- Enables `BufferObject<SkinnedVertex>` for animated meshes without code duplication
- GL buffer management written and debugged once
- Alias preserves all existing call sites

**Cons:**
- Template body must move to the header (or use explicit instantiation) — exposes `setupMesh` internals
- `VertexType` must satisfy a concept (has `position`, compatible `glVertexAttribPointer` layout) — worth adding a C++20 concept or static_assert
- Debugger display of templated types is noisier

---

### 4. `ScenePass` / `SkyboxPass` / `PostPass` — CRTP base — 🟢 Low

**Files:** `src/core/renderer/ScenePass.h`, `SkyboxPass.h`, `PostPass.h`

**Problem:** All three classes repeat identical boilerplate:
```cpp
~XxxPass() override = default;
XxxPass(const XxxPass&)            = delete;
XxxPass& operator=(const XxxPass&) = delete;
XxxPass(XxxPass&&)                 = default;
XxxPass& operator=(XxxPass&&)      = default;
```

**CRTP design:**
```cpp
template <typename Derived>
class RenderPassBase : public IRenderPass {
public:
    RenderPassBase()                               = default;
    ~RenderPassBase() override                     = default;
    RenderPassBase(const RenderPassBase&)          = delete;
    RenderPassBase& operator=(const RenderPassBase&) = delete;
    RenderPassBase(RenderPassBase&&)               = default;
    RenderPassBase& operator=(RenderPassBase&&)    = default;
    // No static dispatch needed here — execute() is still virtual
};

class ScenePass : public RenderPassBase<ScenePass> { ... };
```

**Pros:**
- Boilerplate lives in one place; adding a new pass type is 5 lines shorter
- Consistent copy/move policy enforced by inheritance

**Cons:**
- CRTP adds conceptual complexity for a small gain (5 lines per class)
- No actual static dispatch benefit here since `execute()` is virtual anyway
- Reviewers unfamiliar with CRTP will be confused

> **Verdict:** Only worth doing if many more pass types are expected. For 3 classes, the gain is marginal.

---

### 5. `RenderGraph` — template on pass base — 🟢 Low

**File:** `src/core/renderer/RenderGraph.h`

**Problem:** Hard-coded to `unique_ptr<IRenderPass>`. If a second pass hierarchy (e.g., `IComputePass`) is introduced, a second near-identical graph class is needed.

**Template design:**
```cpp
template <typename PassBase = IRenderPass>
class RenderGraph {
public:
    void addPass(const std::string& name, std::unique_ptr<PassBase> pass);
    void execute(const FrameParams& params, std::vector<PassTiming>* timings = nullptr);
    void clear();
private:
    std::vector<std::string>              names_;
    std::vector<std::unique_ptr<PassBase>> passes_;
};
```

**Pros:**
- Zero code duplication if a compute or ray-tracing pass hierarchy is added
- `RenderGraph<IRenderPass>` is identical to current behaviour

**Cons:**
- Template body stays in header — already the case since `RenderGraph.h` is currently header-only
- Premature generalisation; only valuable if a second pass base is actually planned

---

## Recommended Order of Implementation

| Step | Action | Expected benefit |
|---|---|---|
| 1 | PIMPL `ModelLoader` | Biggest compile-time win; unblocks `MeshInfo` cleanup |
| 2 | Template `Shader::setUniform` | Small effort, eliminates 8 duplicate methods |
| 3 | PIMPL `Renderer` | Clean public ABI; removes all incorrectly-public members |
| 4 | `ResourceCache<T>` from `TextureCache` | Reusable foundation for future resource managers |
| 5 | Template `BufferObject<VertexType>` | Needed when skinned mesh support is added |
| 6 | PIMPL `ConfigParser` | Hides json.hpp; do after designing clean query API |
| 7 | CRTP `RenderPassBase` | Do only if pass count grows significantly |
| 8 | Template `RenderGraph` | Do only if a second pass hierarchy is added |
