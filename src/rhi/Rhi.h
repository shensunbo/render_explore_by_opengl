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

inline void drawIndexedTriangles(std::size_t indexCount) {
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
}

} // namespace rhi
