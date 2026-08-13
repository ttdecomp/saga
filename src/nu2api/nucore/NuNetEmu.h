#pragma once

struct nunetaddr_s;

struct NetSmallStats {
    enum eInfo {};
};

struct NuNetEmu {
    struct EmuPacket {
        void AddPayload(void *, i32);
        EmuPacket(nunetaddr_s *);
        ~EmuPacket();
    };
    struct PackStats {
        void Draw(float, float, float, float, NetSmallStats::eInfo) const;
    };
    struct eConditions {};
    void FindPacket(nunetaddr_s *, i32);
    NuNetEmu();
    void RecvFrom(void *, i32, nunetaddr_s &);
    void SendTo(void *, i32, nunetaddr_s *, i32);
    void SetConditions(NuNetEmu::eConditions);
    void SplitSendPacket(NuNetEmu::EmuPacket *);
    void Update();
};
