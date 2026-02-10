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
        "//src/core/refactor:render_core",
        ":configParser",
        ":platform",
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
        "//src/core/refactor:render_core",
        ":configParser",
    ],
)