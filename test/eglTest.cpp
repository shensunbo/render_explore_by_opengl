#include <EGL/egl.h>
#include <GLES3/gl3.h>  // Change to GLES3 header
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. 初始化 EGL
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    // 2. 配置 EGL for GLES 3.0
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,  // Change to ES3 bit
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    // 3. 创建离屏 Surface（PBuffer）
    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 512,
        EGL_HEIGHT, 512,
        EGL_NONE
    };
    EGLSurface surface = eglCreatePbufferSurface(display, config, pbufferAttribs);

    // 4. 创建 OpenGL ES 3.0 上下文
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,  // Change to version 3
        EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(display, surface, surface, context);

    // 5. 使用 OpenGL ES 3.0 渲染
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 6. 读取像素数据
    unsigned char *pixels = (unsigned char *)malloc(512 * 512 * 4);
    glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // 7. 保存像素数据到文件
    FILE *file = fopen("render_result.raw", "wb");
    if (file) {
        fwrite(pixels, 1, 512 * 512 * 4, file);
        fclose(file);
        printf("Render result saved to render_result.raw\n");
    } else {
        printf("Failed to open file for writing\n");
    }

    // 8. 清理资源
    free(pixels);  // Don't forget to free allocated memory
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);

    return 0;
}