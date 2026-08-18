#include "decomp.h"
struct nugspline_s;
struct nuvec_s;
struct SOCK;
struct SOCKROT;
struct SOCKPOSITION_s;
struct SOCKSYS;

static __used__ void SockRailAngles(SOCK *, nugspline_s *, SOCKROT *) {
}

static __used__ void SockSysPointAlongMID(SOCK *, SOCKPOSITION_s *, nuvec_s *) {
}

static __used__ void SockEdgeAnglesXY(nuvec_s *, nuvec_s *, u16 *, u16 *) {
}

static __used__ float BestSockPosition(struct SOCKSYS *, nuvec_s *, SOCKPOSITION_s *, int, int) {
    return {};
}

static __used__ void FillSockPosition(struct SOCKSYS *, SOCKPOSITION_s *) {
}

void TurnOffAllSocksExcept(struct SOCKSYS *, i32) {
}

void RestoreLastSocksTurnoff(struct SOCKSYS *) {
}
