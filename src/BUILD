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