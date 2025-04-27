#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>

#include <stb_image.h>

// #include "core/shader.h"
// #include "core/camera.h"
#include "core/refactor/VehicleMeshInfo.h"
#include "core/refactor/VehicleShader.h"

#include "log/mylog.h"

#include "VehicleVirCamera.h" 
#include "core/refactor/Skybox.h"
#include "core/refactor/VehicleRenderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void processCameraInput(GLFWwindow* window);

static GLFWwindow* windowAndGlInit(int width, int height);
static bool FrameRatemonitorAnd100msTick(void);
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//TODO: handle callback event 
// camera
VehicleVirCamera camera(glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static unsigned int glerror = 0;

static VehicleRenderer vRender;

int main()
{
    printf("printf test");

    GLFWwindow* window = windowAndGlInit(SCR_WIDTH, SCR_HEIGHT);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    vRender.create();
    vRender.cubemap.ActiveCubeMap();

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        (void)FrameRatemonitorAnd100msTick();
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        processCameraInput(window);
        camera.ProcessJump(deltaTime);

        // render
        // ------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        vRender.ourShader->use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        vRender.ourShader->setMat4("projection", projection);
        vRender.ourShader->setMat4("view", view);


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.7f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.0001f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        vRender.ourShader->setMat4("model", model);

        glm::mat4 look = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        look = camera.GetViewMatrix();
        vRender.ourShader->setMat4("look", look);

        //skybox
        vRender.ourShader->setInt("cubemap", 1);

        vRender.draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void processCameraInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        camera.ProcessKeyboard(UPROLL, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNROLL, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && camera.isOnGround)
        camera.ProcessKeyboard(SPACE, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        camera.ProcessKeyboard(R, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        camera.ProcessKeyboard(K1, deltaTime);


}

static bool FrameRatemonitorAnd100msTick(void) {
    static std::chrono::high_resolution_clock::time_point startTime =
        std::chrono::high_resolution_clock::now();
    static std::chrono::high_resolution_clock::time_point tickStartTime =
        std::chrono::high_resolution_clock::now();
    static int frameCount = 0;

    std::chrono::high_resolution_clock::time_point currentTime =
        std::chrono::high_resolution_clock::now();
    double deltaTime =
        std::chrono::duration<double>(currentTime - startTime).count();
    double tickTime =
        std::chrono::duration<double>(currentTime - tickStartTime).count();

    frameCount++;

    if (deltaTime >= 3) {
        double fps = frameCount / deltaTime;
        std::cout<< "FPS: " << (int)fps << std::endl;
        startTime = currentTime;
        frameCount = 0;
    }

    if (tickTime >= 0.03f) {
        tickStartTime = currentTime;
        return true;
    }

    return false;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        if (!(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
            camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static GLFWwindow* windowAndGlInit(int width, int height){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    GLFWwindow* wnd = glfwCreateWindow(width, height, "explore render", NULL, NULL);
    if (wnd == NULL)
    {
        mylog(LogLevel::E, "Failed to create GLFW window");
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(wnd);
    glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);
    // glfwSetCursorPosCallback(wnd, mouse_callback);
    glfwSetScrollCallback(wnd, scroll_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        mylog(LogLevel::E, "Failed to initialize GLAD");
        return nullptr;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    return wnd;
}