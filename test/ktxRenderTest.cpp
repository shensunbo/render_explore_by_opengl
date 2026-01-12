#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ktx.h>
#include <iostream>

// 顶点着色器
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// 片段着色器
const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 窗口大小改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 输入处理
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 检查着色器编译错误
void checkShaderCompile(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// 检查程序链接错误
void checkProgramLink(GLuint program) {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

// 从 KTX 文件加载纹理到 OpenGL
GLuint loadKTXTexture(const char* ktxPath) {
    std::cout << "Loading KTX texture: " << ktxPath << std::endl;
    
    ktxTexture* texture = nullptr;
    KTX_error_code result = ktxTexture_CreateFromNamedFile(ktxPath,
                                                            KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                            &texture);
    
    if (result != KTX_SUCCESS) {
        std::cerr << "Failed to load KTX file: " << ktxErrorString(result) << std::endl;
        return 0;
    }
    
    std::cout << "✓ KTX texture loaded successfully!" << std::endl;
    std::cout << "  Dimensions: " << texture->baseWidth << "x" << texture->baseHeight << std::endl;
    std::cout << "  Levels: " << texture->numLevels << std::endl;
    std::cout << "  Layers: " << texture->numLayers << std::endl;
    std::cout << "  Faces: " << texture->numFaces << std::endl;
    
    GLuint textureID;
    GLenum target, glerror;
    
    // 上传到 OpenGL
    result = ktxTexture_GLUpload(texture, &textureID, &target, &glerror);
    
    if (result != KTX_SUCCESS) {
        std::cerr << "Failed to upload texture to OpenGL: " << ktxErrorString(result) << std::endl;
        if (glerror != GL_NO_ERROR) {
            std::cerr << "OpenGL error: 0x" << std::hex << glerror << std::endl;
        }
        ktxTexture_Destroy(texture);
        return 0;
    }
    
    std::cout << "✓ Texture uploaded to OpenGL!" << std::endl;
    std::cout << "  Texture ID: " << textureID << std::endl;
    std::cout << "  Target: GL_TEXTURE_2D (0x" << std::hex << target << std::dec << ")" << std::endl;
    
    // 设置纹理参数
    glBindTexture(target, textureID);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(target, 0);
    
    ktxTexture_Destroy(texture);
    
    return textureID;
}

int main() {
    std::cout << "=== KTX Texture Rendering Demo ===" << std::endl;
    std::cout << "Rendering a textured quad to window using KTX library\n" << std::endl;
    
    // 1. 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
                                          "KTX Texture Demo - Press ESC to exit", 
                                          NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // 3. 加载 OpenGL 函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    std::cout << "✓ OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "✓ Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << std::endl;
    
    // 4. 加载 KTX 纹理
    const char* ktxPath = "res/model/APTIV.ktx";
    GLuint texture = loadKTXTexture(ktxPath);
    
    if (texture == 0) {
        std::cerr << "Failed to load KTX texture from: " << ktxPath << std::endl;
        glfwTerminate();
        return -1;
    }
    
    std::cout << std::endl;
    
    // 5. 创建着色器程序
    std::cout << "Creating shader program..." << std::endl;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompile(vertexShader, "VERTEX");
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompile(fragmentShader, "FRAGMENT");
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLink(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    std::cout << "✓ Shader program created" << std::endl;
    std::cout << std::endl;
    
    // 6. 设置顶点数据（正方形）
    float vertices[] = {
        // 位置           // 纹理坐标
         0.8f,  0.8f, 0.0f,   1.0f, 1.0f,   // 右上
         0.8f, -0.8f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.8f, -0.8f, 0.0f,   0.0f, 0.0f,   // 左下
        -0.8f,  0.8f, 0.0f,   0.0f, 1.0f    // 左上
    };
    
    unsigned int indices[] = {
        0, 1, 3,   // 第一个三角形
        1, 2, 3    // 第二个三角形
    };
    
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    std::cout << "✓ Vertex data configured" << std::endl;
    std::cout << "\n=== Rendering to screen... Press ESC to exit ===\n" << std::endl;
    
    // 7. 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);
        
        // 渲染
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 绘制纹理正方形
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        // 交换缓冲区并轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 8. 清理资源
    std::cout << "\nCleaning up resources..." << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);
    
    glfwTerminate();
    
    std::cout << "\n=== Demo completed! ===" << std::endl;
    
    return 0;
}
