#ifndef GAMELIB_UTIL_TYPES_H
#define GAMELIB_UTIL_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"

struct AIPATHNODE_s;
struct AndroidOBBUtils;
struct BOLT_s;
struct CRC16;
struct EdClass;
struct EdStream;
struct FtpFile;
struct GIZFORCE_s;
struct GIZMOBLOWUP_s;
struct GameObject_s;
struct NOSContext;
struct NOSFilter;
struct NetAddress;
struct NetChangedReplicator;
struct NetConstReplicator;
struct NetFtpManager;
struct NetListenerBinding;
struct NetListenerInterface;
struct NetListenerList;
struct NetMessage;
struct NetPeer;
struct NetPredictor;
struct NetPredictor2;
struct NetPredictor3;
struct NetReplicator;
struct NetRotator2;
struct NetSample;
struct NetSimpleReplicator;
struct NetSmallStats;
struct NetStats;
struct NetTransporter;
struct NetworkObject;
struct NetworkObjectManager;
struct nucolour3_s;
struct NuFileDeviceAndroidOBBType;
struct ReplicatorData;
struct TouchHacks;
struct V2SessionManager;
struct VirtualStackAllocator;
struct VuVec;
struct WORLDINFO_s;
struct ePeerLeftReason;

struct AIPATHNODE_s;
struct BOLT_s;
struct EdClass;
struct EdStream;
struct GIZFORCE_s;
struct GIZMOBLOWUP_s;
struct GameObject_s;
struct NOSContext {};
struct NOSFilter {};
struct NetAddress {};
struct NetListenerInterface {};
struct NetPeer {};
struct ReplicatorData {};
struct WORLDINFO_s;
struct ePeerLeftReason {};

