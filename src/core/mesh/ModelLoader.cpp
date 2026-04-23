#include "ModelLoader.h"
#include "gl/gl_headers.h"
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

// tinygltf — implementation compiled separately in tinygltf_impl.cc
#include "tiny_gltf.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "BufferObjectData.h"
#include "log/mylog.h"
#include "texture/TextureCache.h"

// ---- static helpers ------------------------------------------------

static glm::mat4 GetNodeLocalTransform(const tinygltf::Node& node) {
    if (node.matrix.size() == 16) {
        // Column-major matrix stored directly in glTF
        return glm::make_mat4(node.matrix.data());
    }
    glm::mat4 T(1.0f), R(1.0f), S(1.0f);
    if (node.translation.size() == 3) {
        T = glm::translate(glm::mat4(1.0f),
                           glm::vec3((float)node.translation[0],
                                     (float)node.translation[1],
                                     (float)node.translation[2]));
    }
    if (node.rotation.size() == 4) {
        // glTF stores [x, y, z, w]; glm::quat ctor takes (w, x, y, z)
        glm::quat q((float)node.rotation[3],
                    (float)node.rotation[0],
                    (float)node.rotation[1],
                    (float)node.rotation[2]);
        R = glm::mat4_cast(q);
    }
    if (node.scale.size() == 3) {
        S = glm::scale(glm::mat4(1.0f),
                       glm::vec3((float)node.scale[0],
                                 (float)node.scale[1],
                                 (float)node.scale[2]));
    }
    return T * R * S;
}

static std::vector<glm::vec3> ReadVec3Accessor(const tinygltf::Model& model, int idx) {
    const auto& acc = model.accessors[idx];
    const auto& bv  = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[bv.buffer];
    const uint8_t* base = buf.data.data() + bv.byteOffset + acc.byteOffset;
    size_t stride = bv.byteStride > 0 ? bv.byteStride : sizeof(float) * 3;
    std::vector<glm::vec3> out;
    out.reserve(acc.count);
    for (size_t i = 0; i < acc.count; ++i) {
        const float* f = reinterpret_cast<const float*>(base + i * stride);
        out.push_back({f[0], f[1], f[2]});
    }
    return out;
}

static std::vector<glm::vec4> ReadVec4Accessor(const tinygltf::Model& model, int idx) {
    const auto& acc = model.accessors[idx];
    const auto& bv  = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[bv.buffer];
    const uint8_t* base = buf.data.data() + bv.byteOffset + acc.byteOffset;
    size_t stride = bv.byteStride > 0 ? bv.byteStride : sizeof(float) * 4;
    std::vector<glm::vec4> out;
    out.reserve(acc.count);
    for (size_t i = 0; i < acc.count; ++i) {
        const float* f = reinterpret_cast<const float*>(base + i * stride);
        out.push_back({f[0], f[1], f[2], f[3]});
    }
    return out;
}

static std::vector<glm::vec2> ReadTexCoordAccessor(const tinygltf::Model& model, int idx) {
    const auto& acc = model.accessors[idx];
    const auto& bv  = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[bv.buffer];
    const uint8_t* base = buf.data.data() + bv.byteOffset + acc.byteOffset;
    std::vector<glm::vec2> out;
    out.reserve(acc.count);
    if (acc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        size_t stride = bv.byteStride > 0 ? bv.byteStride : sizeof(float) * 2;
        for (size_t i = 0; i < acc.count; ++i) {
            const float* f = reinterpret_cast<const float*>(base + i * stride);
            out.push_back({f[0], f[1]});
        }
    } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        size_t stride = bv.byteStride > 0 ? bv.byteStride : 2;
        for (size_t i = 0; i < acc.count; ++i) {
            const uint8_t* p = base + i * stride;
            out.push_back({p[0] / 255.0f, p[1] / 255.0f});
        }
    } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        size_t stride = bv.byteStride > 0 ? bv.byteStride : 4;
        for (size_t i = 0; i < acc.count; ++i) {
            const uint16_t* p = reinterpret_cast<const uint16_t*>(base + i * stride);
            out.push_back({p[0] / 65535.0f, p[1] / 65535.0f});
        }
    }
    return out;
}

