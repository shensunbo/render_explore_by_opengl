#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

#include "gl/gl_headers.h"
#include "log/mylog.h"

#ifndef __ANDROID__

namespace platform {

enum class Key {
    W,
    A,
    S,
    D,
    T,
    G,
    Space,
    R,
    M,
    I,
    F,
    E,
    P,
    Escape,
    Unknown,
};

struct InputState {
    double mouseX{0.0};
    double mouseY{0.0};
    double deltaX{0.0};
    double deltaY{0.0};
    double scrollY{0.0};
    bool leftButton{false};
    std::unordered_map<Key, bool> keyDown;

    void resetDeltas() {
        deltaX = 0.0;
        deltaY = 0.0;
        scrollY = 0.0;
    }

    bool isDown(Key k) const {
        auto it = keyDown.find(k);
        return it != keyDown.end() && it->second;
    }
};

class Platform {
public:
    Platform(int width, int height, const std::string &title);
    ~Platform();

    bool valid() const { return window_ != nullptr; }
    bool shouldClose() const;
    void requestClose();
    void pollEvents();
    void swapBuffers();
    double timeSeconds() const;

    int width() const { return width_; }
    int height() const { return height_; }
    // Returns true if size changed since last poll; outputs new size.
    bool consumeResize(int &w, int &h);

    const InputState &input() const { return inputState_; }
    GLFWwindow *rawWindow() const { return window_; }

private:
    GLFWwindow *window_{nullptr};
    InputState inputState_{};
    bool firstMouse_{true};
    double lastX_{0.0};
    double lastY_{0.0};
    int width_{0};
    int height_{0};
    bool sizeDirty_{false};

    static Key mapKey(int key);
    static void framebufferCb(GLFWwindow *wnd, int width, int height);
    static void mouseCb(GLFWwindow *wnd, double xpos, double ypos);
    static void scrollCb(GLFWwindow *wnd, double xoffset, double yoffset);
    static void mouseButtonCb(GLFWwindow *wnd, int button, int action, int mods);
    static void keyCb(GLFWwindow *wnd, int key, int scancode, int action, int mods);
};

} // namespace platform

#else
#error "Platform wrapper is desktop-only (GLFW)."
#endif // __ANDROID__
