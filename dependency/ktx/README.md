# KTX Library Integration

This directory contains the KTX (Khronos Texture) library for loading and manipulating texture files.

## Structure

```
dependency/ktx/
├── BUILD           # Bazel build configuration
├── README.md       # This file
├── include/        # Header files
│   ├── ktx.h       # Main KTX API header
│   ├── ktxvulkan.h # Vulkan-specific definitions
│   └── KHR/
│       └── khr_df.h # Khronos Data Format header
└── lib/            # Shared libraries
    ├── libktx.so   # Symlink to versioned library
    ├── libktx.so.4 # Symlink to specific version
    └── libktx.so.4.4.2 # Actual library file
```

## Bazel Integration

The BUILD file defines a `cc_library` target that can be used in other Bazel targets:

```python
cc_library(
    name = "ktx",
    srcs = ["lib/libktx.so.4.4.2", "lib/libktx.so.4", "lib/libktx.so"],
    hdrs = ["include/ktx.h", "include/ktxvulkan.h", "include/KHR/khr_df.h"],
    includes = ["include"],
    visibility = ["//visibility:public"]
)
```

## Usage Example

To use the KTX library in your Bazel target:

```python
cc_binary(
    name = "your_app",
    srcs = ["your_app.cpp"],
    deps = [
        "//dependency/ktx:ktx",
    ],
)
```

In your C++ code:

```cpp
#include <ktx.h>

// Load a KTX texture from file
ktxTexture* texture = nullptr;
KTX_error_code result = ktxTexture_CreateFromNamedFile(
    "path/to/texture.ktx2",
    KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
    &texture
);

if (result == KTX_SUCCESS) {
    // Use the texture...
    // Upload to OpenGL: ktxTexture_GLUpload(texture, &textureId, ...);
    
    // Clean up
    ktxTexture_Destroy(texture);
}
```

## Testing

A test program is available at `test/ktxTest.cpp`. To build and run it:

```bash
bazel build //test:ktxTest
./bazel-bin/test/ktxTest
```

## Key Features

- **KTX 1.0 and 2.0 Support**: Load both legacy and modern KTX formats
- **Supercompression**: Support for Basis LZ, Zstd, and Zlib compression
- **Multiple Backends**: OpenGL and Vulkan texture upload support
- **Error Handling**: Comprehensive error codes with human-readable strings
- **Mipmap Generation**: Automatic mipmap generation capabilities

## Common APIs

### Loading Textures
- `ktxTexture_CreateFromNamedFile()` - Load from file path
- `ktxTexture_CreateFromMemory()` - Load from memory buffer
- `ktxTexture_CreateFromStdioStream()` - Load from FILE* stream

### Creating Textures
- `ktxTexture2_Create()` - Create a new KTX2 texture

### GPU Upload
- `ktxTexture_GLUpload()` - Upload texture to OpenGL
- `ktxTexture_VkUploadEx()` - Upload texture to Vulkan

### Cleanup
- `ktxTexture_Destroy()` - Release texture resources

## Version

Current library version: **4.4.2**

## License

The KTX library is provided by the Khronos Group under the Apache 2.0 license.
