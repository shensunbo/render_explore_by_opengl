cc_library(
    name = "mesh",
    srcs = [
        "BufferObjectData.cpp",
        "ModelLoader.cpp",
        "MeshInfo.cpp",
    ],
    hdrs = [
        "BufferObjectData.h",
        "ModelLoader.h",
        "MeshInfo.h",
    ],
    includes = [".."],
    deps = [
        "//src/core/common:common",
        "//src/core/shader:shader",
        "//src/core/texture:texture",
        "//include:log",
        "//include:gl_headers",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
        "//dependency/tinygltf:tinygltf",
        "//dependency/ktx:ktx",
        "//src:configParser",
    ],
    visibility = ["//visibility:public"],
)
