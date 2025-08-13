cc_library(
    name = "render_core",
    srcs = [
        "AnimationHandler.cpp",
        "BufferObjectData.cpp",
        "DataManager.cpp",
        "ModelLoader.cpp",
        "Skybox.cpp",
        "VehicleMeshInfo.cpp",
        "VehicleRenderer.cpp",
        "VehicleShader.cpp",
        "VehicleVirCamera.cpp",
        "FboHandler.cpp"
    ],
    hdrs = [
        "AnimationHandler.h",
        "BufferObjectData.h",
        "DataManager.h",
        "ModelLoader.h",
        "Skybox.h",
        "VehicleMeshInfo.h",
        "VehicleRenderer.h",
        "VehicleShader.h",
        "VehicleVirCamera.h",
        "CommonDataStruct.h",
        "FboHandler.h"
    ],
    deps = [
        "//include:log",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
        "//dependency/assimp:assimp",
        "//dependency/glad:glad",
        "//src:configParser"
    ],
    linkopts = [
        "-lglfw",
        "-lGL",
    ],
    visibility = [
        "//visibility:public",
    ],
)

cc_binary(
    name = "librender_core.so.1",
    deps = [
        ":render_core",
    ],
    linkshared = True,
)