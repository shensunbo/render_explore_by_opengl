cc_binary(
    name = "eglTest",
    srcs = [
        "eglTest.cpp",
    ],
    deps = [
        "//include:log",
        "//dependency/glm:glm",
        # "//dependency/stb:stb",
        # "//src/core/refactor:render_core",
        # "//src:configParser",
    ],
    linkopts = [
        "-lEGL",
        "-lGLESv2",
    ],
)

cc_binary(
    name = "mesaTest",
    srcs = [
        "mesaTest.cpp",
    ],
    deps = [
        "//include:log",
        "//dependency/glm:glm",
        # "//dependency/glad:glad",
    ],
    linkopts = [
        # "-lglfw",
        # "-lGL",
        "-lOSMesa",
        "-lGL",
        "-lGLESv2",
    ],
)