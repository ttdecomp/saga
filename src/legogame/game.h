#ifdef __cplusplus

struct WORLDINFO_s;

void InitGameBeforeConfig(void);
void InitGameAfterConfig(void);

void CompleteLevel(struct WORLDINFO_s *world);
void StoreLevelProgress(struct WORLDINFO_s *world);
void NewAreaMusicChanges(void);

#endif
