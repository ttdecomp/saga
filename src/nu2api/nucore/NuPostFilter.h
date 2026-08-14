#pragma once

struct numtx_s;

struct NuPostFilter {
    void initSharedResources(i32, i32);
    void renderFrustum(numtx_s *);
};
