#pragma once
#include <glad/glad.h>


// Function to get base format, data type, and attachment type from an internal format
void GetBaseFormatTypeAndAttachment(GLenum internalFormat, GLenum& baseFormat, GLenum& type, GLenum& attachment) {
    // Default unknown values
    baseFormat = 0;
    type = 0;
    attachment = 0;

    // Mapping internal formats to base format, type, and attachment type
    switch (internalFormat) {
        // Depth-only formats
    case GL_DEPTH_COMPONENT16: baseFormat = GL_DEPTH_COMPONENT; type = GL_UNSIGNED_SHORT; attachment = GL_DEPTH_ATTACHMENT; break;
    case GL_DEPTH_COMPONENT24: baseFormat = GL_DEPTH_COMPONENT; type = GL_FLOAT;   attachment = GL_DEPTH_ATTACHMENT; break;
    case GL_DEPTH_COMPONENT32: baseFormat = GL_DEPTH_COMPONENT; type = GL_UNSIGNED_INT;   attachment = GL_DEPTH_ATTACHMENT; break;

        // Stencil-only format
    case GL_STENCIL_INDEX8: baseFormat = GL_STENCIL_INDEX; type = GL_UNSIGNED_BYTE; attachment = GL_STENCIL_ATTACHMENT; break;

        // Depth-stencil formats
    case GL_DEPTH24_STENCIL8: baseFormat = GL_DEPTH_STENCIL; type = GL_UNSIGNED_INT_24_8; attachment = GL_DEPTH_STENCIL_ATTACHMENT; break;
    case GL_DEPTH32F_STENCIL8: baseFormat = GL_DEPTH_STENCIL; type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV; attachment = GL_DEPTH_STENCIL_ATTACHMENT; break;

        // === Single-channel formats ===
    case GL_R8:    baseFormat = GL_RED;  type = GL_UNSIGNED_BYTE;  attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_R16:   baseFormat = GL_RED;  type = GL_UNSIGNED_SHORT; attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_R16F:  baseFormat = GL_RED;  type = GL_FLOAT;          attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_R32F:  baseFormat = GL_RED;  type = GL_FLOAT;          attachment = GL_COLOR_ATTACHMENT0; break;

        // === Two-channel formats ===
    case GL_RG8:   baseFormat = GL_RG;   type = GL_UNSIGNED_BYTE;  attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RG16:  baseFormat = GL_RG;   type = GL_UNSIGNED_SHORT; attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RG16F: baseFormat = GL_RG;   type = GL_HALF_FLOAT;     attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RG32F: baseFormat = GL_RG;   type = GL_FLOAT;          attachment = GL_COLOR_ATTACHMENT0; break;

        // === Three-channel (RGB) formats ===
    case GL_RGB8:   baseFormat = GL_RGB;  type = GL_UNSIGNED_BYTE;  attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGB16:  baseFormat = GL_RGB;  type = GL_UNSIGNED_SHORT; attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGB16F: baseFormat = GL_RGB;  type = GL_HALF_FLOAT;     attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGB32F: baseFormat = GL_RGB;  type = GL_FLOAT;          attachment = GL_COLOR_ATTACHMENT0; break;

        // === Four-channel (RGBA) formats ===
    case GL_RGBA8:   baseFormat = GL_RGBA; type = GL_UNSIGNED_BYTE;  attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGBA16:  baseFormat = GL_RGBA; type = GL_UNSIGNED_SHORT; attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGBA16F: baseFormat = GL_RGBA; type = GL_HALF_FLOAT;     attachment = GL_COLOR_ATTACHMENT0; break;
    case GL_RGBA32F: baseFormat = GL_RGBA; type = GL_FLOAT;          attachment = GL_COLOR_ATTACHMENT0; break;

    default:
        baseFormat = 0; // Unknown format
        type = 0;
        attachment = 0;
        break;
    }
}
