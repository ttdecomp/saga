#include "decomp.h"
struct EDLOCATOR_s;
struct EDLOCATORSET_s;
struct nuvec_s;

struct eduimenu_s;
struct eduiitem_s;

static __used__ void locatorEditor_cbDeleteLocator(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbRenameLocator(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbSetLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbEmptyLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbCreateLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbDeleteLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbRenameLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbSelectLocatorSet(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbAddLocatorsByName(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbRenameLocatorMenu(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbRenameLocatorSetMenu(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbAddLocatorsByNameMenu(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbAddLocatorsByNameYesNo(eduimenu_s *, eduiitem_s *, u32) { }
static __used__ void locatorEditor_cbCancelRenameMenu(eduimenu_s *, eduimenu_s *) { }
static __used__ void locatorEditor_cbCancelSelectLocatorSet(eduimenu_s *, eduimenu_s *) { }
static __used__ void locatorEditor_cbCancelDeleteLocatorMenu(eduimenu_s *, eduimenu_s *) { }
static __used__ void locatorEditor_cbCancelRenameLocatorSetMenu(eduimenu_s *, eduimenu_s *) { }

static __used__ void * CreateCreature(int, nuvec_s*, int) { return {}; }

static __used__ void DestroyLocator(EDLOCATOR_s*) {}

static __used__ unsigned int AddLocatorToSet(EDLOCATORSET_s*, EDLOCATOR_s*, EDLOCATOR_s*) { return {}; }
