#include "nu2api/nuandroid/ios_graphics.h"

#include "host/platform/graphics.hpp"

#include <stdio.h>

static char host_documents_path[256] = ".work/host-documents/";

void HostSetDocumentsPath(const char *path) {
    snprintf(host_documents_path, sizeof(host_documents_path), "%s", path);
}

char *NuIOS_GetDocumentsPath(void) {
    // Keep host utility saves isolated from extracted resources and user saves.
    return host_documents_path;
}
