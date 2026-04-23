cc_library(
    name = "tinygltf",
    srcs = ["tinygltf_impl.cc"],
    hdrs = [
        "tiny_gltf.h",
        "json.hpp",
    ],
    includes = ["."],
    defines = [
        "TINYGLTF_NO_STB_IMAGE",
        "TINYGLTF_NO_STB_IMAGE_WRITE",
    ],
    visibility = ["//visibility:public"],
)
