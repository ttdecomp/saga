#pragma once

// TM stands for "Target Manager"
// seemed to be some sort of remote server for something?
class TMClient {
    public:
        TMClient(int use_target_manager, char* target_manager_mac_address);

    private:
        char filler[0x530];
};

extern TMClient* the_tm_client;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif