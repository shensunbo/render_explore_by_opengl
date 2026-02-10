#include  "FboHandler.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <memory>
#include <chrono>
#include "rhi/Rhi.h"

void FboHandler::init(){
    initFullscreenQuad();
    initFramebuffer();
}

void FboHandler::initFullscreenQuad(){
    // fboShader.setInt("fboTexture", 0); // not necessary
    // Setup screen VAO
    GLfloat quadVertices[] = {   // Vertex attributes for a full-screen quad in NDC.
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };	

    glGenVertexArrays(1, &quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glBindVertexArray(quadVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
}

void FboHandler::initFramebuffer(){
    assert(width_ > 0 && height_ > 0);
    // Create framebuffer.
    fbo = rhi::createFramebuffer();
    rhi::bindFramebuffer(fbo);

    // Create color texture attachment.
    texture = rhi::createTexture2D(width_, height_, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    rhi::setFramebufferTexture2D(GL_COLOR_ATTACHMENT0, texture);

    // Create renderbuffer for depth and stencil attachments.
    rbo = rhi::createRenderbuffer();
    rhi::setupRenderbufferStorage(rbo, width_, height_);
    rhi::attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, rbo);

    // Validate framebuffer completeness.
    if(!rhi::framebufferComplete()){
        mylog(LogLevel::E, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        MY_ASSERT(false, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    rhi::bindFramebuffer(0);
}

void FboHandler::renderToFullscreenQuad(){
    rhi::bindFramebuffer(0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    shader_.use();  
    glBindVertexArray(quadVAO_);
    rhi::setActiveTexture(0);
    rhi::bindTexture2D(texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void FboHandler::enable(){
    rhi::bindFramebuffer(fbo);
}

void FboHandler::dumpTextureToFile(const char* filename) {
    auto dumpStartTime = std::chrono::high_resolution_clock::now();
    assert(width_ > 0 && height_ > 0);

    unsigned char* pixels = new unsigned char[width_ * height_ * 3];
    
#ifdef __ANDROID__
    // Android OpenGL ES: use FBO + glReadPixels to read texture
    GLuint tempFbo;
    glGenFramebuffers(1, &tempFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, tempFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    }
    
    glDeleteFramebuffers(1, &tempFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#else
    // Desktop OpenGL: use glGetTexImage
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
#endif
    
    stbi_write_png(filename, width_, height_, 3, pixels, width_ * 3);
    
    delete[] pixels;

    auto dumpEndTime = std::chrono::high_resolution_clock::now();
    mylog(LogLevel::I, "Dump texture to file %s, width %d, height %d, time %f ms", filename, width_, height_, std::chrono::duration<double, std::milli>(dumpEndTime - dumpStartTime).count());
}