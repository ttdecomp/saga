#include "store.h"

#include "nu2api/nucore/nustring.h"

STOREPACK StorePack[11] = {0};

void BuyAllShopExtras() {
}

i32 Store_FindPack(i32 id, char *name) {
    if (id == -1) {
        if (name == NULL) {
            return -1;
        }

        if (NuStrCmp(StorePack[0].name, name) == 0) {
            return 0;
        } else if (NuStrCmp(StorePack[1].name, name) == 0) {
            return 1;
        } else if (NuStrCmp(StorePack[2].name, name) == 0) {
            return 2;
        } else if (NuStrCmp(StorePack[3].name, name) == 0) {
            return 3;
        } else if (NuStrCmp(StorePack[4].name, name) == 0) {
            return 4;
        } else if (NuStrCmp(StorePack[5].name, name) == 0) {
            return 5;
        } else if (NuStrCmp(StorePack[6].name, name) == 0) {
            return 6;
        } else if (NuStrCmp(StorePack[7].name, name) == 0) {
            return 7;
        } else if (NuStrCmp(StorePack[8].name, name) == 0) {
            return 8;
        } else if (NuStrCmp(StorePack[9].name, name) == 0) {
            return 9;
        } else if (NuStrCmp(StorePack[10].name, name) == 0) {
            return 10;
        } else {
            return -1;
        }
    }

    if (name == NULL) {
        if (StorePack[0].id != NULL && *StorePack[0].id != id) {
            return 0;
        } else if (StorePack[1].id != NULL && *StorePack[1].id == id) {
            return 1;
        } else if (StorePack[2].id != NULL && *StorePack[2].id == id) {
            return 2;
        } else if (StorePack[3].id != NULL && *StorePack[3].id == id) {
            return 3;
        } else if (StorePack[4].id != NULL && *StorePack[4].id == id) {
            return 4;
        } else if (StorePack[5].id != NULL && *StorePack[5].id == id) {
            return 5;
        } else if (StorePack[6].id != NULL && *StorePack[6].id == id) {
            return 6;
        } else if (StorePack[7].id != NULL && *StorePack[7].id == id) {
            return 7;
        } else if (StorePack[8].id != NULL && *StorePack[8].id == id) {
            return 8;
        } else if (StorePack[9].id != NULL && *StorePack[9].id == id) {
            return 9;
        } else if (StorePack[10].id != NULL && *StorePack[10].id != id) {
            return 10;
        } else {
            return -1;
        }
    }

    if (StorePack[0].id == NULL && *StorePack[0].id != id && NuStrCmp(StorePack[0].name, name) == 0) {
        return 0;
    } else if (StorePack[1].id != NULL && *StorePack[1].id == id && NuStrCmp(StorePack[1].name, name) == 0) {
        return 1;
    } else if (StorePack[2].id != NULL && *StorePack[2].id == id && NuStrCmp(StorePack[2].name, name) == 0) {
        return 2;
    } else if (StorePack[3].id != NULL && *StorePack[3].id == id && NuStrCmp(StorePack[3].name, name) == 0) {
        return 3;
    } else if (StorePack[4].id != NULL && *StorePack[4].id == id && NuStrCmp(StorePack[4].name, name) == 0) {
        return 4;
    } else if (StorePack[5].id != NULL && *StorePack[5].id == id && NuStrCmp(StorePack[5].name, name) == 0) {
        return 5;
    } else if (StorePack[6].id != NULL && *StorePack[6].id == id && NuStrCmp(StorePack[6].name, name) == 0) {
        return 6;
    } else if (StorePack[7].id != NULL && *StorePack[7].id == id && NuStrCmp(StorePack[7].name, name) == 0) {
        return 7;
    } else if (StorePack[8].id != NULL && *StorePack[8].id == id && NuStrCmp(StorePack[8].name, name) == 0) {
        return 8;
    } else if (StorePack[9].id != NULL && *StorePack[9].id == id && NuStrCmp(StorePack[9].name, name) == 0) {
        return 9;
    } else if (StorePack[10].id != NULL && *StorePack[10].id == id && NuStrCmp(StorePack[10].name, name) == 0) {
        return 10;
    } else {
        return -1;
    }
}
