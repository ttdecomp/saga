#pragma once

#include <stdint.h>

#include "decomp.h"
#include "nu2api.saga/nucore/common.h"

C_API_START

extern int32_t COLLECTION_COMPLETIONCOUNT;

C_API_END

void Collection_Configure(char *file, VARIPTR *buf, VARIPTR *buf_end);
