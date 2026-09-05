#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/numtl.h"

ShaderManagerOpenGL::ShaderManagerOpenGL(VirtualStackAllocator &) {
}

void ShaderManagerOpenGL::adaptShaderMaterialForShaderVersion(nushadermtldesc_s *) {
}

void ShaderManagerOpenGL::createShader(ShaderMtlDescFilter &, bool, NuShaderObject *, i32) {
}

void ShaderManagerOpenGL::createShader(ShaderObjectKey const &, NuShaderObject *, i32) {
}

void ShaderManagerOpenGL::setElementfv(SHADERSEMANTIC_enum, i32, float const *) {
}

void ShaderManagerOpenGL::setElementsfv(SHADERSEMANTIC_enum, i32, i32, float const *) {
}

void ShaderManagerOpenGL::setElementsfv_transpose(SHADERSEMANTIC_enum, i32, i32, float const *) {
}

void ShaderManagerOpenGL::setfv(SHADERSEMANTIC_enum, float const *) {
}

ShaderManagerOpenGL::~ShaderManagerOpenGL() {
}

u32 ShaderMtlDescFilter::getVertexFlags() const {
    const u8 *vertex_descriptor = reinterpret_cast<const u8 *>(&desc->vtx_desc);
    const u8 texture_flags = vertex_descriptor[1];
    const u8 attribute_flags = vertex_descriptor[2];
    const u8 extra_attribute_flags = vertex_descriptor[3];
    const bool extended = variant == 0 || desc->unknown_1b4 != 0;

    u32 flags = (attribute_flags >> 2) & 1;
    if ((attribute_flags & 3) != 0 && (attribute_flags & 0x40) == 0) {
        flags |= 2;
    }

    const u8 texture_count = (texture_flags >> 1) & 3;
    if (extended) {
        if (texture_count != 0) {
            flags |= 4;
        }
        if (texture_count >= 2) {
            flags |= 8;
        }
        if (texture_count == 3) {
            flags |= 0x10;
        }
    }
    if ((attribute_flags & 3) != 0 && (attribute_flags & 0x40) != 0) {
        flags |= 0x40;
    }
    if ((attribute_flags & 8) != 0) {
        flags |= 0x80;
    }
    if ((extra_attribute_flags & 1) != 0) {
        flags |= 0x100;
    }
    if ((extra_attribute_flags & 2) != 0) {
        flags |= 0x400;
    }
    if (extended) {
        if (static_cast<i8>(desc->flagsbits_1bb) < 0) {
            flags |= 0x800;
        }
        if ((texture_flags & 1) != 0) {
            flags |= 0x1000;
        }
    }
    return flags;
}

bool ShaderMtlDescFilter::hasDiffuseMap(i32 layer) const {
    if (variant != 0 && desc->unknown_1b4 == 0) {
        return false;
    }
    if (layer < 0 || layer >= 4) {
        return false;
    }
    return desc->diffuse_map_tex_id[layer] > texture_id_threshold;
}

bool ShaderMtlDescFilter::hasLayer(i32 layer) const {
    if (variant != 0) {
        return layer == 0 && (desc->unknown_1b4 & 1) != 0;
    }

    switch (layer) {
        case 0:
            return true;
        case 1:
            return (desc->flagsbits_1b8 & 0x40) != 0 && desc->blend_op2 != 0xff;
        case 2:
            return static_cast<i8>(desc->flagsbits_1b8) < 0 && desc->blend_op3 != 0xff;
        case 3:
            return (desc->byte4 & 1) != 0 && desc->blend_op4 != 0xff;
        default:
            return false;
    }
}

void ShaderMtlDescFilter::internalInit(nushadermtldesc_s const *material_desc, numtl_s const *material, i32 flags,
                                       i32 texture_threshold) {
    const u8 *material_bytes = reinterpret_cast<const u8 *>(material);
    desc = material_desc;
    mtl = material;
    flags_in = flags;
    variant = flags & 3;
    texture_id_threshold = texture_threshold;

    const u8 *vertex_descriptor = reinterpret_cast<const u8 *>(&desc->vtx_desc);
    const bool special_vertex_path = (vertex_descriptor[2] & 4) != 0 || (material_bytes[0x41] & 0x40) != 0;
    const bool base_variant = variant == 0;
    field_0x10 = base_variant && (flags & 0x10) != 0 && !special_vertex_path;
    field_0x18 = (variant != 0 || (flags & 0x10) != 0) && !special_vertex_path;
    field_0x14 = base_variant && (flags & 0x20) != 0 && !special_vertex_path;

    layer_count = 0;
    if (base_variant) {
        layer_count = field_0x10 == 0 && (desc->flagsbits_1ba & 1) == 0 ? 1 : 2;
    }
    if (field_0x18 != 0) {
        ++layer_count;
    }
    if (field_0x14 != 0) {
        ++layer_count;
    }
}

extern "C" {

    void GetGLSLTypeInfo(void) {
    }

} // extern "C"
