#pragma once

#include "VehicleShader.h"


class FboHandler {
public:
    // FboHandler() = delete;
    FboHandler():shader_("res/shader/fbo_rect.vs", "res/shader/fbo_rect.fs") {
    }

    ~FboHandler(){
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &rbo);
    }
   

    void init(unsigned int width, unsigned int height);
    void renderToFullscreenQuad();

    void setFboState(bool state){
        fboState_ = state;
    }

    void enable();
    void checkFboStatus();

private:
    void initFullscreenQuad();
    void initFramebuffer(unsigned int width, unsigned int height);
private:
    VehicleShader shader_;
    GLuint quadVAO_;
    GLuint quadVBO_;
    GLuint fbo; 
    GLuint texture;
    GLuint rbo;

    bool fboState_ = false;
};