static std::vector<unsigned int> ReadIndices(const tinygltf::Model& model, int idx) {
    const auto& acc = model.accessors[idx];
    const auto& bv  = model.bufferViews[acc.bufferView];
    const auto& buf = model.buffers[bv.buffer];
    const uint8_t* base = buf.data.data() + bv.byteOffset + acc.byteOffset;
    std::vector<unsigned int> out;
    out.reserve(acc.count);
    if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
        for (size_t i = 0; i < acc.count; ++i)
            out.push_back(reinterpret_cast<const uint32_t*>(base)[i]);
    } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        for (size_t i = 0; i < acc.count; ++i)
            out.push_back(reinterpret_cast<const uint16_t*>(base)[i]);
    } else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
        for (size_t i = 0; i < acc.count; ++i)
            out.push_back(base[i]);
    }
    return out;
}

// ---- Impl ----------------------------------------------------------

struct ModelLoader::Impl {
    std::vector<Texture> m_textures_loaded;

    bool LoadModel(const std::string& resPath,
                   std::vector<BufferObjectData>& meshInfo,
                   ConfigParser& vehInfo,
                   const std::unordered_map<std::string, imageParam>& textureData,
                   TextureCache& textureCache);

    void ProcessNode(const tinygltf::Model& model,
                     int nodeIdx,
                     const glm::mat4& parentTransform,
                     std::vector<BufferObjectData>& meshInfo,
                     ConfigParser& vehInfo,
                     const std::unordered_map<std::string, imageParam>& textureData,
                     TextureCache& textureCache);

    void ProcessPrimitive(const tinygltf::Model& model,
                          const tinygltf::Primitive& prim,
                          const glm::mat4& worldTransform,
                          const std::string& meshName,
                          const tinygltf::Material& mat,
                          int matIndex,
                          std::vector<BufferObjectData>& meshInfo,
                          ConfigParser& vehInfo,
                          const std::unordered_map<std::string, imageParam>& textureData,
                          TextureCache& textureCache);

    std::vector<Texture> LoadTexturesByConfig(
        const std::string& meshName,
        const std::string& matName,
        ConfigParser& vehInfo,
        const std::unordered_map<std::string, imageParam>& textureData,
        TextureCache& textureCache);

    Texture LoadSingleTexture(const std::string& textureName,
                               unsigned int bindId,
                               const char* typeName,
                               const std::unordered_map<std::string, imageParam>& textureData,
                               TextureCache& textureCache,
                               bool isSrgb);

    myMaterial BuildMaterial(const tinygltf::Material& mat, int matIndex);
};

// ---- public interface ----------------------------------------------

ModelLoader::ModelLoader() : pImpl_(std::make_unique<Impl>()) {}
ModelLoader::~ModelLoader() = default;

bool ModelLoader::LoadModel(const std::string& resPath,
                            std::vector<BufferObjectData>& meshInfo,
                            ConfigParser& cfgInfo,
                            const std::unordered_map<std::string, imageParam>& textureData,
                            TextureCache& textureCache) {
    return pImpl_->LoadModel(resPath, meshInfo, cfgInfo, textureData, textureCache);
}

// ---- Impl definitions ----------------------------------------------

