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

cc_binary(
    name = "ktxTest",
    srcs = [
        "ktxTest.cpp",
    ],
    deps = [
        "//dependency/ktx:ktx",
    ],
    linkopts = [
        "-lstdc++",
    ],
)
cc_binary(
    name = "ktxRenderTest",
    srcs = [
        "ktxRenderTest.cpp",
    ],
    deps = [
        "//dependency/ktx:ktx",
        "//dependency/glad:glad",
    ],
    linkopts = [
        "-lglfw",
        "-lGL",
        "-lstdc++",
    ],
)

cc_binary(
    name = "multithreadLoadTest",
    srcs = [
        "multithreadLoadTest.cpp",
    ],
    deps = [
        "//include:log",
        "//dependency/stb:stb",
    ],
    linkopts = [
        "-lstdc++",
    ],
)
