#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

struct STREAK_s {
    STREAK_s *next;
    STREAK_s *prev;
    nuvec_s position;
    nuvec_s previous_position;
    nuvec_s start_tangent;
    nuvec_s end_tangent;
    nuvec_s positions[7];
    nuvec_s tangents[7];
    i32 segment_count;
    f32 remaining_time;
};

struct STREAKHDR_s {
    STREAKHDR_s *next;
    STREAKHDR_s *prev;
    STREAK_s *streaks;
    u16 index;
    u8 flags;
    u8 field_0xf;
    i32 has_new_streak;
    STREAKHDR_s **owner_slot;
    u32 colour;
};

DECOMP_ASSERT(sizeof(STREAK_s) == 0xe8, "STREAK_s size");
DECOMP_ASSERT(sizeof(STREAKHDR_s) == 0x1c, "STREAKHDR_s size");

static STREAKHDR_s streakhdrs[32];
static STREAKHDR_s *streakhdrs_free;
static STREAKHDR_s *streakhdrs_used;
static STREAK_s streaks[128];
static STREAK_s *streaks_free;
static STREAK_s *streaks_used;

static inline void UnlinkStreak(STREAK_s **head, STREAK_s *streak) {
    if (streak->prev != NULL) {
        streak->prev->next = streak->next;
    } else {
        *head = streak->next;
    }

    if (streak->next != NULL) {
        streak->next->prev = streak->prev;
    }
}

static inline void UnlinkStreakHeader(STREAKHDR_s **head, STREAKHDR_s *header) {
    if (header->prev != NULL) {
        header->prev->next = header->next;
    } else {
        *head = header->next;
    }

    if (header->next != NULL) {
        header->next->prev = header->prev;
    }
}

void InitStreaks(variptr_u *, variptr_u, char *) {
    for (i32 i = 0; i < 32; i++) {
        streakhdrs[i].index = i;
        streakhdrs[i].next = &streakhdrs[i + 1];
        streakhdrs[i].prev = &streakhdrs[i - 1];
    }

    streakhdrs_free = streakhdrs;
    streakhdrs_used = NULL;

    for (i32 i = 0; i < 128; i++) {
        streaks[i].next = &streaks[i + 1];
        streaks[i].prev = &streaks[i - 1];
    }

    streaks_free = streaks;
    streaks_used = NULL;
}

void ResetStreaks() {
    STREAKHDR_s *used_headers = streakhdrs_used;
    STREAKHDR_s *header = used_headers;

    if (header == NULL) {
        return;
    }

    STREAKHDR_s *free_headers = streakhdrs_free;
    STREAK_s *free_streaks = streaks_free;

    do {
        while (header->streaks != NULL) {
            STREAK_s *streak = header->streaks;
            UnlinkStreak(&header->streaks, streak);
            streak->next = free_streaks;
            free_streaks = streak;
        }

        if (header->owner_slot != NULL) {
            if (*header->owner_slot == header) {
                *header->owner_slot = NULL;
            } else {
                header->owner_slot = NULL;
            }
        }

        UnlinkStreakHeader(&used_headers, header);
        header->next = free_headers;
        free_headers = header;
        header = used_headers;
    } while (header != NULL);

    streaks_free = free_streaks;
    streakhdrs_used = NULL;
    streakhdrs_free = free_headers;
}

void UpdateStreaks(float) {
}

void AddStreakPoints(nuvec_s *, float, u32, void **, i32, void *) {
}
