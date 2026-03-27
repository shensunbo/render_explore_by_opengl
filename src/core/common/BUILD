cc_library(
    name = "common",
    srcs = [
        "tool.cpp",
        "AnimationHandler.cpp",
        "DataManager.cpp",
    ],
    hdrs = [
        "CommonDataStruct.h",
        "tool.h",
        "AnimationHandler.h",
        "DataManager.h",
    ],
    includes = [".."],
    deps = [
        "//include:log",
        "//include:gl_headers",
        "//dependency/glm:glm",
        "//dependency/stb:stb",
    ],
    visibility = ["//visibility:public"],
)
