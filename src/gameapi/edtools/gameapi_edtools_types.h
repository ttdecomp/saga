#ifndef GAMEAPI_EDTOOLS_TYPES_H
#define GAMEAPI_EDTOOLS_TYPES_H
#pragma once

#include "nu2api/nucore/fixed_width.h"

struct ClassObjectList;
struct EdBitControl;
struct EdClass;
struct EdClassInterface;
struct EdClassObjectNameControl;
struct EdColourControl;
struct EdControl;
struct EdDefunctList;
struct EdEnumControl;
struct EdFileInputStream;
struct EdFileOutputStream;
struct EdInputContext;
struct EdInputStream;
struct EdManMove;
struct EdManRotate;
struct EdManScale;
struct EdManipulator;
struct EdMatrixControl;
struct EdMember;
struct EdObjectNotifier;
struct EdOutputStream;
struct EdRef;
struct EdRefKnot;
struct EdRefPlaceable;
struct EdRefSpecialObject;
struct EdRefSpline;
struct EdRegistry;
struct EdSfxNameControl;
struct EdSpecialObjectControl;
struct EdStream;
struct EdString;
struct EdStringControl;
struct EdSubSystem;
struct EdSystem;
struct EdType;
struct EdVectorControl;
struct EditorSettings;
struct KnotHelper;
struct MemoryBuffer;
struct SplineHelper;
struct SplineKnot;
struct SplineKnotList;
struct SplineObject;
struct SplinePointBlock;
struct SplinePointList;
struct SplineTool;
struct VuMtx;
struct VuVec;
struct burnset_s;
struct eduiiattr_s;
struct eduiitem_s;
struct eduimenu_s;
struct nucamera_s;
struct nugscn_s;
struct nugspline_s;
struct nupad_s;
struct nuvec_s;
struct part_typedesc_s;
union variptr_u;

struct ClassObjectList;
struct EdMember {};
struct EdObjectNotifier {};
struct EdSubSystem {
    virtual ~EdSubSystem();
    virtual void SubInitialise(variptr_u &, variptr_u &, i32);
    virtual void SubReset();
    virtual void SubProcess(float);
    virtual void SubRender();

    EdSubSystem *next;
    EdSubSystem *previous;
};
struct MemoryBuffer;
struct VuMtx;
struct VuVec;
struct burnout_s {
    i32 active;
    u8 reserved_04[0x24 - 0x04];
};
struct burnset_s {
    u8 reserved_000[0xd0];
    burnout_s burnouts[32];
    i32 active_count;
    i32 selected_index;
};
struct eduiiattr_s {};
struct eduiitem_s;
struct eduimenu_s;
struct nucamera_s;
struct nugscn_s;
struct nugspline_s;
struct nupad_s;
struct nuvec_s;
struct part_typedesc_s {};
union variptr_u;

