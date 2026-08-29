// Transcription of NuGetVertexDeclaration from LEGO Star Wars TCS Android x86
// res/libTTapp.so (nm vaddr 0x2fc9a0, size 0xd91; Ghidra address 0x30c9a0).
//
// Original file-static data referenced:
//   g_vertexFormatPool          nm vaddr 0x11b90a0, size 0x14000 (256 records * 0x140)
//   g_allocatedDescriptorCount  nm vaddr 0x11cd0a0
//
// One pool record is 0x140 bytes: u32 key (the NUVERTEXDESCRIPTOR bitfield) at +0,
// followed by the 0x13c-byte NuVertexFormatPS attribute array at +4. The function
// returns a pointer to that array (record + 4).
//
// Attribute locations (fixed slots in the declaration):
//   0 position           always GL_FLOAT x3 @0
//   1 diffuse colour     GL_UNSIGNED_BYTE x4 normalized   (desc bit 8)
//   2 specular colour    GL_UNSIGNED_BYTE x4 normalized   (desc bits 9|10)
//   3 normal             GL_FLOAT x3 or packed UBYTE x4   (desc bits 2/3/19/23)
//   4 tangent            GL_FLOAT x3 or packed UBYTE x4   (desc bits 4/5/24)
//   5 binormal           GL_FLOAT x3 or packed UBYTE x4   (desc bits 6/7/25)
//   6/7 texcoords        float/half-float sets per desc bits 11-13 / 27
//   8+9 paired UBYTE x4  normalized                       (desc bit 26)
//   10                   GL_UNSIGNED_BYTE x4 norm or GL_FLOAT x2 (desc bits 14/15)
//   11                   GL_UNSIGNED_BYTE x4 or GL_FLOAT x3      (desc bits 16/17)
//   12 duplicate position GL_FLOAT x3 @0, stride 12       (desc bit 22)

#include <string.h>

#include "nu2api/nu3d/android/nuvertexformat_android.h"

#include "nu2api/nu3d/numtl.h"

// g_vertexFormatPool: original static bss 0x11b90a0 (_ZL18g_vertexFormatPool),
// 256 entries of 0x140 bytes each.
static NuVertexFormatPoolEntryPS g_vertexFormatPool[256];

// g_allocatedDescriptorCount: original static bss 0x11cd0a0
// (_ZL26g_allocatedDescriptorCount).
static i32 g_allocatedDescriptorCount;

static_assert(sizeof(NuVertDeclAttribPS) == 0x18, "attrib record must be 6 dwords");
static_assert(sizeof(NuVertexFormatPS) == 0x13c, "format array must be mask + 13 attrib records");
static_assert(sizeof(NuVertexFormatPoolEntryPS) == 0x140, "pool record stride must be 0x140");

