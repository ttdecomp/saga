#include "nu2api/nu3d/nutex.h"

#include <pthread.h>
#include <string.h>

#include "nu2api/nu3d/android/nutex_android.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/nustring.h"

void NuChecksumAsHex(u8 *checksum, char *out) {
    i32 i;
    char hex_digits[] = "0123456789abcdef";

    for (i = 0; i < 16; i++) {
        u8 check_digit = checksum[i];
        i32 least_sig_digit = check_digit >> 4;

        out[i * 2] = hex_digits[least_sig_digit];
        out[i * 2 + 1] = hex_digits[(u8)(check_digit - (least_sig_digit << 4))];
    }

    out[32] = '\0';
}

void NuTexHiresFilename(int tex_id, char *filename) {
    NUNATIVETEX *tex;
    char checksum_hex[33];

    tex = NuTexGetNative(tex_id);

    NuStrCpy(filename, "c:\\temp\\stream\\textures\\");
    NuChecksumAsHex(tex->checksum, checksum_hex);
    NuStrCat(filename, checksum_hex);
    NuStrCat(filename, ".tex");
}

int NuTexSwapHires(int tex_id_lo, int tex_id_hi) {
    return 0;
}

void NuTexLoadHires(int tex_id) {
    char hires_path[2048];
    int tex_id_hi;

    NuTexHiresFilename(tex_id, hires_path);
    tex_id_hi = NuTexRead(hires_path, NULL, NULL);
    NuTexSwapHires(tex_id, tex_id_hi);
}

void NuTexUnloadHires(int tex_id) {
}

void NuTexAddReference(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count++;
    }
}

void NuTexRemoveReference(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        tex->ref_count--;
    }
}

int NuTexGetRefCount(int tex_id) {
    NUNATIVETEX *tex;

    tex = NuTexGetNative(tex_id);
    if (tex != NULL) {
        return tex->ref_count;
    }

    return 0;
}

i32 max_textures;
static NUNATIVETEX **texture_list;
static i32 *texture_order;
static i32 gTextureLoadCount;

void NuTexInitEx(VARIPTR *buf, i32 max_tex_count) {
    max_textures = max_tex_count;

    texture_list = (NUNATIVETEX **)ALIGN(buf->addr, 0x4);
    buf->addr = (usize)texture_list + max_tex_count * sizeof(NUNATIVETEX *);
    memset(texture_list, 0, max_tex_count * sizeof(NUNATIVETEX *));

    texture_order = (i32 *)ALIGN(buf->addr, 0x4);
    buf->addr = (usize)texture_order + max_tex_count * sizeof(i32);
    memset(texture_order, 0, max_tex_count * sizeof(i32));

    gTextureLoadCount = 0;
}

pthread_mutex_t criticalSection = PTHREAD_MUTEX_INITIALIZER;

int NuTexCreateNative(NUNATIVETEX *tex, bool is_pvrtc) {
    int i;

    if (tex == NULL) {
        return 0;
    }

    pthread_mutex_lock(&criticalSection);

    for (int i = 0; i < max_textures; i++) {
        if (texture_list[i] == NULL) {
            texture_list[i] = tex;
            texture_order[i] = gTextureLoadCount++;

            pthread_mutex_unlock(&criticalSection);

            NuTexCreatePS(tex, is_pvrtc);

            return i + 1;
        }
    }

    pthread_mutex_unlock(&criticalSection);

    return 0;
}

NUNATIVETEX *NuTexGetNative(int tex_id) {
    if (tex_id > 0) {
        return texture_list[tex_id - 1];
    }

    return NULL;
}

int NuTexWidth(int tex_id) {
    return texture_list[tex_id - 1]->width;
}

int NuTexHeight(int tex_id) {
    return texture_list[tex_id - 1]->height;
}



struct nutexformat_s {




} typedef NUTEXFORMAT;


/* NuDDSGetTextureDescription(char const*, NUTEXFORMAT&, int&, int&, int&, int&, bool&, bool*) */

i32 NuDDSGetTextureDescription(char *dds_buffer,NUTEXFORMAT *param_2,int *param_3,int *param_4,int *param_5,int *param_6,bool *param_7,bool *param_8)

