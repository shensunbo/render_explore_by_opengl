#pragma once

/**
 * @file gl_headers.h
 * @brief Centralized OpenGL-related header includes for cross-platform compilation
 * 
 * This header consolidates all OpenGL, GLAD, and GLFW includes in one place,
 * making it easy to manage dependencies for different platforms (desktop, Android, etc.)
 */

// ============================================================================
// Platform-specific OpenGL includes
// ============================================================================

#ifdef __ANDROID__
    // Android OpenGL ES headers
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
    
    // No GLFW on Android - native window handling instead
    #define NO_GLFW_SUPPORT
    
#else
    // Desktop OpenGL - use GLAD loader
    #include <glad/glad.h>
    
    // GLFW for window management
    #include <GLFW/glfw3.h>
#endif

// ============================================================================
// Common GL utilities (available on all platforms after including gl headers)
// ============================================================================

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>