NuVertexFormatPS *NuGetVertexDeclaration(NUVERTEXDESCRIPTOR vtx_desc) {
    const u32 desc = vtx_desc.flags;
    const i32 count = g_allocatedDescriptorCount;

    // 0x2fc9bb-0x2fca1e: linear scan of the pool for a record whose key matches
    // the descriptor; return the existing attribute array on hit.
    for (i32 i = 0; i < count; ++i) {
        if (memcmp(&g_vertexFormatPool[i].key, &desc, sizeof(u32)) == 0) {
            return &g_vertexFormatPool[i].format;
        }
    }

    // 0x2fca20-0x2fca2d: pool exhausted.
    if ((u32)count > 0xff) {
        return NULL;
    }

    NuVertexFormatPoolEntryPS *const rec = &g_vertexFormatPool[count];
    NuVertexFormatPS *const fmt = &rec->format;

    // 0x2fca57-0x2fca85: re-assemble the descriptor with its tex-coord mode field
    // (bits 11-13) made explicit, and extract per-attribute selectors. The stored
    // pool key is this normalized value (numerically equal to desc).
    const u32 tex_mode = ((desc >> 11) & 7);                        // 0x2fca5a-0x2fca6d
    const u32 desc_norm = (desc & ~(u32)0x3800) | (tex_mode << 11); // 0x2fca78-0x2fca80

    u32 sel_normal = 2;                    // esp+0x84 (0x2fca48)
    if ((desc_norm & 0x880008) == 0) {     // 0x2fca85
        sel_normal = (desc_norm >> 2) & 1; // 0x2fca8d-0x2fca99
    }
    u32 sel_tangent = 2;                    // esp+0x70
    if ((desc_norm & 0x1000020) == 0) {     // 0x2fcaa0
        sel_tangent = (desc_norm >> 4) & 1; // 0x2fcab0-0x2fcabc
    }
    u32 sel_binormal = 2;                    // esp+0x6c
    if ((desc_norm & 0x2000080) == 0) {      // 0x2fcac0
        sel_binormal = (desc_norm >> 6) & 1; // 0x2fcad0-0x2fcadc
    }

    const u32 desc_byte1 = (desc_norm >> 8) & 0xff; // 0x2fcae0
    const u32 has_diffuse = desc_byte1 & 1;         // byte @ esp+0x3f, bit 8
    const u32 spec_bits = desc_byte1 & 6;           // byte @ esp+0x3e, bits 9|10

    const u32 byte3_bit26 = ((desc_norm >> 24) & 4); // byte @ esp+0x80, bit 26 (0x2fcaf5/0x2fcb66-0x2fcb73)

    // 0x2fcb01-0x2fcb13 / 0x2fd0e8: UV path selection. Bit 27 (has_half_uvs)
    // switches to half-float texcoord emission driven by esp+0x60 and disables the
    // float switch selector at esp+0x4c.
    u32 sw_tex_units; // esp+0x4c: float-UV switch selector
    u32 half_units;   // esp+0x60: half-float-UV selector
    if ((desc_norm & 0x08000000) != 0) {
        half_units = tex_mode; // 0x2fd0e8
        sw_tex_units = 0;      // 0x2fd0ec
    } else {
        sw_tex_units = tex_mode; // 0x2fcb0f
        half_units = 0;          // 0x2fcb13
    }

    // 0x2fcb1b-0x2fcb31: location 10 selector; default 2 (absent) while bit 15 set.
    u32 sel_loc10 = 2;
    if ((i8)desc_byte1 >= 0) {
        sel_loc10 = (desc_byte1 >> 6) & 1; // 0x2fcb29-0x2fcb31 (bit 14)
    }

    // 0x2fcb35-0x2fcb4a: location 11 selector; default 2 (absent) while bit 17 set.
    const u32 desc_byte2 = (desc_norm >> 16) & 0xff; // 0x2fcb35
    u32 sel_loc11 = 2;
    if ((desc_byte2 & 2) == 0) {
        sel_loc11 = desc_byte2 & 1; // bit 16
    }

    const u32 has_extra_pos = (desc_byte2 >> 6) & 1; // esp+0x38, bit 22 (0x2fcb4e/0x2fcb55-0x2fcb66)

    // 0x2fcb51-0x2fcb9c: position attribute (location 0), always present.
    u32 mask = 0;
    mask |= 0x1;                    // 0x2fcb86/0x2fcb9c
    fmt->attribs[0].normalized = 0; // 0x2fcb51 (+0xc)
    fmt->attribs[0].unknown_0c = 0; // 0x2fcb57 (+0x10)
    fmt->attribs[0].offset = 0;     // 0x2fcb6c (+0x14)
    fmt->attribs[0].type = 0x1406;  // 0x2fcb7f (+0x4) GL_FLOAT
    fmt->attribs[0].size = 3;       // 0x2fcb91 (+0x8)

    // ------------------------------------------------------------------
    // Location 3 (normal) dispatch + candidate-offset table init.
    //
    // The remaining attributes are emitted in code order tangent, binormal,
    // diffuse, specular, texcoords, location 10, location 11, the 8+9 pair and
    // location 12. Every layout combination of the attributes that may precede a
    // given one has its own precomputed byte offset held in one of the stack
    // slots mirrored below (c18..c84); emitting an attribute rotates those
    // candidates so downstream slots account for its size. ecx_c/edx_c/esi_c are
    // the registers carrying the current cursor snapshots between blocks.
    // ------------------------------------------------------------------

    u32 c18, c1c, c20, c24, c28, c2c, c30, c34, c3c, c40, c44, c48, c4a, c50, c54, c58, c5c, c78, c7c, c84;
    i32 ecx_c = 0, edx_c = 0, esi_c = 0;

    if (sel_normal == 1) {              // 0x2fd5b0 (float normal)
        mask |= 0x9;                    // or edx,9
        fmt->attribs[3].unknown_0c = 0; // 0x2fd5b3 (+0x58)
        fmt->attribs[3].offset = 0xc;   // 0x2fd5ba (+0x5c)
        fmt->attribs[3].type = 0x1406;  // 0x2fd5c1 GL_FLOAT
        fmt->attribs[3].size = 3;       // 0x2fd5c8
        fmt->attribs[3].normalized = 0; // 0x2fd5cf (+0x54)
        c1c = 0x34;
        c20 = 0x34;
        c34 = 0x34; // 0x2fd5d3-0x2fd5e6
        c24 = 0x30;
        c28 = 0x30; // 0x2fd5ed-0x2fd5f5
        c48 = 0x28; // 0x2fd5fd
        c2c = 0x2c;
        c30 = 0x2c;   // 0x2fd604-0x2fd60c
        esi_c = 0x2c; // 0x2fd614
        c50 = 0x28;
        c54 = 0x28; // 0x2fd619-0x2fd621
        c58 = 0x24;
        c5c = 0x24; // 0x2fd629-0x2fd631
        c40 = 0x20;
        c44 = 0x20;
        c4a = 0x20; // 0x2fd639-0x2fd649
        c78 = 0x1c;
        c7c = 0x1c;
        c84 = 0x1c; // 0x2fd650-0x2fd660
        c3c = 0x24; // 0x2fd66a
        c18 = 0x18; // 0x2fd671
        ecx_c = 0x18;
        edx_c = 0x18;             // 0x2fd679-0x2fd67e
    } else if (sel_normal == 2) { // 0x2fd4d1 (packed normal)
        mask |= 0x9;
        fmt->attribs[3].unknown_0c = 0; // 0x2fd4d4
        fmt->attribs[3].offset = 0xc;   // 0x2fd4db
        fmt->attribs[3].type = 0x1401;  // 0x2fd4e2 GL_UNSIGNED_BYTE
        fmt->attribs[3].size = 4;       // 0x2fd4e9
        fmt->attribs[3].normalized = 1; // 0x2fd4f0
        c1c = 0x2c;
        c20 = 0x2c;
        c34 = 0x2c; // 0x2fd4f4-0x2fd507
        c24 = 0x28;
        c28 = 0x28; // 0x2fd50e-0x2fd516
        c48 = 0x20; // 0x2fd51e
        c2c = 0x24;
        c30 = 0x24;   // 0x2fd525-0x2fd52d
        esi_c = 0x24; // 0x2fd535
        c50 = 0x20;
        c54 = 0x20; // 0x2fd53a-0x2fd542
        c58 = 0x1c;
        c5c = 0x1c; // 0x2fd54a-0x2fd552
        c40 = 0x18;
        c44 = 0x18;
        c4a = 0x18; // 0x2fd55a-0x2fd56a
        c78 = 0x14;
        c7c = 0x14;
        c84 = 0x14; // 0x2fd571-0x2fd581
        c3c = 0x1c; // 0x2fd58b
        c18 = 0x10; // 0x2fd592
        ecx_c = 0x10;
        edx_c = 0x10; // 0x2fd59a-0x2fd59f
    } else {          // 0x2fcbb3 (no normal)
        c1c = 0x28;
        c20 = 0x28;
        c34 = 0x28; // 0x2fcbb3-0x2fcbc3
        c24 = 0x24;
        c28 = 0x24; // 0x2fcbca-0x2fcbd2
        c48 = 0x1c; // 0x2fcbda
        c2c = 0x20;
        c30 = 0x20;   // 0x2fcbe1-0x2fcbe9
        esi_c = 0x20; // 0x2fcbf1
        c50 = 0x1c;
        c54 = 0x1c; // 0x2fcbf6-0x2fcbfe
        c58 = 0x18;
        c5c = 0x18; // 0x2fcc06-0x2fcc0e
        c40 = 0x14;
        c44 = 0x14;
        c4a = 0x14; // 0x2fcc16-0x2fcc26
        c78 = 0x10;
        c7c = 0x10;
        c84 = 0x10; // 0x2fcc2d-0x2fcc3d
        c3c = 0x18; // 0x2fcc47
        c18 = 0xc;  // 0x2fcc4e
        ecx_c = 0xc;
        edx_c = 0xc; // 0x2fcc56-0x2fcc5b
    }

    // 0x2fcc60: location 4 (tangent). 1 -> float x3 (0x2fd440), 2 -> packed
    // UBYTE x4 normalized inline, 0 -> absent.
    if (sel_tangent == 1) { // 0x2fd440
        edx_c = c30;
        fmt->attribs[4].offset = (u32)ecx_c; // 0x2fd444 (+0x74)
        ecx_c = c2c;                         // 0x2fd447
        c44 = edx_c;                         // 0x2fd44b
        c40 = ecx_c;                         // 0x2fd44f
        edx_c = c54;                         // 0x2fd453
        ecx_c = c50;                         // 0x2fd457
        c4a = (u32)(esi_c & 0xffff);         // 0x2fd45b
        esi_c = c1c;                         // 0x2fd460
        c78 = ecx_c;                         // 0x2fd464
        c50 = esi_c;                         // 0x2fd468
        ecx_c = c20;                         // 0x2fd46c
        esi_c = c48 & 0xffff;                // 0x2fd470
        c7c = edx_c;                         // 0x2fd475
        edx_c = c34 & 0xffff;                // 0x2fd479
        c54 = ecx_c;                         // 0x2fd47e
        c84 = esi_c;                         // 0x2fd482
        ecx_c = c58;                         // 0x2fd48a
        c48 = edx_c & 0xffff;                // 0x2fd48e
        esi_c = c24;                         // 0x2fd493
        edx_c = c28;                         // 0x2fd497
        c18 = ecx_c;                         // 0x2fd49b
        fmt->attribs[4].unknown_0c = 0;      // 0x2fd49f (+0x70)
        fmt->attribs[4].type = 0x1406;       // 0x2fd4aa GL_FLOAT
        fmt->attribs[4].size = 3;            // 0x2fd4b5
        fmt->attribs[4].normalized = 0;      // 0x2fd4bc
        mask |= 0x10;                        // 0x2fd4c0
        c5c = edx_c;                         // 0x2fd4b1
        c58 = esi_c;                         // 0x2fd4c3
        edx_c = c3c & 0xffff;                // 0x2fd4c7
    } else if (sel_tangent == 2) {           // 0x2fcc76 packed tangent
        c48 = (u32)(esi_c & 0xffff);         // 0x2fcc76
        esi_c = c40;                         // 0x2fcc7b
        fmt->attribs[4].offset = (u32)ecx_c; // 0x2fcc7f
        ecx_c = c78;                         // 0x2fcc82
        c78 = esi_c;                         // 0x2fcc86
        esi_c = c2c;                         // 0x2fcc8a
        edx_c = c58;                         // 0x2fcc8e
        c18 = ecx_c;                         // 0x2fcc92
        ecx_c = c50;                         // 0x2fcc96
        c50 = esi_c;                         // 0x2fcc9a
        esi_c = c5c;                         // 0x2fcc9e
        c40 = edx_c;                         // 0x2fcca2
        edx_c = c44;                         // 0x2fcca6
        c44 = esi_c;                         // 0x2fccaa
        esi_c = c30;                         // 0x2fccae
        c58 = ecx_c;                         // 0x2fccb2
        ecx_c = c7c;                         // 0x2fccb6 (binormal cursor snapshot)
        c7c = edx_c;                         // 0x2fccba
        edx_c = c54;                         // 0x2fccbe
        c54 = esi_c;                         // 0x2fccc2
        esi_c = c4a & 0xffff;                // 0x2fccc6
        c5c = edx_c;                         // 0x2fcccb
        edx_c = c84 & 0xffff;                // 0x2fcccf
        c84 = esi_c;                         // 0x2fccd7
        esi_c = c3c & 0xffff;                // 0x2fccdf
        fmt->attribs[4].unknown_0c = 0;      // 0x2fcce4 (+0x70)
        fmt->attribs[4].type = 0x1401;       // 0x2fcceb GL_UNSIGNED_BYTE
        fmt->attribs[4].size = 4;            // 0x2fccf2
        fmt->attribs[4].normalized = 1;      // 0x2fccf9
        mask |= 0x10;                        // 0x2fccfd
        c4a = esi_c;                         // 0x2fcd00
    }
    // else sel_tangent == 0: absent (0x2fcc70 jumps past).

    // 0x2fcd05: location 5 (binormal).
    if (sel_binormal == 1) {                 // 0x2fd3d8 float x3
        esi_c = c54;                         // 0x2fd3d8
        fmt->attribs[5].offset = (u32)ecx_c; // 0x2fd3dc (+0x8c)
        ecx_c = c50;                         // 0x2fd3e2
        edx_c = c18 & 0xffff;                // 0x2fd3e6
        c78 = ecx_c;                         // 0x2fd3eb
        c7c = esi_c;                         // 0x2fd3ef
        ecx_c = c48 & 0xffff;                // 0x2fd3f3
        esi_c = c58;                         // 0x2fd3f8
        c84 = (u32)(ecx_c & 0xffff);         // 0x2fd3fc
        fmt->attribs[5].unknown_0c = 0;      // 0x2fd404 (+0x88)
        fmt->attribs[5].type = 0x1406;       // 0x2fd40e GL_FLOAT
        fmt->attribs[5].size = 3;            // 0x2fd415
        fmt->attribs[5].normalized = 0;      // 0x2fd41f
        mask |= 0x20;                        // 0x2fd426
        edx_c += 0xc;                        // 0x2fd429
        c18 = esi_c;                         // 0x2fd42c
        ecx_c = c5c;                         // 0x2fd430
    } else if (sel_binormal == 2) {          // 0x2fcd17 packed UBYTE x4
        esi_c = c44;                         // 0x2fcd17
        fmt->attribs[5].offset = (u32)ecx_c; // 0x2fcd1b
        ecx_c = c40;                         // 0x2fcd21
        edx_c = c78;                         // 0x2fcd25
        c78 = ecx_c;                         // 0x2fcd29
        ecx_c = c7c;                         // 0x2fcd2d (diffuse cursor snapshot)
        c7c = esi_c;                         // 0x2fcd31
        esi_c = c4a & 0xffff;                // 0x2fcd35
        c18 = edx_c;                         // 0x2fcd3a
        fmt->attribs[5].unknown_0c = 0;      // 0x2fcd3e (+0x88)
        edx_c = c84 & 0xffff;                // 0x2fcd48
        fmt->attribs[5].type = 0x1401;       // 0x2fcd50 GL_UNSIGNED_BYTE
        fmt->attribs[5].size = 4;            // 0x2fcd57
        fmt->attribs[5].normalized = 1;      // 0x2fcd61
        mask |= 0x20;                        // 0x2fcd68
        c84 = esi_c;                         // 0x2fcd6b
    }
    // else sel_binormal == 0: absent (0x2fcd15 jumps past).

    // 0x2fcd73: location 1 (diffuse colour), desc bit 8.
    if (has_diffuse != 0) {
        edx_c = c78;                         // 0x2fcd7a
        fmt->attribs[1].offset = (u32)ecx_c; // 0x2fcd7e (+0x2c)
        c18 = edx_c;                         // 0x2fcd81
        fmt->attribs[1].unknown_0c = 0;      // 0x2fcd85 (+0x28)
        fmt->attribs[1].type = 0x1401;       // 0x2fcd8c GL_UNSIGNED_BYTE
        fmt->attribs[1].size = 4;            // 0x2fcd93
        fmt->attribs[1].normalized = 1;      // 0x2fcd9a (+0x24)
        mask |= 0x2;                         // 0x2fcd9e
        ecx_c = c7c;                         // 0x2fcda1 (specular cursor snapshot)
        edx_c = c84 & 0xffff;                // 0x2fcda5
    }

    // 0x2fcdad: location 2 (specular colour), enabled by desc bits 9|10.
    if (spec_bits != 0) {
        edx_c = c18 & 0xffff;                // 0x2fcdb4
        fmt->attribs[2].unknown_0c = 0;      // 0x2fcdb9 (+0x40)
        fmt->attribs[2].offset = (u32)ecx_c; // 0x2fcdc0 (+0x44)
        fmt->attribs[2].type = 0x1401;       // 0x2fcdc3 GL_UNSIGNED_BYTE
        fmt->attribs[2].size = 4;            // 0x2fcdca
        fmt->attribs[2].normalized = 1;      // 0x2fcdd1 (+0x3c)
        mask |= 0x4;                         // 0x2fcdd5
        edx_c += 4;                          // 0x2fcdd8
    }

    // 0x2fcddb: float texcoord emission, switched on tex_coord_mode (bits 11-13);
    // jump table at nm vaddr 0x57dcac, cases 0..5 (case 0 empty).
    switch (sw_tex_units) {
        case 1:                                  // 0x2fcdf1: vec2
            edx_c = (i32)(i16)(edx_c & 0xffff);  // movsx edx, dx
            fmt->attribs[6].unknown_0c = 0;      // (+0xa0)
            fmt->attribs[6].offset = (u32)edx_c; // (+0xa4)
            fmt->attribs[6].type = 0x1406;       // (+0x94) GL_FLOAT
            fmt->attribs[6].size = 2;            // (+0x98)
            fmt->attribs[6].normalized = 0;      // (+0x9c)
            mask |= 0x40;                        // 0x2fce1f
            edx_c += 8;                          // 0x2fce22
            break;                               // falls through to 0x2fce30
        case 2:                                  // 0x2fd2d3: vec4
            edx_c = (i32)(i16)(edx_c & 0xffff);
            fmt->attribs[6].unknown_0c = 0;
            fmt->attribs[6].offset = (u32)edx_c;
            fmt->attribs[6].type = 0x1406;
            fmt->attribs[6].size = 4;
            fmt->attribs[6].normalized = 0;
            mask |= 0x40;  // 0x2fd301
            edx_c += 0x10; // 0x2fd304
            break;
        case 3:                             // 0x2fd263: vec4 + vec2
            fmt->attribs[6].normalized = 0; // 0x2fd263 (+0x9c)
            edx_c = (i32)(i16)(edx_c & 0xffff);
            fmt->attribs[6].unknown_0c = 0;               // 0x2fd27f
            fmt->attribs[6].offset = (u32)edx_c;          // 0x2fd276
            fmt->attribs[6].type = 0x1406;                // 0x2fd289
            fmt->attribs[6].size = 4;                     // 0x2fd293
            fmt->attribs[7].unknown_0c = 0;               // 0x2fd29d (+0xb8)
            fmt->attribs[7].offset = (u32)(edx_c + 0x10); // 0x2fd27c lea ecx,[edx+0x10]
            fmt->attribs[7].type = 0x1406;                // 0x2fd2ad
            fmt->attribs[7].size = 2;                     // 0x2fd2b7
            fmt->attribs[7].normalized = 0;               // 0x2fd2c1 (+0xb4)
            mask |= 0xc0;                                 // 0x2fd270
            edx_c += 0x18;                                // 0x2fd2c8
            break;
        case 4:                             // 0x2fd1f3: vec4 + vec4
            fmt->attribs[6].normalized = 0; // 0x2fd1f3
            edx_c = (i32)(i16)(edx_c & 0xffff);
            fmt->attribs[6].unknown_0c = 0;               // 0x2fd20f
            fmt->attribs[6].offset = (u32)edx_c;          // 0x2fd206
            fmt->attribs[6].type = 0x1406;                // 0x2fd219
            fmt->attribs[6].size = 4;                     // 0x2fd223
            fmt->attribs[7].unknown_0c = 0;               // 0x2fd22d
            fmt->attribs[7].offset = (u32)(edx_c + 0x10); // 0x2fd20c
            fmt->attribs[7].type = 0x1406;                // 0x2fd23d
            fmt->attribs[7].size = 4;                     // 0x2fd247
            fmt->attribs[7].normalized = 0;               // 0x2fd251
            mask |= 0xc0;                                 // 0x2fd200
            edx_c += 0x20;                                // 0x2fd258
            break;
        case 5:                             // 0x2fd183: vec2 + vec2
            fmt->attribs[6].normalized = 0; // 0x2fd183
            edx_c = (i32)(i16)(edx_c & 0xffff);
            fmt->attribs[6].unknown_0c = 0;            // 0x2fd19f
            fmt->attribs[6].offset = (u32)edx_c;       // 0x2fd196
            fmt->attribs[6].type = 0x1406;             // 0x2fd1a9
            fmt->attribs[6].size = 2;                  // 0x2fd1b3
            fmt->attribs[7].unknown_0c = 0;            // 0x2fd1bd
            fmt->attribs[7].offset = (u32)(edx_c + 8); // 0x2fd19c lea ecx,[edx+8]
            fmt->attribs[7].type = 0x1406;             // 0x2fd1cd
            fmt->attribs[7].size = 2;                  // 0x2fd1d7
            fmt->attribs[7].normalized = 0;            // 0x2fd1e1
            mask |= 0xc0;                              // 0x2fd190
            edx_c += 0x10;                             // 0x2fd1e8
            break;
        case 0:
        default: // 0x2fce30
            break;
    }

    // 0x2fce30-0x2fce7c / 0x2fd100: half-float texcoord emission (bit 27 set),
    // selected by the number of tex units in esp+0x60.
    if (half_units == 2) { // 0x2fd6f8: hvec4
        edx_c = (i32)(i16)(edx_c & 0xffff);
        fmt->attribs[6].unknown_0c = 0;          // 0x2fd6fb
        fmt->attribs[6].offset = (u32)edx_c;     // 0x2fd705
        fmt->attribs[6].type = 0x8d61;           // 0x2fd70b GL_HALF_FLOAT
        fmt->attribs[6].size = 4;                // 0x2fd715
        fmt->attribs[6].normalized = 0;          // 0x2fd71f
        mask |= 0x40;                            // 0x2fd726
        edx_c += 8;                              // 0x2fd729
    } else if (half_units > 2) {                 // 0x2fd100
        if (half_units == 3) {                   // 0x2fd688: hvec4 + hvec2
            fmt->attribs[6].normalized = 0;      // 0x2fd688
            mask |= 0xc0;                        // 0x2fd695
            edx_c = (i32)(i16)(edx_c & 0xffff);  // 0x2fd692
            fmt->attribs[6].offset = (u32)edx_c; // 0x2fd698
            esi_c = edx_c + 8;                   // 0x2fd69e
            fmt->attribs[6].unknown_0c = 0;      // 0x2fd6a1
            fmt->attribs[6].type = 0x8d61;       // 0x2fd6ab
            fmt->attribs[6].size = 4;            // 0x2fd6b5
            fmt->attribs[7].unknown_0c = 0;      // 0x2fd6bf
            fmt->attribs[7].offset = (u32)esi_c; // 0x2fd6c9
            fmt->attribs[7].type = 0x8d61;       // 0x2fd6cf
            fmt->attribs[7].size = 2;            // 0x2fd6d9
            fmt->attribs[7].normalized = 0;      // 0x2fd6e3
            edx_c += 0xc;                        // 0x2fd6ea
        } else if (half_units == 4) {            // 0x2fd116: hvec4 + hvec4
            fmt->attribs[6].normalized = 0;      // 0x2fd116
            mask |= 0xc0;                        // 0x2fd123
            edx_c = (i32)(i16)(edx_c & 0xffff);  // 0x2fd120
            fmt->attribs[6].offset = (u32)edx_c; // 0x2fd126
            esi_c = edx_c + 8;                   // 0x2fd12c
            fmt->attribs[6].unknown_0c = 0;      // 0x2fd12f
            fmt->attribs[6].type = 0x8d61;       // 0x2fd139
            fmt->attribs[6].size = 4;            // 0x2fd143
            fmt->attribs[7].unknown_0c = 0;      // 0x2fd14d
            fmt->attribs[7].offset = (u32)esi_c; // 0x2fd157
            fmt->attribs[7].type = 0x8d61;       // 0x2fd15d
            fmt->attribs[7].size = 4;            // 0x2fd167
            fmt->attribs[7].normalized = 0;      // 0x2fd171
            edx_c += 0x10;                       // 0x2fd178
        }
        // half_units > 4: nothing (0x2fd10b/0x2fd110 jump past).
    } else if (half_units == 1) {            // 0x2fce48: hvec2
        edx_c = (i32)(i16)(edx_c & 0xffff);  // 0x2fce48
        fmt->attribs[6].unknown_0c = 0;      // 0x2fce4b
        fmt->attribs[6].offset = (u32)edx_c; // 0x2fce55 (+0xa4)
        fmt->attribs[6].type = 0x8d61;       // 0x2fce5b
        fmt->attribs[6].size = 2;            // 0x2fce65
        fmt->attribs[6].normalized = 0;      // 0x2fce6f
        mask |= 0x40;                        // 0x2fce76
        edx_c += 4;                          // 0x2fce79
    }
    // half_units == 0: no texcoords.

    // 0x2fce80: location 10. 1 -> float x2 (0x2fd398), 2 -> UBYTE x4 normalized,
    // 0 -> absent.
    if (sel_loc10 == 1) {
        edx_c = (i32)(i16)(edx_c & 0xffff);   // 0x2fd398
        fmt->attribs[10].unknown_0c = 0;      // 0x2fd39b (+0x100)
        fmt->attribs[10].offset = (u32)edx_c; // 0x2fd3a5 (+0x104)
        fmt->attribs[10].type = 0x1406;       // 0x2fd3ab (+0xf4) GL_FLOAT
        fmt->attribs[10].size = 2;            // 0x2fd3b5 (+0xf8)
        fmt->attribs[10].normalized = 0;      // 0x2fd3bf (+0xfc)
        mask |= 0x400;                        // 0x2fd3c6
        edx_c += 8;                           // 0x2fd3cc
    } else if (sel_loc10 == 2) {              // 0x2fce92
        edx_c = (i32)(i16)(edx_c & 0xffff);
        fmt->attribs[10].unknown_0c = 0;      // 0x2fce95
        fmt->attribs[10].offset = (u32)edx_c; // 0x2fce9f
        fmt->attribs[10].type = 0x1401;       // 0x2fcea5 GL_UNSIGNED_BYTE
        fmt->attribs[10].size = 4;            // 0x2fceaf
        fmt->attribs[10].normalized = 1;      // 0x2fceb9
        mask |= 0x400;                        // 0x2fcec0
        edx_c += 4;                           // 0x2fcec6
    }

    // 0x2fcec9: location 11. 1 -> float x3 (0x2fd358), 2 -> UBYTE x4 NOT
    // normalized, 0 -> absent.
    if (sel_loc11 == 1) {
        edx_c = (i32)(i16)(edx_c & 0xffff);   // 0x2fd358
        fmt->attribs[11].unknown_0c = 0;      // 0x2fd35b (+0x118)
        fmt->attribs[11].offset = (u32)edx_c; // 0x2fd365 (+0x11c)
        fmt->attribs[11].type = 0x1406;       // 0x2fd36b (+0x10c) GL_FLOAT
        fmt->attribs[11].size = 3;            // 0x2fd375 (+0x110)
        fmt->attribs[11].normalized = 0;      // 0x2fd37f (+0x114)
        mask |= 0x800;                        // 0x2fd386
        edx_c += 0xc;                         // 0x2fd38c
    } else if (sel_loc11 == 2) {              // 0x2fcedb
        edx_c = (i32)(i16)(edx_c & 0xffff);
        fmt->attribs[11].unknown_0c = 0;      // 0x2fcede
        fmt->attribs[11].offset = (u32)edx_c; // 0x2fcee8
        fmt->attribs[11].type = 0x1401;       // 0x2fceee
        fmt->attribs[11].size = 4;            // 0x2fcef8
        fmt->attribs[11].normalized = 0;      // 0x2fcf02 (+0x114) note: NOT normalized
        mask |= 0x800;                        // 0x2fcf09
        edx_c += 4;                           // 0x2fcf0f
    }

    // 0x2fcf12: locations 8+9 emitted together as two UBYTE x4 normalized
    // attributes when desc bit 26 is set.
    if (byte3_bit26 != 0) {
        fmt->attribs[8].normalized = 1;      // 0x2fcf1c (+0xcc)
        mask |= 0x300;                       // 0x2fcf29 (or ch,3)
        edx_c = (i32)(i16)(edx_c & 0xffff);  // 0x2fcf26
        fmt->attribs[8].offset = (u32)edx_c; // 0x2fcf2c (+0xd4)
        esi_c = edx_c + 4;                   // 0x2fcf32
        fmt->attribs[8].unknown_0c = 0;      // 0x2fcf35 (+0xd0)
        fmt->attribs[8].type = 0x1401;       // 0x2fcf3f (+0xc4)
        fmt->attribs[8].size = 4;            // 0x2fcf49 (+0xc8)
        fmt->attribs[9].unknown_0c = 0;      // 0x2fcf53 (+0xe8)
        fmt->attribs[9].offset = (u32)esi_c; // 0x2fcf5d (+0xec)
        fmt->attribs[9].type = 0x1401;       // 0x2fcf63 (+0xdc)
        fmt->attribs[9].size = 4;            // 0x2fcf6d (+0xe0)
        fmt->attribs[9].normalized = 1;      // 0x2fcf77 (+0xe4)
        edx_c += 8;                          // 0x2fcf7e
    }

    // 0x2fcf84: location 12 — duplicate of the position stream, emitted when
    // desc bit 22 is set.
    if (has_extra_pos == 1) {            // -> 0x2fd310
        fmt->attribs[12].normalized = 0; // 0x2fd310 (+0x12c)
        mask |= 0x1000;                  // 0x2fd31a (or ch,0x10)
        fmt->attribs[12].unknown_0c = 1; // 0x2fd31d (+0x130)
        fmt->attribs[12].offset = 0;     // 0x2fd327 (+0x134)
        fmt->attribs[12].type = 0x1406;  // 0x2fd331 (+0x124) GL_FLOAT
        fmt->attribs[12].size = 3;       // 0x2fd33b (+0x128)
        esi_c = 0xc;                     // 0x2fd345
    } else {
        esi_c = 0; // 0x2fcf92
    }

    // 0x2fcf94-0x2fd0af: stride fixup. Every enabled attribute receives the final
    // total vertex size as its stride, except where its unknown_0c marker is set:
    // those take esi_c instead (location 12 ends up with the fixed 0xc).
    {
        const i32 total = (i32)(i16)(edx_c & 0xffff); // movsx reg, dx
        if ((mask & 0x001) != 0) {                    // 0x2fcf94 test cl,1
            i32 st = total;
            if (fmt->attribs[0].unknown_0c != 0)
                st = esi_c;                   // cmovnz
            fmt->attribs[0].stride = (u32)st; // (+0x18)
        }
        if ((mask & 0x002) != 0) { // 0x2fcfa6 test cl,2
            i32 st = total;
            if (fmt->attribs[1].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[1].stride = (u32)st; // (+0x30)
        }
        if ((mask & 0x004) != 0) { // 0x2fcfb8 test cl,4
            i32 st = total;
            if (fmt->attribs[2].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[2].stride = (u32)st; // (+0x48)
        }
        if ((mask & 0x008) != 0) { // 0x2fcfca test cl,8
            i32 st = total;
            if (fmt->attribs[3].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[3].stride = (u32)st; // (+0x60)
        }
        if ((mask & 0x010) != 0) { // 0x2fcfdc test cl,0x10
            i32 st = total;
            if (fmt->attribs[4].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[4].stride = (u32)st; // (+0x78)
        }
        if ((mask & 0x020) != 0) { // 0x2fcfee test cl,0x20
            i32 st = total;
            if (fmt->attribs[5].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[5].stride = (u32)st; // (+0x90)
        }
        if ((mask & 0x040) != 0) { // 0x2fd006 test cl,0x40
            i32 st = total;
            if (fmt->attribs[6].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[6].stride = (u32)st; // (+0xa8)
        }
        if ((mask & 0x080) != 0) { // 0x2fd01e test cl,0x80
            i32 st = total;
            if (fmt->attribs[7].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[7].stride = (u32)st; // (+0xc0)
        }
        if ((mask & 0x100) != 0) { // 0x2fd036 test ch,1
            i32 st = total;
            if (fmt->attribs[8].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[8].stride = (u32)st; // (+0xd8)
        }
        if ((mask & 0x200) != 0) { // 0x2fd04e test ch,2
            i32 st = total;
            if (fmt->attribs[9].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[9].stride = (u32)st; // (+0xf0)
        }
        if ((mask & 0x400) != 0) { // 0x2fd066 test ch,4
            i32 st = total;
            if (fmt->attribs[10].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[10].stride = (u32)st; // (+0x108)
        }
        if ((mask & 0x800) != 0) { // 0x2fd07e test ch,8
            i32 st = total;
            if (fmt->attribs[11].unknown_0c != 0)
                st = esi_c;
            fmt->attribs[11].stride = (u32)st; // (+0x120)
        }
        if ((mask & 0x1000) != 0) { // 0x2fd096 test ch,0x10
            // Inverted form: esi_c is kept when the marker is nonzero (it holds
            // 0xc on this path), otherwise replaced by the total.
            if (fmt->attribs[12].unknown_0c == 0)
                esi_c = total;                    // 0x2fd0a6 cmovz esi, edx
            fmt->attribs[12].stride = (u32)esi_c; // 0x2fd0a9 (+0x138)
        }
    }

    // 0x2fd0af-0x2fd0e1: commit the new record and return record + 4.
    fmt->attrib_mask = mask;
    rec->key = desc_norm;                   // 0x2fd0c7
    g_allocatedDescriptorCount = count + 1; // 0x2fd0ca-0x2fd0d4
    return fmt;
}
