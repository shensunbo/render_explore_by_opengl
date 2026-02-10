cc_library(
    name = "imgui",
    srcs = [
        "src/imgui.cpp",
        "src/imgui_demo.cpp",
        "src/imgui_draw.cpp",
        "src/imgui_tables.cpp",
        "src/imgui_widgets.cpp",
        "src/imgui_impl_glfw.cpp",
        "src/imgui_impl_opengl3.cpp",
    ],
    hdrs = glob([
        "include/*.h",
    ]),
    includes = [
        "include",
    ],
    copts = ["-DIMGUI_IMPL_OPENGL_LOADER_GLAD"],
    visibility = [
        "//visibility:public"
    ],
    # deps = [
    #     "//dependency/glad:glad",
    #     "//dependency/glm:glm",
    #     "//dependency/glfw:glfw",
    # ],
)