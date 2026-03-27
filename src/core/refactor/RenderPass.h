#pragma once

#include <memory>
#include <vector>
#include "Shader.h"
#include "Skybox.h"
#include "FboHandler.h"
#include "CommonDataStruct.h"
#include "BufferObjectData.h"
#include <glm/glm.hpp>


/**
 * @struct FrameParams
 * @brief Parameters passed to each render pass for frame rendering.
 */
struct FrameParams;


/**
 * @class IRenderPass
 * @brief Abstract base class for all render passes.
 *
 * Defines the interface for executing a render pass with given frame parameters.
 */
class IRenderPass {
public:
    /**
     * @brief Virtual destructor for IRenderPass.
     */
    virtual ~IRenderPass() = default;

    /**
     * @brief Executes the render pass with the given frame parameters.
     * @param params Frame parameters for rendering.
     */
    virtual void execute(const FrameParams& params) = 0;
};


/**
 * @class ScenePass
 * @brief Render pass for drawing the main scene meshes and skybox.
 */
class ScenePass : public IRenderPass {
public:
    /**
     * @brief Constructs a ScenePass with shader, skybox, and mesh data.
     * @param shader Pointer to the shader.
     * @param skybox Pointer to the skybox object.
     * @param meshes Pointer to the vector of mesh data.
     */
    ScenePass(Shader* shader, Skybox* skybox,
              const std::vector<BufferObjectData>* meshes)
        : shader_(shader), skybox_(skybox), meshes_(meshes) {}

    /**
     * @brief Executes the scene render pass.
     * @param params Frame parameters for rendering.
     */
    void execute(const FrameParams& params) override;
private:
    Shader* shader_; ///< Pointer to the shader.
    Skybox* skybox_; ///< Pointer to the skybox object.
    const std::vector<BufferObjectData>* meshes_; ///< Pointer to the mesh data.
};


/**
 * @class SkyboxPass
 * @brief Render pass for drawing the skybox.
 */
class SkyboxPass : public IRenderPass {
public:
    /**
     * @brief Constructs a SkyboxPass with a skybox object.
     * @param skybox Pointer to the skybox object.
     */
    SkyboxPass(Skybox* skybox) : skybox_(skybox) {}

    /**
     * @brief Executes the skybox render pass.
     * @param params Frame parameters for rendering.
     */
    void execute(const FrameParams& params) override;
private:
    Skybox* skybox_; ///< Pointer to the skybox object.
};


/**
 * @class PostPass
 * @brief Render pass for post-processing using a framebuffer object.
 */
class PostPass : public IRenderPass {
public:
    /**
     * @brief Constructs a PostPass with a framebuffer handler.
     * @param fbo Pointer to the framebuffer handler.
     */
    PostPass(FboHandler* fbo) : fbo_(fbo) {}

    /**
     * @brief Executes the post-processing render pass.
     * @param params Frame parameters for rendering.
     */
    void execute(const FrameParams& params) override;
private:
    FboHandler* fbo_; ///< Pointer to the framebuffer handler.
};
