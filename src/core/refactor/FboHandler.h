#pragma once

#include "VehicleShader.h"


class FboHandler {
public:
    FboHandler() = delete;
    FboHandler(unsigned int width, unsigned int height):
        width_(width),
        height_(height),
        shader_("res/shader/fbo_rect.vs", "res/shader/fbo_rect.fs") {
    }

    ~FboHandler(){
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &rbo);
    }
   

    void init();
    void renderToFullscreenQuad();

    void setFboState(bool state){
        fboState_ = state;
    }

    void enable();
    void checkFboStatus();

    void dumpTextureToFile(const char* filename);

private:
    void initFullscreenQuad();
    void initFramebuffer();
private:
    unsigned int width_;
    unsigned int height_;

    VehicleShader shader_;
    GLuint quadVAO_;
    GLuint quadVBO_;
    GLuint fbo; 
    GLuint texture;
    GLuint rbo;

    

    bool fboState_ = false;
};