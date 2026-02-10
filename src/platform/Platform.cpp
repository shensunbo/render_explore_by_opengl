#include "platform/Platform.h"

#ifndef __ANDROID__

#include <utility>

namespace platform {

Platform::Platform(int width, int height, const std::string &title) {
    if (!glfwInit()) {
        mylog(LogLevel::E, "Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        mylog(LogLevel::E, "Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, framebufferCb);
    glfwSetCursorPosCallback(window_, mouseCb);
    glfwSetScrollCallback(window_, scrollCb);
    glfwSetMouseButtonCallback(window_, mouseButtonCb);
    glfwSetKeyCallback(window_, keyCb);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        mylog(LogLevel::E, "Failed to initialize GLAD");
        glfwDestroyWindow(window_);
        window_ = nullptr;
        glfwTerminate();
        return;
    }

    glViewport(0, 0, width, height);
    lastX_ = static_cast<double>(width) * 0.5;
    lastY_ = static_cast<double>(height) * 0.5;
    firstMouse_ = true;
}

Platform::~Platform() {
    if (window_) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

bool Platform::shouldClose() const {
    return window_ ? glfwWindowShouldClose(window_) : true;
}

void Platform::requestClose() {
    if (window_) {
        glfwSetWindowShouldClose(window_, true);
    }
}

void Platform::pollEvents() {
    inputState_.resetDeltas();
    glfwPollEvents();
}

void Platform::swapBuffers() {
    if (window_) {
        glfwSwapBuffers(window_);
    }
}

double Platform::timeSeconds() const { return glfwGetTime(); }

Key Platform::mapKey(int key) {
    switch (key) {
    case GLFW_KEY_W: return Key::W;
    case GLFW_KEY_A: return Key::A;
    case GLFW_KEY_S: return Key::S;
    case GLFW_KEY_D: return Key::D;
    case GLFW_KEY_T: return Key::T;
    case GLFW_KEY_G: return Key::G;
    case GLFW_KEY_SPACE: return Key::Space;
    case GLFW_KEY_R: return Key::R;
    case GLFW_KEY_M: return Key::M;
    case GLFW_KEY_I: return Key::I;
    case GLFW_KEY_F: return Key::F;
    case GLFW_KEY_E: return Key::E;
    case GLFW_KEY_ESCAPE: return Key::Escape;
    default: return Key::Unknown;
    }
}

void Platform::framebufferCb(GLFWwindow *wnd, int width, int height) {
    (void)width; (void)height;
    glViewport(0, 0, width, height);
}

void Platform::mouseCb(GLFWwindow *wnd, double xpos, double ypos) {
    auto *self = static_cast<Platform *>(glfwGetWindowUserPointer(wnd));
    if (!self) return;

    if (self->firstMouse_) {
        self->lastX_ = xpos;
        self->lastY_ = ypos;
        self->firstMouse_ = false;
    }

    self->inputState_.deltaX += xpos - self->lastX_;
    self->inputState_.deltaY += self->lastY_ - ypos; // invert y
    self->lastX_ = xpos;
    self->lastY_ = ypos;
    self->inputState_.mouseX = xpos;
    self->inputState_.mouseY = ypos;
}

void Platform::scrollCb(GLFWwindow *wnd, double xoffset, double yoffset) {
    (void)xoffset;
    auto *self = static_cast<Platform *>(glfwGetWindowUserPointer(wnd));
    if (!self) return;
    self->inputState_.scrollY += yoffset;
}

void Platform::mouseButtonCb(GLFWwindow *wnd, int button, int action, int mods) {
    (void)mods;
    auto *self = static_cast<Platform *>(glfwGetWindowUserPointer(wnd));
    if (!self) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        self->inputState_.leftButton = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

void Platform::keyCb(GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    auto *self = static_cast<Platform *>(glfwGetWindowUserPointer(wnd));
    if (!self) return;

    Key k = mapKey(key);
    if (k == Key::Unknown) return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        self->inputState_.keyDown[k] = true;
    } else if (action == GLFW_RELEASE) {
        self->inputState_.keyDown[k] = false;
    }
}

} // namespace platform

#endif // __ANDROID__