struct EdBitControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void Refresh();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectItem(eduimenu_s *, eduiitem_s *, u32);
};
struct EdClass {
    void AddType(EdRef *);
    void CopyObject(void *, void *);
    void FindMember(EdMember *, void *, i32, i32);
    void FindObject(char *);
    void FindTypeRef(char *, i32);
    void FindTypeRef(i32, i32);
    void GetStreamClasses(EdStream &, i32 *, i32 &, i32);
    void Serialise(EdStream &, i32 *);
    void SerialiseObject(EdStream &, void *);
    void SerialiseObject(EdStream &, void *, EdClass *, EdRegistry *);
    void SerialiseObjectHeader(EdStream &, void *);
};
struct EdClassInterface {
    void DistanceToObject(VuVec &, VuVec &, void *, EdRef **);
    void DistanceToObject(VuVec &, void *, EdRef **);
    void GetNextObject(void *, i32 (*)(void *));
};
struct EdClassObjectNameControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    EdClassObjectNameControl();
    void Process(EdInputContext &);
    void Render();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectClass(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectObject(eduimenu_s *, eduiitem_s *, u32);
};
struct EdColourControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    EdColourControl();
    void Refresh();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbColourSelected(eduimenu_s *, eduiitem_s *, u32);
};
struct EdControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void Process(EdInputContext &);
    void Refresh();
    void Render();
    void SelectSubObject();
    void SetMenuItemAttr(i32, eduiitem_s *, eduiiattr_s *, eduiiattr_s *);
    void cbSelected(eduimenu_s *, eduiitem_s *, u32);
};
struct EdDefunctList {
    void ReviveAll(i32);
};
struct EdEnumControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void GetEnumString(i32);
    void GetEnumValue(char *);
    void Refresh();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectItem(eduimenu_s *, eduiitem_s *, u32);
};
struct EdFileInputStream {
    void BeginBlock(char const *);
    void Eat(i32, i32);
    void EndBlock();
    void Open(i32, i32);
    void SerialiseBuffer(void *, i32, i32);
};
struct EdFileOutputStream {
    void BeginBlock(char const *);
    void Eat(i32, i32);
    void EndBlock();
    void Open(i32, i32);
    void SerialiseBuffer(void *, i32, i32);
};
struct EdInputContext {
    void Clear(i32);
    EdInputContext();
    void Get(i32);
    void GetHold(i32);
    void GetPress(i32);
    void GetRelease(i32);
    void GetRepeat(i32);
    void Set(i32, float, float);
    void Update(nucamera_s *, nupad_s *, float, bool);
};
struct EdInputStream {
    void SerialiseString(char **);
    void SerialiseString(char **, i32);
    void SerialiseString(char *, i32);
};
struct EdManMove {
    EdManMove();
    void Process(EdInputContext &, ClassObjectList &);
    void Render(ClassObjectList &);
};
struct EdManRotate {
    EdManRotate();
    void Process(EdInputContext &, ClassObjectList &);
    void Render(ClassObjectList &);
    void RotateItem(EdInputContext &, ClassObjectList &, i32, i32);
};
struct EdManScale {
    EdManScale();
    void Process(EdInputContext &, ClassObjectList &);
    void Render(ClassObjectList &);
};
struct EdManipulator {
    void DrawAxis(VuVec &, VuMtx *);
    void DrawRotator(VuVec &);
    void GetAxisLocators(VuVec &, VuVec *, VuMtx *);
    void Process(EdInputContext &, ClassObjectList &);
    void Render(ClassObjectList &);
    void SelectAxis(EdInputContext &, VuVec &, VuVec &, VuVec &, VuMtx *);
    void SelectRotator(EdInputContext &, VuVec &, VuVec &);
};
struct EdMatrixControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void Destroy();
    EdMatrixControl();
    void Refresh();
    void SetMenuItemAttr(i32, eduiitem_s *, eduiiattr_s *, eduiiattr_s *);
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelected(eduimenu_s *, eduiitem_s *, u32);
};
struct EdOutputStream {
    void SerialiseString(char **);
    void SerialiseString(char **, i32);
    void SerialiseString(char *, i32);
};
struct EdRef {
    void CheckType(i32);
    EdRef(char *, char *, i32, i32, i32, EdControl *, i32);
    void GetAttributeData(void *, i32, i32, void *, i32);
    void GetMemberData(void *, i32, void *, i32);
    void GetMemberObject(void *);
    void GetTypeSize(i32, i32);
    void Serialise(EdStream &, i32 *);
    void SetAttributeData(void *, i32, i32, void *, i32);
    void SetMemberData(void *, i32, void *, i32, i16 *);
};
struct EdRefKnot {
    void GetMemberData(void *, i32, void *, i32);
    void SetMemberData(void *, i32, void *, i32, i16 *);
};
struct EdRefPlaceable {
    void GetMemberData(void *, i32, void *, i32);
    void SetMemberData(void *, i32, void *, i32, i16 *);
};
struct EdRefSpecialObject {
    void GetMemberData(void *, i32, void *, i32);
    void SetMemberData(void *, i32, void *, i32, i16 *);
};
struct EdRefSpline {
    void GetMemberData(void *, i32, void *, i32);
    void SetMemberData(void *, i32, void *, i32, i16 *);
};
struct EdRegistry {
    void AddMapping(char *, char *);
    void AddObjectNotifier(EdObjectNotifier *);
    void ClassIFaceProcess(EdClass *, void *, EdInputContext &);
    void ClassIFaceProcess(i32, void *, EdInputContext &);
    void ClassIFaceRender(EdClass *, void *, i32);
    void ClassIFaceRender(i32, void *, i32);
    void CreateObject(EdClassInterface *, void *, i32, i32, i32);
    void DefunctObject(EdClassInterface *, void *, i32, i32);
    void DestroyObject(EdClassInterface *, void *, i32, i32);
    void Flush();
    void GetClass(char *);
    void GetClass(i32);
    void GetClassId(EdClass *);
    void GetClassId(char *);
    void GetStreamClassMapping(EdStream &, i32 *, i32 &, i32);
    void GetType(char *);
    void GetType(i32);
    void GetTypeId(char *);
    void Initialise(variptr_u &, variptr_u &, i32, i32, i32, i32);
    void MapName(char *);
    void NotifyCreateObject(void *, EdClass *, void *, i32, i32, i32);
    void NotifyDefunctObject(void *, EdClass *, i32);
    void NotifyDestroyObject(void *, EdClass *, i32, i32);
    void NotifyReviveObject(void *, EdClass *, i32);
    void RegisterBaseTypes();
    void RegisterClass(char *, EdClassInterface *, i32);
    void RegisterType(char *, i32, void (*)(EdStream &, void *, i32));
    void Serialise(EdStream &);
    void SerialiseObjects(EdStream &, EdRegistry *);
};
struct EdSfxNameControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    EdSfxNameControl();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectSfx(eduimenu_s *, eduiitem_s *, u32);
};
struct EdSpecialObjectControl {
    EdSpecialObjectControl();
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void Process(EdInputContext &);
    void Render();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelectObject(eduimenu_s *, eduiitem_s *, u32);
};
struct EdStream {
    EdStream();
    EdStream(MemoryBuffer *);
    EdStream(MemoryBuffer *, MemoryBuffer *);
};
struct EdString {
    void Set(char const *);
    ~EdString();
};
struct EdStringControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    EdStringControl();
    void GetVal(char *, i32);
    void Refresh();
    void SetVal(char const *);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbPress(eduimenu_s *, eduiitem_s *, u32);
};
struct EdSystem {
    EdSubSystem *first_subsystem;
    EdSubSystem *last_subsystem;
    i32 subsystem_count;

