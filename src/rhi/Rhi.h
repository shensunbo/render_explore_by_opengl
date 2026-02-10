#pragma once

#include <cstddef>
#include <cstdint>
#include "gl/gl_headers.h"

namespace rhi {

inline void enableDepthTest(bool enable) {
    if (enable) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
}

inline void setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

inline void clearColorDepth() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

inline void clearColor() { glClear(GL_COLOR_BUFFER_BIT); }

inline void setActiveTexture(uint32_t slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

inline void bindTexture2D(uint32_t textureId) {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

inline void bindVertexArray(uint32_t vao) {
    glBindVertexArray(vao);
}

inline void bindUniformBuffer(uint32_t binding, uint32_t ubo) {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
}

inline void bindFramebuffer(uint32_t fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

inline uint32_t createFramebuffer() {
    GLuint id = 0;
    glGenFramebuffers(1, &id);
    return id;
}

inline void deleteFramebuffer(uint32_t fbo) {
    glDeleteFramebuffers(1, &fbo);
}

inline void setFramebufferTexture2D(uint32_t attachment, uint32_t texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

inline uint32_t createTexture2D(int width, int height, GLint internalFormat, GLenum format, GLenum type) {
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return id;
}

inline void deleteTexture(uint32_t tex) {
    glDeleteTextures(1, &tex);
}

inline uint32_t createRenderbuffer() {
    GLuint id = 0;
    glGenRenderbuffers(1, &id);
    return id;
}

inline void setupRenderbufferStorage(uint32_t rbo, int width, int height) {
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

inline void attachRenderbuffer(uint32_t attachment, uint32_t rbo) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
}

inline void deleteRenderbuffer(uint32_t rbo) {
    glDeleteRenderbuffers(1, &rbo);
}

inline bool framebufferComplete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

inline void enableDepth(bool enable) { enableDepthTest(enable); }

inline void drawIndexedTriangles(std::size_t indexCount) {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
}

} // namespace rhi
