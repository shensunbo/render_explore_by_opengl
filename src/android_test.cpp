#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>

#include "renderer_api.h"
#include "log/mylog.h"

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static RendererAPI rendererAPI;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static GLFWwindow* windowAndGlInit(int width, int height)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    GLFWwindow* wnd = glfwCreateWindow(width, height, "RendererAPI Test", NULL, NULL);
    if (wnd == NULL)
    {
        mylog(LogLevel::E, "Failed to create GLFW window");
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(wnd);
    glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        mylog(LogLevel::E, "Failed to initialize GLAD");
        return nullptr;
    }

    return wnd;
}

int main()
{
    mylog(LogLevel::I, "Starting RendererAPI Test");

    GLFWwindow* window = windowAndGlInit(SCR_WIDTH, SCR_HEIGHT);
    if (!window)
    {
        mylog(LogLevel::E, "Window initialization failed");
        return -1;
    }

    // Initialize RendererAPI
    rendererAPI.init(SCR_WIDTH, SCR_HEIGHT, "../");

    mylog(LogLevel::I, "Entering render loop");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // update
        rendererAPI.update();

        // render
        rendererAPI.render();

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    rendererAPI.deinit();
    glfwTerminate();

    mylog(LogLevel::I, "RendererAPI Test completed");
    return 0;
}