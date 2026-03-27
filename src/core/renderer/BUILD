cc_library(
    name = "render_core",
    srcs = [
        "RenderPass.cpp",
        "Renderer.cpp",
    ],
    hdrs = [
        "RenderPass.h",
        "RenderGraph.h",
        "Renderer.h",
    ],
    includes = [".."],
    deps = [
        "//src/core/common:common",
        "//src/core/shader:shader",
        "//src/core/texture:texture",
        "//src/core/mesh:mesh",
        "//src/core/scene:scene",
        "//src/core/postprocess:postprocess",
        "//include:log",
        "//include:gl_headers",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
        "//dependency/glad:glad",
        "//src:configParser",
        "//src:rhi",
    ],
    linkopts = [
        "-lglfw",
        "-lEGL",
        "-lGLESv2",
    ],
    visibility = ["//visibility:public"],
)