struct NuFileDeviceAndroidOBBType {
    struct T {};
};
struct AndroidOBBUtils {
    void InitPackagePaths();
    void LookupPackagePath(char *, NuFileDeviceAndroidOBBType::T);
    void OpenFile(char const *);
};
struct CRC16 {
    CRC16();
    void hash(unsigned char const *, i32);
    void hashInverse(unsigned char const *, i32);
};
struct FtpFile {
    void Accept();
    void Accept(i32);
    void Accept(i32, void *);
    void Init(i32, char const *, i32, NetAddress const &, void *, i32);
    void RecvData(NetMessage &);
    void SendData();
    void Term();
    void Update();
};
struct NetChangedReplicator {
    void AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32);
    void CheckSum(unsigned char const *, u32, u32 &) const;
    void CheckSumObject(EdClass const *, void const *, u32 &) const;
    void InitTable();
};
struct NetConstReplicator {
    void AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32);
};
struct NetFtpManager {
    void Abort(char const *, NetAddress const &, i32, i32);
    void FindTransfer(char const *, NetAddress const &, i32);
    void FindTransfer(char const *, NetAddress const &, i32) const;
    void Get(char const *, void *, i32, NetAddress const &);
    void GetTransfers() const;
    void Init();
    NetFtpManager();
    void PeerLeft(NetAddress const &, ePeerLeftReason);
    void Receive(NetMessage, unsigned char, NetAddress const &);
    void Reset();
    void Send(char const *, void const *, i32, NetAddress const &);
    void Term();
    void Update();
    virtual ~NetFtpManager();
};
struct NetListenerBinding {
    NetListenerBinding(NetListenerInterface *, unsigned char, char *);
    void operator=(NetListenerBinding const &);
    void operator==(NetListenerBinding const &);
};
struct NetListenerList {
    void Find(NetListenerBinding *);
};
struct NetMessage {
    void DebugPrint() const;
    void RaiseError();
};
struct NetPredictor {
    struct PredictorData {};
    struct PredictorTime {};
    void AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32);
    void CheckPredictionError(EdClass const *, void *, float *, float *, i32);
    void DoPrediction(EdClass const *, void *, ReplicatorData &, NetPredictor::PredictorTime *, i32);
    void DoPrediction(EdClass const *, void *, ReplicatorData &, i32);
    void SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &,
                         NetPredictor::PredictorTime *, i16 *);
    void SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &, i16 *);
    void StoreSampleData(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **,
                         float *, i32);
};
struct NetPredictor2 {
    void PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **, float *,
                      i32);
};
struct NetPredictor3 {
    void PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **, float *,
                      i32);
};
struct NetReplicator {
    NetReplicator(i32, float, float);
    void SerialiseObject(EdStream &, NetPeer *, EdClass const *, void *, ReplicatorData &, i16 *);
};
struct NetRotator2 {
    void PredictValue(EdClass const *, void *, NetPredictor::PredictorTime *, NetPredictor::PredictorData **, float *,
                      i32);
};
struct NetSample {
    void Max(NetSample const &);
    void Reset();
    void operator+=(NetSample const &);
    void operator-=(NetSample const &);
};
struct NetSimpleReplicator {
    void AllowPush(EdClass const *, void const *, ReplicatorData &, i32, i32);
};
struct NetSmallStats {
    struct eInfo {};
    void Draw(float, float, float, float, NetSmallStats::eInfo) const;
};
struct NetStats {
    void Draw(float, float, float, float, NetSmallStats::eInfo) const;
    void Update();
};
struct NetTransporter {
    void AddListener(NetListenerInterface *, unsigned char, char *);
    void Distribute(NetMessage const &, unsigned char, NetPeer const &) const;
    void FtpComplete(FtpFile *, i32) const;
    void FtpDownload(FtpFile *) const;
    void FtpUpload(FtpFile *) const;
    void NosAcquire(NetworkObject *, NetPeer const &) const;
    void NosAdopted(NetworkObject *, NetPeer const &) const;
    void PeerDead(NetPeer const &) const;
    void PeerJoined(NetPeer const &) const;
    void PeerLeft(NetPeer const &, ePeerLeftReason) const;
    void PeerRequest(NetPeer const &) const;
    void RemoveListener(NetListenerInterface *, unsigned char);
    void StatsReceiveMessage(NetMessage, unsigned char);
    void StatsSendMessage(NetMessage, unsigned char);
    void StatsUpdate();
};
struct NetworkObject {
    void Destroy();
    void Initialise(i32, void *, EdClass *, NetPeer const &, i32);
};
struct NetworkObjectManager {
    struct NetPeerPush {
        void FlushMessages();
        void GetMessage(i32);
        void GetReliableMessage(i32);
        void NextStage();
        void Stop();
        void Sync();
    };
    void Acquire(i32);
    void AddToLocalObjectList(NetworkObject *);
    void BindFilter(NOSFilter *, EdClass const *);
    void BindReplicator(NetReplicator *, EdClass const *);
    void CalcReplicatorDataSize(NetReplicator *, EdClass const *, i32 &, i32 &);
    void ChangeContext(NOSContext &);
    void ConstructObject(NetworkObject *, NetworkObjectManager::NetPeerPush *);
    void ContinuityBreak(i32, float);
    void FindNetworkObject(i32);
    void FindNetworkObject(void *);
    void FindPendingObject(NetworkObject *);
    void FlushObjects(i32);
    void GetGuid(void *);
    void GetNextGuid();
    void GetObject(i32);
    void GetPeerStatus();
    void ImportObjects();
    void Init();
    void InitClassStats();
    void IsLocal(i32);
    void IsPeerReady(NetPeer const &) const;
    void IsPeerStarted(NetPeer const &) const;
    NetworkObjectManager();
    void NotifyCreateObject(void *, EdClass *, void *, i32, i32, i32);
    void NotifyDestroyObject(void *, EdClass *, i32, i32);
    void ObjectCall(void *, i32, NetMessage, NetPeer const *);
    void ObjectOtherCall(void *, i32, NetMessage);
    void ObjectOwnerCall(void *, i32, NetMessage);
    void Owner(i32);
    void PeerJoined(NetPeer const &);
    void PeerLeft(NetPeer const &, ePeerLeftReason);
    void Push(NetworkObject const *, NetReplicator *, ReplicatorData &, NetworkObjectManager::NetPeerPush *);
    void PushObject(NetworkObject *, NetworkObjectManager::NetPeerPush *, i32);
    void Receive(NetMessage, unsigned char, NetPeer const &);
    void ReceiveAcquireMessage(NetMessage &, NetPeer const &);
    void ReceiveAcquiredMessage(NetMessage &, NetPeer const &);
    void ReceiveAdoptedMessage(NetMessage &, NetPeer const &);
    void ReceiveConstructorMessage(NetMessage &, NetPeer const &);
    void ReceiveContinuityBreak(NetMessage &, NetPeer const &);
    void ReceiveObjectCallMessage(NetMessage &, NetPeer const &);
    void ReceiveReleaseMessage(NetMessage &, NetPeer const &);
    void ReceiveRemoteCallMessage(NetMessage &, NetPeer const &);
    void ReceiveReplicaMessage(NetMessage &, NetPeer const &);
    void ReceiveStartMessage(NetMessage &, NetPeer const &);
    void ReceiveStatusMessage(NetMessage &, NetPeer const &);
    void ReceiveStopMessage(NetMessage &, NetPeer const &);
    void Recover(NetworkObject *);
    void RegisterObject(void *, EdClass *, i32);
    void RegisterObjectCall(void (*)(void *, NetMessage &), i32);
    void RegisterRemoteCall(void (*)(NetMessage &), i32);
    void ReleaseObject(void *, EdClass *, i32);
    void RemoteCall(i32, NetMessage, NetPeer const *);
    void RemoveFromLocalObjectList(NetworkObject *);
    void RemovePendingObject(NetworkObject *);
    void Reset();
    void SendAcquireMessage(NetworkObject *);
    void SendAcquiredMessage(i16, NetPeer const &);
    void SendAdoptedMessage(i16);
    void SendPushMessage(NetMessage *, NetworkObjectManager::NetPeerPush const *, i32);
    void Start(NOSContext const &);
    void StealPendingObject();
    void Stop();
    void Term();
    void Update();
    void UpdateLocalObjectList();
    virtual ~NetworkObjectManager();
};
struct TouchHacks {
    static bool TouchControlsActive;

