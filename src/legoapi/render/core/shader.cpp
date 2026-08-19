#include "legoapi/legoapi_types.h"

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

void ShaderMtlDescFilter::getVertexFlags() const {
}

void ShaderMtlDescFilter::hasDiffuseMap(i32) const {
}

void ShaderMtlDescFilter::hasLayer(i32) const {
}

void ShaderMtlDescFilter::internalInit(nushadermtldesc_s const *, numtl_s const *, i32, i32) {
}

extern "C" {

    void GetGLSLTypeInfo(void) {
    }

} // extern "C"
