#include "host/platform/compressed_texture.hpp"

#include <dlfcn.h>

#include <string>
#include <vector>

namespace {
    typedef void (*HostCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
    typedef const GLubyte *(*HostGlGetString)(GLenum);

    HostGlGetString host_real_gl_get_string() {
        static HostGlGetString host_real = reinterpret_cast<HostGlGetString>(dlsym(RTLD_NEXT, "glGetString"));
        return host_real;
    }

    HostCompressedTexImage2D host_real_compressed_tex_image_2d() {
        static HostCompressedTexImage2D host_real =
            reinterpret_cast<HostCompressedTexImage2D>(dlsym(RTLD_NEXT, "glCompressedTexImage2D"));
        return host_real;
    }
} // namespace

extern "C" const GLubyte *glGetString(GLenum name) {
    HostGlGetString host_real = host_real_gl_get_string();
    const GLubyte *value = host_real != nullptr ? host_real(name) : nullptr;
    if (name != GL_EXTENSIONS || value == nullptr) {
        return value;
    }

    static std::string host_extensions;
    host_extensions.assign(reinterpret_cast<const char *>(value));
    if (host_extensions.find("GL_OES_compressed_ETC1_RGB8_texture") == std::string::npos) {
        host_extensions.append(" GL_OES_compressed_ETC1_RGB8_texture");
    }
    return reinterpret_cast<const GLubyte *>(host_extensions.c_str());
}

extern "C" void glCompressedTexImage2D(GLenum target, GLint level, GLenum internal_format, GLsizei width,
                                       GLsizei height, GLint border, GLsizei image_size, const void *data) {
    std::vector<u8> rgba;
    if (HostDecodeCompressedTexture(internal_format, width, height, image_size, data, rgba)) {
        glTexImage2D(target, level, GL_RGBA, width, height, border, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
        return;
    }

    // These formats are advertised by the host shim and must never reach a
    // native driver when validation or decoding failed.
    if (internal_format == 0x8d64 || internal_format == 0x8c00 || internal_format == 0x8c02) {
        return;
    }

    HostCompressedTexImage2D host_real = host_real_compressed_tex_image_2d();
    if (host_real != nullptr) {
        host_real(target, level, internal_format, width, height, border, image_size, data);
    }
}