    void Initalise(variptr_u &, variptr_u &, i32);
    void Process(float);
    void RegisterSubSystem(EdSubSystem *);
    void Render();
    void Reset();
};
struct EdType {
    void Serialise(EdStream &);
};
struct EdVectorControl {
    void AddMenuItem(eduimenu_s *, EdRef *, void *);
    void Destroy();
    EdVectorControl();
    void Refresh();
    void cbButton(eduimenu_s *, eduiitem_s *, u32);
    void cbChanged(eduimenu_s *, eduiitem_s *, u32);
    void cbSelected(eduimenu_s *, eduiitem_s *, u32);
};
struct EditorSettings {
    void AddMenuItems(eduimenu_s *);
    EditorSettings();
    void Serialise(EdStream &);
};
struct KnotHelper {
    void CreateObject(void *, i32, i32);
    void DestroyObject(void *, i32);
    void DistanceToObject(VuVec &, VuVec &, void *, EdRef **);
    void GetNextObject(void *);
    void GetNumObjects();
    void Process(void *, EdInputContext &);
    void Render(void *, i32);
};
struct SplineHelper {
    void AddMenuItems(eduimenu_s *);
    void ClearLevel(i32);
    void CreateObject(void *, i32, i32);
    void DestroyObject(void *, i32);
    void Find(char *);
    void Find(char *, SplineObject **, i32);
    void GetNextObject(void *);
    void GetNumObjects();
    void Initialise();
    void PostLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void PreLoadInitialisation(MemoryBuffer *, MemoryBuffer *);
    void Process(void *, EdInputContext &);
    void Render(void *, i32);
    void SerialiseObject(EdStream &, void *);
    void cbEdSplineAutoGenPoints(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSplineReGenPoints(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSplineReverseSpline(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSplineSmoothKnot(eduimenu_s *, eduiitem_s *, u32);
    void cbEdSplineSmoothSpline(eduimenu_s *, eduiitem_s *, u32);
};
struct SplineKnot {
    void Smooth();
};
struct SplineKnotList {
    void GetPoint(i32, VuVec &);
};
struct SplineObject {
    void Clone();
    void Draw(i32, i32, i32, float);
    void DropPoint(VuVec &);
    void GenBezierPoints();
    void GenLinearPoints();
    void GenPoints();
    void ReverseKnots();
    void SmoothKnots();
};
struct SplinePointBlock {
    void Draw();
    SplinePointBlock();
    SplinePointBlock(i32);
    virtual ~SplinePointBlock();
};
struct SplinePointList {
    void AddPoint(VuVec &);
    void Clear();
    void Draw();
    void GetNumPoints();
    void GetPoint(i32, VuVec &);
};
struct SplineTool {
    void Initialise(variptr_u &, variptr_u &, i32);
    void Process(EdInputContext &);
    void Render();
};

#endif // GAMEAPI_EDTOOLS_TYPES_H
