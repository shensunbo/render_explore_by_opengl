cc_library(
    name = "configParser",
    srcs = [
        "configParser/ConfigParser.cpp",
    ],
    hdrs = [
        "configParser/ConfigParser.h",
    ],
    includes = [
        ".",
    ],
    deps = [
        "//include:log",
        "//dependency/json:json",
        "//dependency/glm:glm",
    ],
    visibility = [
        "//visibility:public"
    ],
)

cc_library(
    name = "platform",
    srcs = [
        "platform/Platform.cpp",
    ],
    hdrs = [
        "platform/Platform.h",
    ],
    includes = [
        ".",
    ],
    deps = [
        "//include:log",
        "//include:gl_headers",
        "//dependency/glad:glad",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "rhi",
    hdrs = [
        "rhi/Rhi.h",
    ],
    includes = [
        ".",
    ],
    deps = [
        "//include:gl_headers",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_binary(
    name = "refactor_test",
    srcs = [
        "refactor_test.cpp",
    ],
    deps = [
        "//include:log",
        "//include:gl_headers",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
        "//dependency/imgui:imgui",
        "//src/core/renderer:render_core",
        "//src/core/camera:camera",
        "//src/core/mesh:mesh",
        "//src/core/shader:shader",
        "//src/core/scene:scene",
        ":configParser",
        ":platform",
        ":rhi",
    ],
    copts = [
        "-g",
    ],
)

cc_binary(
    name = "android_test",
    srcs = [
        "android_test.cpp",
    ],
    deps = [
        "//include:log",
        "//include:gl_headers",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
        "//src/core/renderer:render_core",
        ":configParser",
    ],
)