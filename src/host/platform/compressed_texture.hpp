#pragma once

#include <GLES2/gl2.h>

#include <vector>

#include "nu2api/nucore/common.h"

// Shared CPU decoders used by native GL interposition and the WebGL upload
// override. Returns false for unsupported or malformed input.
bool HostDecodeCompressedTexture(GLenum internal_format, GLsizei width, GLsizei height, GLsizei image_size,
                                 const void *data, std::vector<u8> &rgba);