bool ModelLoader::Impl::LoadModel(const std::string& resPath,
                                  std::vector<BufferObjectData>& meshInfo,
                                  ConfigParser& vehInfo,
                                  const std::unordered_map<std::string, imageParam>& textureData,
                                  TextureCache& textureCache) {
    tinygltf::TinyGLTF loader;
    // No-op image loader: textures are supplied via ConfigParser/TextureCache, not embedded.
    loader.SetImageLoader(
        [](tinygltf::Image*, const int, std::string*, std::string*,
           int, int, const unsigned char*, int, void*) -> bool { return true; },
        nullptr);

    tinygltf::Model model;
    std::string err, warn;
    bool ok = loader.LoadBinaryFromFile(&model, &err, &warn, resPath);
    if (!warn.empty()) LOG_W("[ModelLoader] {}", warn);
    if (!ok) {
        LOG_E("[ModelLoader] Failed to load GLB: {} — {}", resPath, err);
        return false;
    }

    LOG_I("[ModelLoader] GLB loaded: {} mesh(es), {} material(s), {} node(s)",
          model.meshes.size(), model.materials.size(), model.nodes.size());

    if (model.scenes.empty()) {
        LOG_E("[ModelLoader] No scenes in {}", resPath);
        return false;
    }
    int sceneIdx = model.defaultScene >= 0 ? model.defaultScene : 0;
    for (int nodeIdx : model.scenes[sceneIdx].nodes) {
        ProcessNode(model, nodeIdx, glm::mat4(1.0f), meshInfo, vehInfo, textureData, textureCache);
    }
    return true;
}

void ModelLoader::Impl::ProcessNode(const tinygltf::Model& model,
                                    int nodeIdx,
                                    const glm::mat4& parentTransform,
                                    std::vector<BufferObjectData>& meshInfo,
                                    ConfigParser& vehInfo,
                                    const std::unordered_map<std::string, imageParam>& textureData,
                                    TextureCache& textureCache) {
    const tinygltf::Node& node = model.nodes[nodeIdx];
    const glm::mat4 world = parentTransform * GetNodeLocalTransform(node);

    if (node.mesh >= 0) {
        const tinygltf::Mesh& mesh = model.meshes[node.mesh];
        // Use node name for config lookup so it matches the original FBX/OBJ naming.
        // Blender GLB export preserves node names while Assimp would append "-0", "-1"
        // suffixes for multi-primitive meshes, which broke config lookups.
        const std::string meshName = !node.name.empty() ? node.name : mesh.name;

        for (const auto& prim : mesh.primitives) {
            if (prim.mode != TINYGLTF_MODE_TRIANGLES) continue;
            if (prim.material < 0 || prim.material >= (int)model.materials.size()) continue;
            const tinygltf::Material& mat = model.materials[prim.material];
            ProcessPrimitive(model, prim, world, meshName, mat, prim.material,
                             meshInfo, vehInfo, textureData, textureCache);
        }
    }

    for (int child : node.children) {
        ProcessNode(model, child, world, meshInfo, vehInfo, textureData, textureCache);
    }
}

