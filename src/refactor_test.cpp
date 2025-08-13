#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// #include "core/shader.h"
// #include "core/camera.h"
#include "core/refactor/VehicleVirCamera.h" 
#include "core/refactor/VehicleMeshInfo.h"
#include "core/refactor/VehicleShader.h"
#include "core/refactor/Skybox.h"
#include "core/refactor/VehicleRenderer.h"
#include "log/mylog.h"

#include "configParser/ConfigParser.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void processCameraInput(GLFWwindow* window);

void dumpTextureToFile(GLuint texture, int width, int height, const char* filename);

static GLFWwindow* windowAndGlInit(int width, int height);
static bool FrameRatemonitorAnd100msTick(void);
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

//TODO: handle callback event 
// camera
VehicleVirCamera camera(glm::vec3(0.0f, 0.0f, 0.9f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// static unsigned int glerror = 0;

static VehicleRenderer vRender;

static bool dumpRes = false;
static bool fboEnable = false;
int main()
{
    mylog(LogLevel::I, "Starting Refactor");

    GLFWwindow* window = windowAndGlInit(SCR_WIDTH, SCR_HEIGHT);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    vRender.create();
    vRender.ourShader->use();
    glm::mat4 skyboxModel = glm::mat4(1.0f);
    skyboxModel = glm::rotate(skyboxModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender.ourShader->setMat4("cubemapRotateMatrix", skyboxModel);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, -0.5f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.0001f));	// it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    vRender.ourShader->setMat4("model", model);

    glm::mat4 projection = glm::mat4(1.0);
    glm::mat4 view = glm::mat4(1.0);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

     // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // FBO test
    VehicleShader fboShader("res/shader/fbo_rect.vs", "res/shader/fbo_rect.fs");
    // fboShader.setInt("fboTexture", 0); // not necessary
    // Setup screen VAO
    GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };	

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // 初始化FBO
    GLuint fbo, texture, rbo;

    // 创建帧缓冲
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // 创建纹理附件
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // 创建渲染缓冲对象作为深度和模板附件
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // 检查完整性
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        mylog(LogLevel::E, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        MY_ASSERT(false, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {   
        // per-frame time logic
        // --------------------
        auto frameStartTime = std::chrono::high_resolution_clock::now();
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        (void)FrameRatemonitorAnd100msTick();

        // input
        // -----
        processInput(window);
        processCameraInput(window);
        // camera.ProcessJump(deltaTime);

        if(fboEnable){
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        }

        // render
        // ------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.5f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        vRender.ourShader->use();
        vRender.cubemap->ActiveCubeMap();

        if(camera.updateEvent){
            // view/projection transformations
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            view = camera.GetViewMatrix();

            glm::mat4 mvp = projection * view * model;
            vRender.ourShader->setMat4("uMVP", mvp);

            glm::mat4 invLook = glm::inverse(view);
            glm::vec3 viewPosition = glm::vec3(invLook[3]);
            vRender.ourShader->setVec3("viewPosition", viewPosition);
        }

        CHECK_GLES_STATUS();

        vRender.ourShader->setInt("cubemap", vRender.cubemap->GetBindingPoint());
        vRender.draw();

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        // glClearColor(0.2f, 0.5f, 0.1f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CHECK_GLES_STATUS();

        if(camera.updateEvent){
            glm::mat4 skymodel = glm::mat4(1.0f);
            view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Remove any translation component of the view matrix
            glm::mat4 mvp = projection * view * skymodel;
            vRender.cubemap->updateMvpMatrix(mvp);
        }
        
        vRender.cubemap->drawSkybox();
        // clear update event
        camera.updateEvent = false;
        
        if(fboEnable){
            if(dumpRes){
                dumpTextureToFile(texture, SCR_WIDTH, SCR_HEIGHT, "dump.png");
                dumpRes = false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_DEPTH_TEST);

            fboShader.use();  
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
        
        // frame end time stamp
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration<float, std::milli>(frameEndTime - frameStartTime).count();
        
        auto swapBufStartTime = std::chrono::high_resolution_clock::now();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto swapBufEndTime = std::chrono::high_resolution_clock::now();
        auto swapBufCostTime = std::chrono::duration<float, std::milli>(swapBufEndTime - swapBufStartTime).count();

        // Optionally print timing information (can be enabled/disabled)
        static bool printTiming = false;
        if (printTiming) {
            std::cout << "Frame Timing (ms):\n"
                    << "  rendering time: " << frameDuration << "\n"
                    << "  swap buffer time: " << swapBufCostTime << "\n"
                    << "------------------------\n";
        }
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void dumpTextureToFile(GLuint texture, int width, int height, const char* filename) {
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // 分配内存
    unsigned char* pixels = new unsigned char[width * height * 3];
    
    // 获取纹理数据
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    // 保存为图像文件
    stbi_write_png(filename, width, height, 3, pixels, width * 3);
    
    // 释放内存
    delete[] pixels;
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
    else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        dumpRes = true;
        mylog(LogLevel::I, "dump frame to file");
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        fboEnable = true;
        mylog(LogLevel::I, "FBO enabled");
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        fboEnable = false;
        mylog(LogLevel::I, "FBO disabled");
    }

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
    glfwSetCursorPosCallback(wnd, mouse_callback);
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

    // // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    return wnd;
}