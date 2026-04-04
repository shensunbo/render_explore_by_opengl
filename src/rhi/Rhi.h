#pragma once

#include <cstddef>
#include <cstdint>
#include "gl/gl_headers.h"


/**
 * @namespace rhi
 * @brief Contains inline functions for OpenGL resource and state management.
 *
 * The rhi namespace provides a set of utility functions to simplify common OpenGL operations
 * such as buffer creation, binding, framebuffer management, and drawing.
 */
namespace rhi {

/**
 * @brief Enables or disables OpenGL depth testing.
 * @param enable True to enable depth testing, false to disable.
 */
inline void enableDepthTest(bool enable) {
    if (enable) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
}

/**
 * @brief Sets the OpenGL clear color.
 * @param r Red component (0.0-1.0)
 * @param g Green component (0.0-1.0)
 * @param b Blue component (0.0-1.0)
 * @param a Alpha component (0.0-1.0)
 */
inline void setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

/**
 * @brief Clears both the color and depth buffers.
 */
inline void clearColorDepth() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief Clears only the color buffer.
 */
inline void clearColor() { glClear(GL_COLOR_BUFFER_BIT); }

/**
 * @brief Sets the active texture unit.
 * @param slot Texture unit index.
 */
inline void setActiveTexture(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

/**
 * @brief Binds a 2D texture to the current texture unit.
 * @param textureId OpenGL texture ID.
 */
inline void bindTexture2D(uint32_t textureId) {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

/**
 * @brief Binds a vertex array object.
 * @param vao OpenGL VAO ID.
 */
inline void bindVertexArray(uint32_t vao) {
    glBindVertexArray(vao);
}

/**
 * @brief Binds a uniform buffer object to a binding point.
 * @param binding Binding point index.
 * @param ubo OpenGL UBO ID.
 */
inline void bindUniformBuffer(uint32_t binding, uint32_t ubo) {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
}

/**
 * @brief Binds a framebuffer object.
 * @param fbo OpenGL FBO ID.
 */
inline void bindFramebuffer(uint32_t fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

/**
 * @brief Creates a new framebuffer object.
 * @return OpenGL FBO ID.
 */
inline uint32_t createFramebuffer() {
    GLuint id = 0;
    glGenFramebuffers(1, &id);
    return id;
}

/**
 * @brief Deletes a framebuffer object.
 * @param fbo OpenGL FBO ID.
 */
inline void deleteFramebuffer(uint32_t fbo) {
    glDeleteFramebuffers(1, &fbo);
}

/**
 * @brief Attaches a 2D texture to a framebuffer.
 * @param attachment Attachment point (e.g., GL_COLOR_ATTACHMENT0).
 * @param texture OpenGL texture ID.
 */
inline void setFramebufferTexture2D(uint32_t attachment, uint32_t texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

/**
 * @brief Creates a 2D texture with the specified parameters.
 * @param width Texture width.
 * @param height Texture height.
 * @param internalFormat Internal OpenGL format.
 * @param format Pixel data format.
 * @param type Data type of pixel data.
 * @return OpenGL texture ID.
 */
inline uint32_t createTexture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type) {
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return id;
}

/**
 * @brief Deletes a 2D texture.
 * @param tex OpenGL texture ID.
 */
inline void deleteTexture(uint32_t tex) {
    glDeleteTextures(1, &tex);
}

/**
 * @brief Creates a new renderbuffer object.
 * @return OpenGL renderbuffer ID.
 */
inline uint32_t createRenderbuffer() {
    GLuint id = 0;
    glGenRenderbuffers(1, &id);
    return id;
}

/**
 * @brief Allocates storage for a renderbuffer object.
 * @param rbo OpenGL renderbuffer ID.
 * @param width Width of the renderbuffer.
 * @param height Height of the renderbuffer.
 */
inline void setupRenderbufferStorage(uint32_t rbo, int width, int height) {
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

/**
 * @brief Attaches a renderbuffer to a framebuffer.
 * @param attachment Attachment point (e.g., GL_DEPTH_ATTACHMENT).
 * @param rbo OpenGL renderbuffer ID.
 */
inline void attachRenderbuffer(uint32_t attachment, uint32_t rbo) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
}

/**
 * @brief Deletes a renderbuffer object.
 * @param rbo OpenGL renderbuffer ID.
 */
inline void deleteRenderbuffer(uint32_t rbo) {
    glDeleteRenderbuffers(1, &rbo);
}

/**
 * @brief Checks if the currently bound framebuffer is complete.
 * @return True if framebuffer is complete, false otherwise.
 */
inline bool framebufferComplete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

/**
 * @brief Enables or disables depth testing (alias for enableDepthTest).
 * @param enable True to enable depth testing, false to disable.
 */
inline void enableDepth(bool enable) { enableDepthTest(enable); }

/**
 * @brief Draws indexed triangles using the currently bound index buffer.
 * @param indexCount Number of indices to draw.
 */
inline void drawIndexedTriangles(std::size_t indexCount) {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
}

/**
 * @brief Draws indexed triangles using instanced rendering.
 * @param indexCount Number of indices per instance.
 * @param instanceCount Number of instances to render.
 */
inline void drawIndexedTrianglesInstanced(std::size_t indexCount, int instanceCount) {
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indexCount),
                            GL_UNSIGNED_INT, nullptr, instanceCount);
}

} // namespace rhi
