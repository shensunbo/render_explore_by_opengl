// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

#include <GL/osmesa.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "log/mylog.h"

// 顶点着色器源代码
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    out vec3 ourColor;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

// 片段着色器源代码
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 ourColor;
    out vec4 FragColor;
    
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)";

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 立方体顶点数据（位置和颜色）
float vertices[] = {
    // 位置              // 颜色
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
    
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f
};

// 立方体索引数据
unsigned int indices[] = {
    0, 1, 2, 2, 3, 0,    // 前
    4, 5, 6, 6, 7, 4,    // 后
    0, 4, 7, 7, 3, 0,    // 左
    1, 5, 6, 6, 2, 1,    // 右
    0, 1, 5, 5, 4, 0,    // 下
    3, 2, 6, 6, 7, 3     // 上
};

// 编译着色器函数
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    return shader;
}

// 创建着色器程序函数
unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

int main() {
    // // 初始化GLFW
    // if (!glfwInit()) {
    //     std::cerr << "Failed to initialize GLFW" << std::endl;
    //     return -1;
    // }
    
    // // 配置GLFW
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // // 创建窗口
    // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Cube", NULL, NULL);
    // if (!window) {
    //     std::cerr << "Failed to create GLFW window" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }
    // glfwMakeContextCurrent(window);
    
    // // 初始化GLAD
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     std::cerr << "Failed to initialize GLAD" << std::endl;
    //     return -1;
    // }
    
    // osmesa
    // 1. 初始化 EGL
    mylog(LogLevel::E,"---------> RUNNING_BAZEL_TEST");

    // 创建 OSMesa 上下文
    OSMesaContext ctx = OSMesaCreateContextExt(OSMESA_RGBA, 32, 0, 0, NULL);
    if (!ctx) {
        mylog(LogLevel::E,"Failed to create OSMesa context");
        return -1;
    }

    // 分配渲染缓冲区
    unsigned char *buffer = (unsigned char *)malloc(SCR_WIDTH * SCR_HEIGHT * 4 * sizeof(unsigned char));
    if (!buffer) {
        mylog(LogLevel::E,"Failed to allocate buffer");
        OSMesaDestroyContext(ctx);
        return -1;
    }

    // 绑定缓冲区到上下文
    if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, SCR_WIDTH, SCR_HEIGHT)) {
        mylog(LogLevel::E,"Failed to make OSMesa context current");
        free(buffer);
        OSMesaDestroyContext(ctx);
        return -1;
    }

    // 创建着色器程序
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    
    // 创建VAO, VBO和EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // 绑定VAO
    glBindVertexArray(VAO);
    
    // 绑定VBO并填充数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 绑定EBO并填充数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 解绑VBO和VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 渲染循环
    int frameCount = 0;
    // while (!glfwWindowShouldClose(window)) {
    while (frameCount < 30) {
        frameCount++;
        // 清空颜色缓冲和深度缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 使用着色器程序
        glUseProgram(shaderProgram);
        
        // 创建模型、视图和投影矩阵
        // float time = glfwGetTime();
        float time = 0.0f;
        
        // 模型矩阵（旋转）
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, time, glm::vec3(0.5f, 1.0f, 0.0f));
        
        // 视图矩阵（相机位置）
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        // 投影矩阵（透视）
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        // 传递矩阵到着色器
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // 绘制立方体
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // 交换缓冲区和轮询IO事件
        // glfwSwapBuffers(window);
        // glfwPollEvents();
    }
    
    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    // 终止GLFW
    // glfwTerminate();

    glFinish();  // 确保所有 GL 命令完成

    // 先解除当前上下文绑定
    OSMesaMakeCurrent(NULL, NULL, GL_UNSIGNED_BYTE, 0, 0);

    mylog(LogLevel::E,"---------> RUNNING_BAZEL_TEST");
    return 0;
}