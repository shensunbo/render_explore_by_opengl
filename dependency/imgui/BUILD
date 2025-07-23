cc_library(
    name = "imgui",
    srcs = [
        "imgui.cpp",
        "imgui_demo.cpp",
        "imgui_draw.cpp",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "backends/imgui_impl_glfw.cpp",
        "backends/imgui_impl_opengl3.cpp",
    ],
    hdrs = glob([
        "include/*.h",
        "include/*.hpp",
    ]),
    includes = ["include"],
    visibility = [
        "//visibility:public"
    ],
    # deps = [
    #     "//dependency/glad:glad",
    #     "//dependency/glm:glm",
    #     "//dependency/glfw:glfw",
    # ],
)