    struct TintStack {
        float ambient[3];

        TintStack();
        ~TintStack();
    };
    void AiPlayerTakeDamageOnKillRescue(GameObject_s &);
    void CalculateJumpVelToHitPoint(GameObject_s &, VuVec const &);
    void CalculateJumpVelToHitPointDblJump(GameObject_s &, VuVec const &);
    void CalculateXZVelForArcToHitPoint(VuVec const &, VuVec const &, float, float);
    void CanBlowupBeBlownUp(GIZMOBLOWUP_s &, i32);
    void CanForceTargetObj(GameObject_s &, GameObject_s &);
    void CanJump(GameObject_s &);
    void CanJumpToPoint(GameObject_s &, AIPATHNODE_s const &);
    void CanJumpToPoint(GameObject_s &, VuVec const &);
    void CanLunge(GameObject_s &);
    void CanPoo(GameObject_s &);
    void CanShoot(GameObject_s &);
    void CanSlam(GameObject_s &);
    void CanTagTo(GameObject_s &, GameObject_s &);
    void CanTagVehicle(GameObject_s &, GameObject_s &);
    void CanThrowBountyBomb(GameObject_s &);
    void CanToggleTo(GameObject_s &, i32);
    void CanUseBuildIt(GameObject_s &);
    void CanUseGizForce(GameObject_s &);
    void CanUseGizForce(GameObject_s &, GIZFORCE_s &);
    void CanUseHatMachine(GameObject_s &);
    void CanUseLever(GameObject_s &);
    void CanUseTeleport(GameObject_s &);
    void CanUseVehicleSmartBomb(GameObject_s &);
    void CanUseZipup(GameObject_s &);
    void CheckForAboutToRunIntoKillTerrain(GameObject_s &, float);
    void CheckForAboutToRunOffAnEdge(GameObject_s &, float);
    void CheckJumpForLandingSpot(GameObject_s &, float);
    static void CleanupAllMechObjectInterfaces(WORLDINFO_s *);
    void FindBombTarget(GameObject_s &);
    static nucolour3_s *GetFlashColour();
    float GetIncomingPartRange();
    i32 GetLoseStudsDieValue();
    i32 GetLoseStudsFallValue();
    bool InParty(GameObject_s &);
    void PlaySmartBombBuildupEffects(GameObject_s &, float, float);
    void ShouldAutoGrabDragBomb(GameObject_s &);
    void ShouldBlock(GameObject_s &);
    void ShouldDeflectBolt(GameObject_s &, BOLT_s &);
    static bool ShouldFlash(float);
    void ShouldKeepWeaponOut(GameObject_s &);
    void ShouldPutWeaponAway(GameObject_s &);
    bool SolveRoot(float, float, float, float &, float &);
    void TriggerVehicleSmartBomb(GameObject_s &);
};
struct V2SessionManager {
    void Log(char *, ...);
    void RemoveAllPeers(ePeerLeftReason);
    void RemovePeer(NetPeer *, ePeerLeftReason);
    void Reset();
    void SetHostGameData(i32 *, i32);
    void Update();
    V2SessionManager(char *);
    void VerifyStrings(char **, char **, i32, char *);
};
struct VirtualStackAllocator {
    VirtualStackAllocator();
    VirtualStackAllocator(VirtualStackAllocator &, u32);
    VirtualStackAllocator(i32);
    VirtualStackAllocator(void *, u32);
    void setExternalMemoryPool(void *, u32);
    ~VirtualStackAllocator();
};

#endif // GAMELIB_UTIL_TYPES_H