void ModelLoader::Impl::ProcessPrimitive(const tinygltf::Model& model,
                                          const tinygltf::Primitive& prim,
                                          const glm::mat4& worldTransform,
                                          const std::string& meshName,
                                          const tinygltf::Material& mat,
                                          int matIndex,
                                          std::vector<BufferObjectData>& meshInfo,
                                          ConfigParser& vehInfo,
                                          const std::unordered_map<std::string, imageParam>& textureData,
                                          TextureCache& textureCache) {
    // ---- positions ----
    auto posIt = prim.attributes.find("POSITION");
    if (posIt == prim.attributes.end()) {
        LOG_W("[ModelLoader] Primitive in mesh '{}' has no POSITION — skipped", meshName);
        return;
    }
    auto positions = ReadVec3Accessor(model, posIt->second);
    std::vector<Vertex> vertices(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        glm::vec4 p = worldTransform * glm::vec4(positions[i], 1.0f);
        vertices[i].Position  = glm::vec3(p);
        vertices[i].Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices[i].TexCoords = glm::vec2(0.0f);
        vertices[i].Tangent   = glm::vec3(0.0f);
        vertices[i].Bitangent = glm::vec3(0.0f);
    }

    // ---- normals ----
    auto normIt = prim.attributes.find("NORMAL");
    if (normIt != prim.attributes.end()) {
        auto normals = ReadVec3Accessor(model, normIt->second);
        for (size_t i = 0; i < normals.size() && i < vertices.size(); ++i)
            vertices[i].Normal = normals[i];
    }

    // ---- texcoords (flip V for OpenGL top-left → bottom-left) ----
    auto uvIt = prim.attributes.find("TEXCOORD_0");
    if (uvIt != prim.attributes.end()) {
        auto uvs = ReadTexCoordAccessor(model, uvIt->second);
        for (size_t i = 0; i < uvs.size() && i < vertices.size(); ++i)
            vertices[i].TexCoords = glm::vec2(uvs[i].x, 1.0f - uvs[i].y);
    }

    // ---- tangents (VEC4: xyz = tangent, w = handedness) ----
    auto tanIt = prim.attributes.find("TANGENT");
    if (tanIt != prim.attributes.end()) {
        auto tangents = ReadVec4Accessor(model, tanIt->second);
        for (size_t i = 0; i < tangents.size() && i < vertices.size(); ++i) {
            vertices[i].Tangent   = glm::vec3(tangents[i]);
            vertices[i].Bitangent = glm::cross(vertices[i].Normal,
                                                glm::vec3(tangents[i])) * tangents[i].w;
        }
    } else {
        LOG_I("[ModelLoader] {} — no TANGENT attribute, will compute from geometry", meshName);
    }

    // ---- indices ----
    std::vector<unsigned int> indices;
    if (prim.indices >= 0)
        indices = ReadIndices(model, prim.indices);

    // ---- compute tangent basis when not provided (prevents NaN in shader) ----
    if (tanIt == prim.attributes.end() && !indices.empty()) {
        std::vector<glm::vec3> accum_t(vertices.size(), glm::vec3(0.0f));
        std::vector<glm::vec3> accum_b(vertices.size(), glm::vec3(0.0f));
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            const uint32_t i0 = indices[i], i1 = indices[i + 1], i2 = indices[i + 2];
            const glm::vec3 dp1 = vertices[i1].Position - vertices[i0].Position;
            const glm::vec3 dp2 = vertices[i2].Position - vertices[i0].Position;
            const glm::vec2 duv1 = vertices[i1].TexCoords - vertices[i0].TexCoords;
            const glm::vec2 duv2 = vertices[i2].TexCoords - vertices[i0].TexCoords;
            const float det = duv1.x * duv2.y - duv1.y * duv2.x;
            if (std::abs(det) < 1e-8f) continue;
            const float r = 1.0f / det;
            const glm::vec3 t = (dp1 * duv2.y - dp2 * duv1.y) * r;
            const glm::vec3 b = (dp2 * duv1.x - dp1 * duv2.x) * r;
            accum_t[i0] += t; accum_t[i1] += t; accum_t[i2] += t;
            accum_b[i0] += b; accum_b[i1] += b; accum_b[i2] += b;
        }
        for (size_t i = 0; i < vertices.size(); ++i) {
            const glm::vec3& n = vertices[i].Normal;
            const glm::vec3& t = accum_t[i];
            if (glm::dot(t, t) < 1e-8f) {
                // Degenerate UV island — build arbitrary orthonormal basis from normal
                const glm::vec3 up = std::abs(n.y) < 0.999f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
                vertices[i].Tangent   = glm::normalize(glm::cross(up, n));
                vertices[i].Bitangent = glm::cross(n, vertices[i].Tangent);
            } else {
                // Gram-Schmidt orthogonalization
                const glm::vec3 tangent = glm::normalize(t - n * glm::dot(n, t));
                const float sign = (glm::dot(glm::cross(n, t), accum_b[i]) < 0.0f) ? -1.0f : 1.0f;
                vertices[i].Tangent   = tangent;
                vertices[i].Bitangent = glm::cross(n, tangent) * sign;
            }
        }
    }

    // ---- textures ----
    std::vector<Texture> textures;
    if (vehInfo.needTexture(meshName, mat.name)) {
        auto t0 = std::chrono::high_resolution_clock::now();
        textures = LoadTexturesByConfig(meshName, mat.name, vehInfo, textureData, textureCache);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now() - t0).count();
        LOG_I("[ModelLoader] mesh {} material {} texture load cost: {} ms",
              meshName, mat.name, ms);
    }

    // ---- material ----
    myMaterial mMaterial = BuildMaterial(mat, matIndex);

    meshInfo.emplace_back(std::move(vertices), std::move(indices),
                          std::move(textures), mMaterial, meshName);
}

