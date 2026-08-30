#ifdef __cplusplus

struct WORLDINFO_s;

void InitGameBeforeConfig(void);
void InitGameAfterConfig(void);

void CompleteLevel(struct WORLDINFO_s *world);
void StoreLevelProgress(struct WORLDINFO_s *world);
void NewAreaMusicChanges(void);
void Game_WorldInfo_InitLast(struct WORLDINFO_s *world);
void Game_WorldInfo_InitMenu(struct WORLDINFO_s *world, int *menu_id, int *menu_y);

#endif
