// Single translation unit that compiles the tinygltf implementation.
// All consumers include tiny_gltf.h without TINYGLTF_IMPLEMENTATION.
// TINYGLTF_NO_STB_IMAGE and TINYGLTF_NO_STB_IMAGE_WRITE are set via BUILD defines.
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"
