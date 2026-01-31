typedef struct SaveLoad SaveLoad, *PSaveLoad;

struct SaveLoad {
    int field0_0x0;
    int field1_0x4;
    int size;
    int field3_0xc;
    int field4_0x10;
    int extradata_offset;
    char field6_0x18[16];
    short field7_0x28;
    char field8_0x2a[2046];
    short field9_0x828;
    char field10_0x82a[2046];
    short field11_0x1028;
    char field12_0x102a[2046];
    short field13_0x1828;
    char field14_0x182a[2046];
};