{
  i32 uVar1;
  i32 uVar2;
  bool bVar3;
  
  if (dds_buffer[0] != 'D') {
    return 0;
  }
  if (dds_buffer[1] != 'D') {
    return 0;
  }
  if (dds_buffer[2] != 'S') {
    return 0;
  }
  uVar2 = *(uint *)(dds_buffer + 0x54);
  *param_8 = uVar2 != 0;
  uVar1 = *(uint *)(dds_buffer + 0x50);
  if ((uVar1 & 0x40) == 0) {
    if ((uVar1 & 0x20) != 0) {
      *(i32 *)param_2 = 0xc;
      goto LAB_0053a2a5;
    }
    if ((uVar1 & 8) != 0) {
      *(i32 *)param_2 = 0xd;
      goto LAB_0053a2a5;
    }
    if (uVar2 == 0x33545844) {
      *(i32 *)param_2 = 4;
      goto LAB_0053a2a5;
    }
    if (0x33545844 < uVar2) {
      if (uVar2 == 0x41315844) {
        *(i32 *)param_2 = 2;
      }
      else if (uVar2 < 0x41315845) {
        if (uVar2 == 0x34545844) {
          *(i32 *)param_2 = 5;
        }
        else if (uVar2 < 0x34545845) {
          if (uVar2 == 0x344c4150) {
            *(i32 *)param_2 = 0xd;
          }
        }
        else if (uVar2 == 0x35545844) {
          *(i32 *)param_2 = 6;
        }
        else if (uVar2 == 0x384c4150) {
          *(i32 *)param_2 = 0xc;
        }
      }
      else if (uVar2 == 0x41435445) {
        *(i32 *)param_2 = 0x12;
      }
      else if (uVar2 < 0x41435446) {
        if (uVar2 == 0x41325450) {
          *(i32 *)param_2 = 0x15;
        }
        else if (uVar2 == 0x41435441) {
          *(i32 *)param_2 = 0x18;
        }
      }
      else if (uVar2 == 0x41435450) {
        *(i32 *)param_2 = 0x17;
      }
      else if (uVar2 == 0x4e4e4142) {
        *(i32 *)param_2 = 0xe;
      }
      goto LAB_0053a2a5;
    }
    if (uVar2 == 0x31325450) {
      *(i32 *)param_2 = 0x14;
      goto LAB_0053a2a5;
    }
    if (0x31325450 < uVar2) {
      if (uVar2 == 0x31435450) {
        *(i32 *)param_2 = 0x16;
      }
      else if (uVar2 < 0x31435451) {
        if (uVar2 == 0x31435445) {
          *(i32 *)param_2 = 0x11;
        }
      }
      else if (uVar2 == 0x31545844) {
        *(i32 *)param_2 = 1;
      }
      else if (uVar2 == 0x32545844) {
        *(i32 *)param_2 = 3;
      }
      goto LAB_0053a2a5;
    }
    if (uVar2 == 0x71) {
      *(i32 *)param_2 = 8;
      goto LAB_0053a2a5;
    }
    if (0x71 < uVar2) {
      if (uVar2 == 0x74) {
        *(i32 *)param_2 = 9;
      }
      else if (uVar2 == 0x20435441) {
        *(i32 *)param_2 = 0x19;
      }
      goto LAB_0053a2a5;
    }
    if (uVar2 != 0) goto LAB_0053a2a5;
    bVar3 = *(int *)(dds_buffer + 0x58) != 0x20;
  }
  else {
    bVar3 = *(int *)(dds_buffer + 0x58) == 0x18;
  }
  *(uint *)param_2 = (uint)bVar3 * 8 + 7;
LAB_0053a2a5:
  *param_7 = false;
  *param_3 = 0;
  *param_4 = 0;
  *param_5 = 0;
  *param_3 = *(int *)(dds_buffer + 0x10);
  *param_4 = *(int *)(dds_buffer + 0xc);
  if ((dds_buffer[10] & 0x80U) != 0) {
    *param_5 = *(int *)(dds_buffer + 0x18);
  }
  uVar2 = *(uint *)(dds_buffer + 0x1c);
  uVar1 = *(uint *)(dds_buffer + 0x70);
  if (uVar2 == 0) {
    uVar2 = (uint)((dds_buffer[10] & 2U) == 0);
  }
  *param_6 = uVar2;
  if (((dds_buffer[0x6c] & 8U) != 0) && ((uVar1 & 0x200) != 0)) {
    if ((((uVar1 & 0x1000) != 0) && ((((uVar1 & 0x400) != 0 && ((uVar1 & 0x800) != 0)) && ((uVar1 & 0x4000) != 0)))) && (((uVar1 & 0x8000) != 0 && ((uVar1 & 0x2000) != 0)))) {
      *param_7 = true;
      return 1;
    }
    return 1;
  }
  return 1;
}

