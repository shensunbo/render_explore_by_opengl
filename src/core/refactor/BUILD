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