std::vector<Texture> ModelLoader::Impl::LoadTexturesByConfig(
    const std::string& meshName,
    const std::string& matName,
    ConfigParser& vehInfo,
    const std::unordered_map<std::string, imageParam>& textureData,
    TextureCache& textureCache) {

    std::vector<Texture> textures;
    const TextureData td = vehInfo.getTextureData(meshName, matName);

    auto addTex = [&](const std::string& path, unsigned int slot,
                      const char* typeName, bool isSrgb) {
        if (path.empty()) return;
        Texture t = LoadSingleTexture(path, slot, typeName, textureData, textureCache, isSrgb);
        if (t.id) textures.push_back(t);
    };

    addTex(td.diffuse,   0, "texture_diffuse",   true);
    addTex(td.specular,  1, "texture_specular",  true);
    addTex(td.normal,    2, "texture_normal",     false);
    addTex(td.ao,        3, "texture_ao",         false);
    addTex(td.alpha,     4, "texture_alpha",      false);
    addTex(td.roughness, 5, "texture_roughness",  false);
    addTex(td.metallic,  6, "texture_metallic",   false);

    return textures;
}

Texture ModelLoader::Impl::LoadSingleTexture(
    const std::string& textureName,
    unsigned int bindId,
    const char* typeName,
    const std::unordered_map<std::string, imageParam>& textureData,
    TextureCache& textureCache,
    bool isSrgb) {

    Texture texture{};
    texture.bindId = bindId;
    texture.type   = typeName;
    texture.path   = textureName;

    // Return cached texture if already uploaded.
    for (const auto& cached : m_textures_loaded) {
        if (cached.path == textureName) {
            texture.id = cached.id;
            return texture;
        }
    }

    auto endsWithKtx = [](const std::string& p) {
        if (p.size() >= 5 && (p.compare(p.size()-5, 5, ".ktx2") == 0 ||
                               p.compare(p.size()-5, 5, ".KTX2") == 0)) return true;
        if (p.size() >= 4 && (p.compare(p.size()-4, 4, ".ktx") == 0  ||
                               p.compare(p.size()-4, 4, ".KTX") == 0))  return true;
        return false;
    };

    if (endsWithKtx(textureName)) {
        texture.id = textureCache.getOrCreateKtx(textureName);
    } else {
        auto it = textureData.find(textureName);
        if (it == textureData.end()) {
            LOG_W("[ModelLoader] Texture data missing for {}", textureName);
            return texture;
        }
        texture.id = textureCache.getOrCreate(textureName, it->second, isSrgb);
    }

    if (texture.id == 0) {
        LOG_E("[ModelLoader] Failed to upload texture: {}", textureName);
        assert(false);
        return texture;
    }

    m_textures_loaded.push_back(texture);
    return texture;
}

myMaterial ModelLoader::Impl::BuildMaterial(const tinygltf::Material& mat, int matIndex) {
    myMaterial m{};
    m.diffuseColor      = glm::vec3(0.8f);
    m.specularColor     = glm::vec3(0.2f);
    m.ambientColor      = glm::vec3(0.05f);
    m.shininess         = 32.0f;
    m.Opacity           = 1.0f;
    m.TransparencyFactor = 0.0f;
    m.ShininessStrength  = 1.0f;
    m.MaterialName      = mat.name;
    m.MaterialIndex     = static_cast<unsigned int>(matIndex);

    // Map PBR baseColorFactor → diffuseColor + opacity
    const auto& pbr = mat.pbrMetallicRoughness;
    if (pbr.baseColorFactor.size() >= 3) {
        m.diffuseColor = glm::vec3((float)pbr.baseColorFactor[0],
                                   (float)pbr.baseColorFactor[1],
                                   (float)pbr.baseColorFactor[2]);
    }
    if (pbr.baseColorFactor.size() >= 4) {
        m.Opacity = (float)pbr.baseColorFactor[3];
    }
    if (mat.alphaMode == "BLEND") {
        m.TransparencyFactor = 1.0f - m.Opacity;
    }
    return m;
}
