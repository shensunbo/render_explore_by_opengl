#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. 初始化 EGL
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    // 2. 配置 EGL
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    // 3. 创建离屏 Surface（PBuffer）
    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, 512,  // 离屏缓冲区宽度
        EGL_HEIGHT, 512, // 离屏缓冲区高度
        EGL_NONE
    };
    EGLSurface surface = eglCreatePbufferSurface(display, config, pbufferAttribs);

    // 4. 创建 OpenGL ES 上下文
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(display, surface, surface, context);

    // 5. 使用 OpenGL ES 渲染（离屏）
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // 红色背景
    glClear(GL_COLOR_BUFFER_BIT);

    // 6. 读取像素数据（可选）
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

    // 7. 清理资源
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);

    return 0;
}