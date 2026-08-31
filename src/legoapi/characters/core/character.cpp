#include "decomp.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"

#include "legoapi/characters/core/CharacterObjectInterface.h"
#include "legoapi/characters/core/players.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/nuptrblock.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nufile/nufilepak.h"
#include "nu2api/nu3d/nucamera.h"

#include <string.h>
struct numtx_s;
struct APICHARACTERMODELLIST_s;
struct EXTRAMODEL;

extern i32 apiloadcharactermodels_nopakfile;

using ANIMREDIRECTFN = i32 (*)(char *, void *, CHARACTERANIM_s *, char *);
extern "C" void APIResetCharacterRemap(void);
static ANIMREDIRECTFN RedirectAnimFn;
static void *RedirectAnimList;
static char RedirectAnimDir[0x40];

// Forward declarations for local (static) character/gameplay helper stubs.
struct nuvec_s;
struct EDCREATURE_s;
struct APIOBJECT_s;

extern "C" {
    i16 id_WEIRDO1 = -1;
    i16 id_WEIRDO2 = -1;
    i16 id_BATMAN = -1;
    i16 id_ROBIN = -1;
    i16 id_HENCHMAN = -1;
    i16 id_BUGGY = -1;
    i16 id_GLIDEPACK = -1;
    i16 id_RADIOCAR = -1;
    i16 id_POLICECAR = -1;
    i16 id_TWOFACE = -1;
    i16 id_HARLEYQUINN = -1;
    i16 id_CATWOMAN = -1;
    i16 id_WHIP = -1;
    i16 id_PENGUIN_BOMB = -1;
    i16 id_PENGUIN_GOON = -1;
    i16 id_PENGUIN_GOON_GUN = -1;
    i16 id_JOKER_GOON = -1;
    i16 id_JOKER_GOON_GUN = -1;
    i16 id_RIDDLER_GOON = -1;
    i16 id_RIDDLER_GOON_GUN = -1;
    i16 id_JACKINABOX = -1;
    i16 id_MOWER = -1;
    i16 id_DODGEM = -1;
    i16 id_GYROCOPTER = -1;
    i16 id_CLAYFACE = -1;
    i16 id_PENGUIN = -1;
    i16 id_UMBRELLA = -1;
    i16 id_POISONIVY = -1;
    i16 id_QUIGONJINN = -1;
    i16 id_MACEWINDU = -1;
    i16 id_OBIWANKENOBI = -1;
    i16 id_PRINCESSLEIA = -1;
    i16 id_PRINCESSLEIASLAVE = -1;
    i16 id_PRINCESSLEIABOUSHH = -1;
    i16 id_CAPTAINANTILLES = -1;
    i16 id_CAPTAINTARPALS = -1;
    i16 id_IMPERIALGUARD = -1;
    i16 id_BODYGUARD = -1;
    i16 id_DARTHMAUL = -1;
    i16 id_GAMORREANGUARD = -1;
    i16 id_BATTLEDROID = -1;
    i16 id_BATTLEDROIDCOMMANDER = -1;
    i16 id_BATTLEDROIDGEONOSIAN = -1;
    i16 id_BATTLEDROIDSECURITY = -1;
    i16 id_CLONEEP3 = -1;
    i16 id_CLONEEP3SAND = -1;
    i16 id_NAFFDROID1 = -1;
    i16 id_NAFFDROID2 = -1;
    i16 id_NAFFDROID3 = -1;
    i16 id_NAFFDROID4 = -1;
    i16 id_MOUSEDROID = -1;
    i16 id_PROBEDROID = -1;
    i16 id_PKDROID = -1;
    i16 id_SNAKE = -1;
    i16 id_WOMPRAT = -1;
    i16 id_WAMPA = -1;
    i16 id_HANINCARBONITE = -1;
    i16 id_YODAGHOST = -1;
    i16 id_MOSEISLEYCITIZEN = -1;
    i16 id_CANTINAALIEN = -1;
    i16 id_CLOUDCITYCITIZEN = -1;
    i16 id_GEONOSIAN = -1;
    i16 id_BOB = -1;
    i16 id_WATTO = -1;
    i16 id_CHEWBACCA = -1;
    i16 id_WOOKIEE = -1;
    i16 id_ATST_LOWRES = -1;
    i16 id_BARMAN = -1;
    i16 id_DROIDEKA = -1;
    i16 id_SUPERBATTLEDROID = -1;
    i16 id_BOBAFETT = -1;
    i16 id_TRAININGREMOTE = -1;
    i16 id_SERVICECAR = -1;
    i16 id_DRAGBOMB = -1;
    i16 id_CLONEWALKER = -1;
    i16 id_WICKET = -1;
    i16 id_CATAPULT = -1;
    i16 id_BASKETCANNON = -1;
    i16 id_BANTHA = -1;
    i16 id_BOMARRMONK = -1;
    i16 id_DEWBACK = -1;
    i16 id_LANDSPEEDER = -1;
    i16 id_TAUNTAUN = -1;
    i16 id_SPEEDERBIKE = -1;
    i16 id_HEAVYREPEATINGCANNON = -1;
    i16 id_BIGGUN = -1;
    i16 id_TROOPERCANNON = -1;
    i16 id_MOSCANNON = -1;
    i16 id_CANNON = -1;
    i16 id_JABBA = -1;
    i16 id_BOSSNASS = -1;
    i16 id_SLAVE1 = -1;
    i16 id_YWING = -1;
    i16 id_TIEBOMBER = -1;
    i16 id_GRIEVOUS = -1;
    i16 id_TIEFIGHTERDARTH = -1;
    i16 id_TIEFIGHTER = -1;
    i16 id_GRABCONTROL = -1;
    i16 id_GRABR2CONTROL = -1;
    i16 id_GRABMACHINE = -1;
    i16 id_GRABMAGNET = -1;
    i16 id_ROBOTBASE = -1;
    i16 id_LUKESKYWALKERDAGOBAH = -1;
    i16 id_SNOWMOB = -1;
    i16 id_SNOWTROOPER = -1;
    i16 id_DEATHSTARTROOPER = -1;
    i16 id_DARTHVADER = -1;
    i16 id_THEEMPEROR = -1;
    i16 id_MILLENNIUMFALCON = -1;
    i16 id_SPEEDERBIKESNOW = -1;
    i16 id_KAADU = -1;
    i16 id_GUNGAN = -1;
    i16 id_FALUMPASET = -1;
    i16 id_STAP2 = -1;
    i16 id_JUMBOHOMINGDROID = -1;
    i16 id_JARJAR = -1;
    i16 id_PADMECLAWED = -1;
    i16 id_ANAKINPADAWAN = -1;
    i16 id_OBIWANKENOBIJEDIMASTER = -1;
    i16 id_SHAAKTI = -1;
    i16 id_LUMINARA = -1;
    i16 id_JANGOFETT = -1;
    i16 id_JEDISTARFIGHTERYELLOWEP3 = -1;
    i16 id_JEDISTARFIGHTERREDEP3 = -1;
    i16 id_RANCOR = -1;
    i16 id_OBIWANKENOBIEP3 = -1;
    i16 id_ANAKINJEDI = -1;
    i16 id_ANAKINJEDISCARRED = -1;
    i16 id_REPUBLICGUNSHIP = -1;
    i16 id_REPUBLICGUNSHIP_GREEN = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    i16 id_COUNTDOOKU = -1;
    i16 id_PALPATINE = -1;
    i16 id_KITFISTO = -1;
    i16 id_XWING = -1;
    i16 id_ROYALGUARD = -1;
    i16 id_SNOWSPEEDER = -1;
    i16 id_TIEINTERCEPTOR = -1;
    i16 id_IMPERIALSHUTTLE = -1;
    i16 id_UGNAUGHT = -1;
    i16 id_SENTRYDROID = -1;
    i16 id_KAMINOANDROID = -1;
    i16 id_STORMTROOPER = -1;
    i16 id_BEACHTROOPER = -1;
    i16 id_IMPERIALSHUTTLEPILOT = -1;
    i16 id_IMPERIALOFFICER = -1;
    i16 id_GRANDMOFFTARKIN = -1;
    i16 id_GONKDROID = -1;
    i16 id_MOONCAR = -1;
    i16 id_MAPCAR = -1;
    i16 id_WOOKIEFLYER = -1;
    i16 id_IMPERIALSPY = -1;
    i16 id_GREEDO = -1;
    i16 id_BOSSK = -1;
    i16 id_CANTINABAND = -1;
    i16 id_PITDROID = -1;
    i16 id_TOWNCAR = -1;
    i16 id_TRACTOR = -1;
    i16 id_FIRETRUCK = -1;
    i16 id_LIFEBOAT = -1;
    i16 id_LAMASU = -1;
    i16 id_TAUNWE = -1;
    i16 id_DEXTER = -1;
    i16 id_BIBFORTUNA = -1;
    i16 id_ADMIRALACKBAR = -1;
    i16 id_LOBOT = -1;
    i16 id_BESPINGUARD = -1;
    i16 id_TUSKENRAIDER = -1;
    i16 id_BUZZDROID = -1;
    i16 id_CLONEARC = -1;
    i16 id_NABOOSTARFIGHTER = -1;
    i16 id_NABOOSTARFIGHTERLIME = -1;
    i16 id_FLASHSPEEDER = -1;
    i16 id_SKELETON = -1;
    i16 id_MINIXWING = -1;
    i16 id_MINIYWING = -1;
    i16 id_MINITIEINTERCEPTOR = -1;
    i16 id_MINITIEBOMBER = -1;
    i16 id_MINIATAT = -1;
    i16 id_MINISTARDESTROYER = -1;
    i16 id_MINIROYALSTARSHIP = -1;
    i16 id_MINIIMPERIALSHUTTLE = -1;
    i16 id_MINIMILLENNIUMFALCON = -1;
    i16 id_MINIATST = -1;
    i16 id_MINIATTE = -1;
    i16 id_MINISLAVE1 = -1;
    i16 id_MINIDROIDEKA = -1;
    i16 id_MINITIEFIGHTER = -1;
    i16 id_MINITIEADVANCED = -1;
    i16 id_MINISITHINFILTRATOR = -1;
    i16 id_MINISOLARSAILOR = -1;
    i16 id_MINISANDCRAWLER = -1;
    i16 id_ANAKINSPOD = -1;
    i16 id_ANAKINSPODGREEN = -1;
    i16 id_SEBULBASPOD = -1;
    i16 id_GASGANOSPOD = -1;
    i16 id_ANOTHERMISCPOD = -1;
    i16 id_ANOTHERMISCPOD2 = -1;
    i16 id_ANAKINSNEWPOD = -1;
    i16 id_ANAKINSNEWPODGREEN = -1;
    i16 id_ANAKINSSPEEDER = -1;
    i16 id_ANAKINSSPEEDER_GREEN = -1;
    i16 id_ZAMSSPEEDER = -1;
    i16 id_VULTUREDROID = -1;
    i16 id_DROIDTRIFIGHTER = -1;
    i16 id_DROIDSTARFIGHTER = -1;

    CHARFIXUP CharFixUp[222] = {
        {"weirdo1", &id_WEIRDO1},
        {"weirdo2", &id_WEIRDO2},
        {"batman", &id_BATMAN},
        {"robin", &id_ROBIN},
        {"henchman", &id_HENCHMAN},
        {"buggy", &id_BUGGY},
        {"glidepack", &id_GLIDEPACK},
        {"radiocar", &id_RADIOCAR},
        {"policecar", &id_POLICECAR},
        {"twoface", &id_TWOFACE},
        {"harleyquinn", &id_HARLEYQUINN},
        {"catwoman", &id_CATWOMAN},
        {"whip", &id_WHIP},
        {"PenguinBomb", &id_PENGUIN_BOMB},
        {"p_goon", &id_PENGUIN_GOON},
        {"p_goon_gun", &id_PENGUIN_GOON_GUN},
        {"j_goon", &id_JOKER_GOON},
        {"j_goon_gun", &id_JOKER_GOON_GUN},
        {"r_goon", &id_RIDDLER_GOON},
        {"r_goon_gun", &id_RIDDLER_GOON_GUN},
        {"jackinabox", &id_JACKINABOX},
        {"mower", &id_MOWER},
        {"dodgem", &id_DODGEM},
        {"gyrocopter", &id_GYROCOPTER},
        {"clayface", &id_CLAYFACE},
        {"penguin", &id_PENGUIN},
        {"umbrella", &id_UMBRELLA},
        {"poisonivy", &id_POISONIVY},
        {"quigonjinn", &id_QUIGONJINN},
        {"macewindu", &id_MACEWINDU},
        {"obiwankenobi", &id_OBIWANKENOBI},
        {"princessleia", &id_PRINCESSLEIA},
        {"princessleia_slave", &id_PRINCESSLEIASLAVE},
        {"princessleia_Boushh", &id_PRINCESSLEIABOUSHH},
        {"captainantilles", &id_CAPTAINANTILLES},
        {"captaintarpals", &id_CAPTAINTARPALS},
        {"imperialguard", &id_IMPERIALGUARD},
        {"bodyguard", &id_BODYGUARD},
        {"darthmaul", &id_DARTHMAUL},
        {"gamorreanguard", &id_GAMORREANGUARD},
        {"battledroid", &id_BATTLEDROID},
        {"battledroid_commander", &id_BATTLEDROIDCOMMANDER},
        {"battledroid_geonosian", &id_BATTLEDROIDGEONOSIAN},
        {"battledroid_security", &id_BATTLEDROIDSECURITY},
        {"Clone_Ep3", &id_CLONEEP3},
        {"Clone_Ep3_Sand", &id_CLONEEP3SAND},
        {"naffdroid1", &id_NAFFDROID1},
        {"naffdroid2", &id_NAFFDROID2},
        {"naffdroid3", &id_NAFFDROID3},
        {"naffdroid4", &id_NAFFDROID4},
        {"mousedroid", &id_MOUSEDROID},
        {"probedroid", &id_PROBEDROID},
        {"pkdroid", &id_PKDROID},
        {"snake", &id_SNAKE},
        {"womprat", &id_WOMPRAT},
        {"wampa", &id_WAMPA},
        {"hanincarbonite", &id_HANINCARBONITE},
        {"yoda_ghost", &id_YODAGHOST},
        {"moseisleycitizen", &id_MOSEISLEYCITIZEN},
        {"cantinaaliens", &id_CANTINAALIEN},
        {"cloudcitycitizen", &id_CLOUDCITYCITIZEN},
        {"geonosian", &id_GEONOSIAN},
        {"watto", &id_WATTO},
        {"weirdo1", &id_WEIRDO1},
        {"weirdo2", &id_WEIRDO2},
        {"chewbacca", &id_CHEWBACCA},
        {"wookie", &id_WOOKIEE},
        {"atst_lowres", &id_ATST_LOWRES},
        {"barman", &id_BARMAN},
        {"destroyer", &id_DROIDEKA},
        {"superbattledroid", &id_SUPERBATTLEDROID},
        {"bobafett", &id_BOBAFETT},
        {"trainingremote", &id_TRAININGREMOTE},
        {"service_car", &id_SERVICECAR},
        {"dragbomb", &id_DRAGBOMB},
        {"clonewalker", &id_CLONEWALKER},
        {"wicket", &id_WICKET},
        {"catapult", &id_CATAPULT},
        {"basketcannon", &id_BASKETCANNON},
        {"bantha", &id_BANTHA},
        {"bomarrmonk", &id_BOMARRMONK},
        {"dewback", &id_DEWBACK},
        {"speeder_land", &id_LANDSPEEDER},
        {"tauntaun", &id_TAUNTAUN},
        {"speederbike", &id_SPEEDERBIKE},
        {"heavyrepeatingcannon", &id_HEAVYREPEATINGCANNON},
        {"biggun", &id_BIGGUN},
        {"troopercannon", &id_TROOPERCANNON},
        {"moscannon", &id_MOSCANNON},
        {"cannon", &id_CANNON},
        {"jabba", &id_JABBA},
        {"bossnass", &id_BOSSNASS},
        {"slave1", &id_SLAVE1},
        {"ywing", &id_YWING},
        {"tiebomber", &id_TIEBOMBER},
        {"grievous", &id_GRIEVOUS},
        {"tiefighterdarth", &id_TIEFIGHTERDARTH},
        {"tiefighter", &id_TIEFIGHTER},
        {"grabberControl", &id_GRABCONTROL},
        {"grabberr2control", &id_GRABR2CONTROL},
        {"grabber", &id_GRABMACHINE},
        {"magnet", &id_GRABMAGNET},
        {"robot_base", &id_ROBOTBASE},
        {"lukeskywalker_dagobah", &id_LUKESKYWALKERDAGOBAH},
        {"SnowMob", &id_SNOWMOB},
        {"snowtrooper", &id_SNOWTROOPER},
        {"deathstartrooper", &id_DEATHSTARTROOPER},
        {"darthvader", &id_DARTHVADER},
        {"theemperor", &id_THEEMPEROR},
        {"millenniumfalcon", &id_MILLENNIUMFALCON},
        {"speederbikesnow", &id_SPEEDERBIKESNOW},
        {"Kaadu", &id_KAADU},
        {"Gungan", &id_GUNGAN},
        {"Falumpaset", &id_FALUMPASET},
        {"Stap2", &id_STAP2},
        {"jumbohomingdroid", &id_JUMBOHOMINGDROID},
        {"JarJarBinks", &id_JARJAR},
        {"PadmeClawed", &id_PADMECLAWED},
        {"Anakin_padawan", &id_ANAKINPADAWAN},
        {"ObiWanKenobi_Jedi", &id_OBIWANKENOBIJEDIMASTER},
        {"shaakti", &id_SHAAKTI},
        {"luminara", &id_LUMINARA},
        {"JangoFett", &id_JANGOFETT},
        {"JediStarfighter_Yellow_Ep3", &id_JEDISTARFIGHTERYELLOWEP3},
        {"JediStarfighter_Red_Ep3", &id_JEDISTARFIGHTERREDEP3},
        {"Rancor", &id_RANCOR},
        {"obiwankenobi_ep3", &id_OBIWANKENOBIEP3},
        {"anakin_jedi", &id_ANAKINJEDI},
        {"anakin_jedi_scarred", &id_ANAKINJEDISCARRED},
        {"republicgunship", &id_REPUBLICGUNSHIP},
        {"republicgunship_green", &id_REPUBLICGUNSHIP_GREEN},
        {"newrepublicgunship", &id_NEW_REPUBLIC_GUNSHIP},
        {"newrepublicgunship_green", &id_NEW_REPUBLIC_GUNSHIP_GREEN},
        {"countdooku", &id_COUNTDOOKU},
        {"palpatine", &id_PALPATINE},
        {"kitfisto", &id_KITFISTO},
        {"xwing", &id_XWING},
        {"royalguard", &id_ROYALGUARD},
        {"snowspeeder", &id_SNOWSPEEDER},
        {"tieinterceptor", &id_TIEINTERCEPTOR},
        {"imperialshuttle", &id_IMPERIALSHUTTLE},
        {"ugnaught", &id_UGNAUGHT},
        {"sentrydroid", &id_SENTRYDROID},
        {"kaminoandroid", &id_KAMINOANDROID},
        {"stormtrooper", &id_STORMTROOPER},
        {"beachtrooper", &id_BEACHTROOPER},
        {"ImperialShuttlePilot", &id_IMPERIALSHUTTLEPILOT},
        {"imperialofficer", &id_IMPERIALOFFICER},
        {"grandmofftarkin", &id_GRANDMOFFTARKIN},
        {"gonkdroid", &id_GONKDROID},
        {"mooncar", &id_MOONCAR},
        {"mapcar", &id_MAPCAR},
        {"wookieflyer", &id_WOOKIEFLYER},
        {"imperialspy", &id_IMPERIALSPY},
        {"greedo", &id_GREEDO},
        {"bossk", &id_BOSSK},
        {"cantinaband", &id_CANTINABAND},
        {"pitdroid", &id_PITDROID},
        {"TownCar", &id_TOWNCAR},
        {"Tractor", &id_TRACTOR},
        {"fireTruck", &id_FIRETRUCK},
        {"lifeBoat", &id_LIFEBOAT},
        {"lamasu", &id_LAMASU},
        {"taunwe", &id_TAUNWE},
        {"dexter", &id_DEXTER},
        {"bibfortuna", &id_BIBFORTUNA},
        {"admiralackbar", &id_ADMIRALACKBAR},
        {"lobot", &id_LOBOT},
        {"bespinguard", &id_BESPINGUARD},
        {"tuskenraider", &id_TUSKENRAIDER},
        {"buzzdroid", &id_BUZZDROID},
        {"clonearc", &id_CLONEARC},
        {"naboostarfighter", &id_NABOOSTARFIGHTER},
        {"naboostarfighter_lime", &id_NABOOSTARFIGHTERLIME},
        {"flashspeeder", &id_FLASHSPEEDER},
        {"skeleton", &id_SKELETON},
        {"mini_x_wing", &id_MINIXWING},
        {"mini_y_wing", &id_MINIYWING},
        {"mini_tie_interceptor", &id_MINITIEINTERCEPTOR},
        {"mini_tie_bomber", &id_MINITIEBOMBER},
        {"mini_atat", &id_MINIATAT},
        {"mini_star_destroyer", &id_MINISTARDESTROYER},
        {"mini_royal_stardestroyer", &id_MINIROYALSTARSHIP},
        {"mini_imperial_shuttle", &id_MINIIMPERIALSHUTTLE},
        {"Mini_Millennium_Falcon", &id_MINIMILLENNIUMFALCON},
        {"mini_atst", &id_MINIATST},
        {"mini_atte", &id_MINIATTE},
        {"mini_slave1", &id_MINISLAVE1},
        {"mini_droideka", &id_MINIDROIDEKA},
        {"mini_tie_fighter", &id_MINITIEFIGHTER},
        {"mini_tie_advanced", &id_MINITIEADVANCED},
        {"mini_sith_infiltrator", &id_MINISITHINFILTRATOR},
        {"mini_solar_sailer", &id_MINISOLARSAILOR},
        {"mini_sand_crawler", &id_MINISANDCRAWLER},
        {"anakinspod", &id_ANAKINSPOD},
        {"anakinspod_green", &id_ANAKINSPODGREEN},
        {"sebulbaspod", &id_SEBULBASPOD},
        {"gasganospod", &id_GASGANOSPOD},
        {"anothermiscpod", &id_ANOTHERMISCPOD},
        {"anothermiscpod2", &id_ANOTHERMISCPOD2},
        {"newanakinspod", &id_ANAKINSNEWPOD},
        {"newanakinspod_green", &id_ANAKINSNEWPODGREEN},
        {"anakinsspeeder", &id_ANAKINSSPEEDER},
        {"anakinsspeeder_green", &id_ANAKINSSPEEDER_GREEN},
        {"ZamsSpeeder", &id_ZAMSSPEEDER},
        {"vulturedroid", &id_VULTUREDROID},
        {"droidtrifighter", &id_DROIDTRIFIGHTER},
        {"droidstarfighter", &id_DROIDSTARFIGHTER},
        {NULL, NULL},
    };
}

i32 CHARCOUNT = 0;
CHARACTERDATA *CDataList = NULL;
GAMECHARACTERDATA *GCDataList = NULL;
// Original data @0x00666960.  Every character starts with the ordinary
// hierarchy layer selector; FixUpCharacters copies this record into the
// per-character table before applying character-specific overrides.
GAMECHARACTERDATA GCDATA_DEFAULT = {MakeLayerList_Index};

i32 g_loadingCharacterInHub;

i32 CharIDFromName(char *name) {
    for (i32 i = 0; i < CHARCOUNT; i++) {
        if (NuStrICmp(CDataList[i].file, name) == 0) {
            return i;
        }
    }

    return -1;
}

CHARACTERDATA *ConfigureCharacterList(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd, i32 count, i32 *countDest,
                                      i32 count2, GAMECHARACTERDATA **dataList) {
    bool bVar1;
    bool bVar2;
    nufpar_s *fp;
    CHARACTERDATA *characterdata;
    i16 dirnameOffsets[500];
    i16 filenameOffsets[500];
    char buf[10000];
    CHARACTERDATA *cdatas;
    i32 j;
    usize offset;
    i32 i;
    CHARACTERDATA *cdata;

    fp = NuFParCreate(file);
    if (500 < count) {
        count = 500;
    }
    bufferStart->void_ptr = (void *)ALIGN(bufferStart->addr, 4);
    characterdata = (CHARACTERDATA *)bufferStart->void_ptr;
    i = 0;

    memset(buf, 0, 10000);

    buf[0] = '\0';
    offset = 0;
    bVar2 = false;
    cdata = characterdata;
    while (NuFParGetLine(fp) != 0) {
        NuFParGetWord(fp);
        if (*fp->word_buf != '\0') {
            if (bVar2) {
                if (NuStrICmp(fp->word_buf, "char_end") == 0) {
                    bVar2 = false;
                    if ((dirnameOffsets[i] != -1) && (filenameOffsets[i] != -1)) {
                        i = i + 1;
                        cdata = cdata + 1;
                    }
                } else if (NuStrICmp(fp->word_buf, "dir") == 0 && NuFParGetWord(fp) != 0) {
                    i32 len = NuStrLen(fp->word_buf);
                    if ((len + offset + 1) < 10000) {
                        NuStrCpy(buf + offset, fp->word_buf);
                        dirnameOffsets[i] = (i16)offset;
                        offset = offset + len + 1;
                    }
                } else if (NuStrICmp(fp->word_buf, "file") == 0 && NuFParGetWord(fp) != 0) {
                    i32 len = NuStrLen(fp->word_buf);
                    if ((len + offset + 1) < 10000) {
                        NuStrCpy(buf + offset, fp->word_buf);
                        filenameOffsets[i] = (i16)offset;
                        offset = offset + len + 1;
                    }
                }
            } else {
                if (NuStrICmp(fp->word_buf, "char_start") == 0 && i < count) {
                    bVar1 = true;
                } else {
                    bVar1 = false;
                }

                if (bVar1) {
                    bVar2 = true;
                    dirnameOffsets[i] = -1;
                    filenameOffsets[i] = -1;
                    cdata->field0_0x0 = -1;
                    cdata->model_flags = 0;
                    cdata->dir = (char *)0x0;
                    cdata->file = (char *)0x0;
                    cdata->animations = NULL;
                    cdata->field5_0x14 = 0;
                    cdata->move_fn = NULL;
                    cdata->animate_fn = NULL;
                    cdata->draw_fn = NULL;
                    cdata->field11_0x24 = 0;
                    cdata->field12_0x28 = 0;
                    cdata->field13_0x2c = 1.0f;
                    cdata->field14_0x30 = 0.5f;
                    cdata->field15_0x34 = -0.5f;
                    cdata->field16_0x38 = 0.5f;
                    cdata->field17_0x3c = 1.0f;
                    cdata->flags = cdata->flags & 0xfe;
                    cdata->field20_0x42 = -1;
                    cdata->field21_0x44 = 0;
                    cdata->field22_0x48 = 0;
                }
            }
        }
    }
    NuFParDestroy(fp);
    if (i < 1) {
        characterdata = NULL;
    } else {
        bufferStart->void_ptr = cdata;
        memmove(bufferStart->void_ptr, buf, offset);
        for (j = 0; j < i; j = j + 1) {
            characterdata[j].dir = (char *)((i32)dirnameOffsets[j] + (usize)bufferStart->void_ptr);
            characterdata[j].file = (char *)((i32)filenameOffsets[j] + (usize)bufferStart->void_ptr);
        }
        bufferStart->void_ptr = (void *)((usize)bufferStart->void_ptr + offset);
        bufferStart->void_ptr = (void *)ALIGN((usize)bufferStart->void_ptr, 4);
        if (0 < count2) {
            if (dataList != NULL) {
                *dataList = (gamecharacterdata_s *)bufferStart->void_ptr;
            }
            for (j = 0; j < i; j = j + 1) {
                characterdata[j].field11_0x24 = bufferStart->void_ptr;
                bufferStart->void_ptr = (void *)((usize)bufferStart->void_ptr + count2);
            }
        }
        bufferStart->void_ptr = (void *)ALIGN((usize)bufferStart->void_ptr, 4);
        if (countDest != (i32 *)0x0) {
            *countDest = i;
        }
    }
    return characterdata;
}

CharacterObjectInterface::CharacterObjectInterface(GameObject_s &) {
}

void CharacterObjectInterface::GetHeight() const {
}

void CharacterObjectInterface::GetPos(VuVec &, i32) const {
}

void CharacterObjectInterface::GetRadius() const {
}

void CharacterObjectInterface::GetTargetName() const {
}

void CharacterObjectInterface::IsDead() {
}

void CharacterObjectInterface::TargetedFlash() {
}

CharacterObjectInterface::~CharacterObjectInterface() {
}

static __used__ void NewCharacterIdle(GameObject_s *, i32) {
}
static __used__ char *LevelCharacterName(u8) {
    return nullptr;
}
static __used__ i32 LevelCharacterTypeID(char *) {
    return 0;
}
static __used__ i32 LevelCharacterGlobalID(u8) {
    return 0;
}

// Static character/global-character and gameplay helpers. Stubbed to satisfy
// the symbol baseline.

static __used__ char *GlobalCharacterName(int) {
    return nullptr;
}

static __used__ void *GlobalCharacterHGobj(int) {
    return nullptr;
}

static __used__ void GlobalCharacterRender(nuvec_s *, i16, int, int, EDCREATURE_s *) {
}

static __used__ int GlobalCharacterTypeID(char *) {
    return 0;
}

static __used__ float GetViewRange(int) {
    return 0;
}

static __used__ float GetHearDistance(int) {
    return 0;
}

static __used__ float GetMaxViewHeight(int) {
    return 0;
}

static __used__ float GetMinViewHeight(int) {
    return 0;
}

static __used__ bool IsWearingBackPack_Game(GameObject_s *) {
    return false;
}

static __used__ void ExtraDieSfx_LSW(GameObject_s *) {
}

static __used__ void ExtraHurtSfx_LSW(GameObject_s *) {
}

static __used__ int GameAudio_CheckReverb_LSW() {
    return 0;
}

static __used__ int GameAudio_OverrideFootStep_LSW(GameObject_s *, int) {
    return 0;
}

static __used__ float GetCharacterGoalSpeed(APIOBJECT_s *) {
    return 0;
}

static __used__ bool IsAFallAnim(int) {
    return false;
}

static __used__ int IsGrabbable(GameObject_s *) {
    return 0;
}

static __used__ void ForcePushed_MoveCode(GameObject_s *) {
}

static __used__ int Fighting_WeaponInAction_Game(GameObject_s *) {
    return 0;
}

static __used__ int Fighting_WeaponOutAction_Game(GameObject_s *) {
    return 0;
}

static __used__ void GameObjectForceApart2D(APIOBJECT_s *, APIOBJECT_s *) {
}

static __used__ bool FindSlamOrigin_UseCPos(GameObject_s *) {
    return false;
}

static __used__ void DrawWeapons(GameObject_s *, int, float) {
}

static __used__ void DrawCharacterAttachments(GameObject_s *, numtx_s *) {
}

static __used__ void DrawWeapon_SetSabreObjects(GameObject_s *, int, int, int, int, int *, int *) {
}

static __used__ void CharConfig(int, char *, char *, variptr_u *, variptr_u *, int, char *, int, int, nufpcomjmp_s *) {
}

static __used__ void AddToModelList(APICHARACTERMODELLIST_s *, int *, int, int, int, EXTRAMODEL *) {
}

static void NormalizeAnimPath(char *path) {
    while (*path != '\0') {
        *path = static_cast<char>(NuToUpper(*path));
        ++path;
    }
}

static nuanimdata2_s *LoadAnimFromPAK(char *path, i32 area_animation, void *data, i32 size) {
    NormalizeAnimPath(path);
    for (i32 i = 0; i < apicharsys->loaded_animation_count; ++i) {
        if (NuStrCmp(path, apicharsys->animations[i].path) == 0) {
            return apicharsys->animations[i].animation;
        }
    }

    if (apicharsys->loaded_animation_count >= apicharsys->animation_capacity) {
        ++apicharsys->animation_load_attempts;
        return NULL;
    }

    ANIMLIST_s &entry = apicharsys->animations[apicharsys->loaded_animation_count];
    NuStrCpy(entry.path, path);
    entry.animation = static_cast<nuanimdata2_s *>(NuAnimData2LoadBuffFromPAK(data, size));
    if (entry.animation != NULL) {
        if (area_animation != 0) {
            ++apicharsys->area_animation_count;
        }
        ++apicharsys->loaded_animation_count;
        ++apicharsys->animation_load_attempts;
    }
    return entry.animation;
}

static nuanimdata2_s *LoadAnim(char *path, i32 area_animation, VARIPTR *buf, VARIPTR buf_end) {
    NormalizeAnimPath(path);
    for (i32 i = 0; i < apicharsys->loaded_animation_count; ++i) {
        if (NuStrCmp(path, apicharsys->animations[i].path) == 0) {
            return apicharsys->animations[i].animation;
        }
    }

    if (apicharsys->loaded_animation_count >= apicharsys->animation_capacity) {
        ++apicharsys->animation_load_attempts;
        return NULL;
    }

    ANIMLIST_s &entry = apicharsys->animations[apicharsys->loaded_animation_count];
    NuStrCpy(entry.path, path);
    entry.animation = static_cast<nuanimdata2_s *>(NuAnimData2LoadBuff(entry.path, buf, &buf_end));
    if (entry.animation != NULL) {
        if (area_animation != 0) {
            ++apicharsys->area_animation_count;
        }
        ++apicharsys->loaded_animation_count;
        ++apicharsys->animation_load_attempts;
    }
    return entry.animation;
}

static void GetAnimationPath(char *path, const char *default_dir, CHARACTERANIM_s *animation, const char *extension) {
    if (RedirectAnimFn == NULL || RedirectAnimFn(RedirectAnimDir, RedirectAnimList, animation, path) == 0) {
        NuStrCpy(path, default_dir);
        NuStrCat(path, animation->name);
    }
    NuStrCat(path, extension);
}

static bool ShouldLoadAnimation(const CHARACTERANIM_s &animation, i32 area_animation) {
    return (animation.flags & 0x8000) == 0 && ((animation.flags & 1) != 0) == (area_animation != 0);
}

extern "C" {

    i32 apiloadcharactermodels_append = 0;

    void APIObjectRegisterAnimRedirect(ANIMREDIRECTFN fn, void *list, char *directory) {
        RedirectAnimFn = fn;
        RedirectAnimList = list;
        if (NuStrLen(directory) <= 0x3f) {
            NuStrCpy(RedirectAnimDir, directory);
            i32 length = NuStrLen(directory);
            if (length != 0 && directory[length - 1] != '\\') {
                NuStrCat(RedirectAnimDir, "\\");
            }
        }
    }

    APICHARACTERMODEL *APICharacterLoaded(i32 character_id) {
        if (character_id == -1) {
            return NULL;
        }

        const i16 model_index = apicharsys->playermodelids[character_id];
        if (model_index == -1) {
            return NULL;
        }
        return &apicharsys->models[model_index];
    }

    void APICharacterModelReset(APICHARACTERMODEL *model) {
        model->model_id = 0;
        model->flags &= 0xfe;
        model->field_0x3 = 0;
        model->hierarchy = NULL;

        if (apicharsys->model_id_capacity != 0) {
            usize table_size = (usize)apicharsys->model_id_capacity * sizeof(void *);
            memset(model->model_data_a, 0, table_size);
            memset(model->model_data_b, 0, table_size);
            memset(model->model_data_c, 0, table_size);
        }
        memset(model->points_of_interest, 0, sizeof(model->points_of_interest));
    }

    void APICharacterSysInit(VARIPTR *buf, VARIPTR buf_end, i32 char_count, i32 model_capacity, i32 model_id_capacity,
                             i32 extra_capacity, CHARACTERDATA *cdata_list, APICHARACTERLIGHTFN set_creature_lights) {
        (void)buf_end;

        buf->addr = ALIGN(buf->addr, 0x10);
        apicharsys = (APICHARACTERSYS *)buf->void_ptr;
        buf->addr += sizeof(*apicharsys);
        memset(apicharsys, 0, sizeof(*apicharsys));

        apicharsys->character_count = char_count;
        apicharsys->model_capacity = model_capacity;
        apicharsys->model_id_capacity = model_id_capacity;
        apicharsys->animation_capacity = extra_capacity;

        if (model_capacity != 0) {
            buf->addr = ALIGN(buf->addr, 4);
            apicharsys->models = (APICHARACTERMODEL *)buf->void_ptr;
            buf->addr += (usize)model_capacity * sizeof(*apicharsys->models);
            memset(apicharsys->models, 0, (usize)model_capacity * sizeof(*apicharsys->models));

            for (i32 i = 0; i < model_capacity; i++) {
                APICHARACTERMODEL *model = &apicharsys->models[i];
                if (model_id_capacity != 0) {
                    usize table_size = (usize)model_id_capacity * sizeof(void *);

                    buf->addr = ALIGN(buf->addr, 4);
                    model->model_data_a = (void **)buf->void_ptr;
                    buf->addr += table_size;

                    buf->addr = ALIGN(buf->addr, 4);
                    model->model_data_b = (void **)buf->void_ptr;
                    buf->addr += table_size;

                    buf->addr = ALIGN(buf->addr, 4);
                    model->model_data_c = (void **)buf->void_ptr;
                    buf->addr += table_size;
                }
                APICharacterModelReset(model);
            }
        }

        if (char_count != 0) {
            buf->addr = ALIGN(buf->addr, 4);
            apicharsys->playermodelids = buf->i16_ptr;
            buf->addr += (usize)char_count * sizeof(*apicharsys->playermodelids);
            memset(apicharsys->playermodelids, 0, (usize)char_count * sizeof(*apicharsys->playermodelids));
        }

        if (extra_capacity != 0) {
            buf->addr = ALIGN(buf->addr, 4);
            apicharsys->animations = (ANIMLIST_s *)buf->void_ptr;
            buf->addr += (usize)extra_capacity * sizeof(*apicharsys->animations);
            memset(apicharsys->animations, 0, (usize)extra_capacity * sizeof(*apicharsys->animations));
        }

        apicharsys->char_data = cdata_list;
        apicharsys->set_creature_lights = set_creature_lights;
    }

    extern void RootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
    extern void RootFnY(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);
    extern void BlendRootFn(NUMTX *, void *, NUVEC *, NUVEC *, NUVEC *, f32);

    // Original @0x3d0563. This restores the ordinary hierarchy evaluation and
    // render path; DWA, locator/effect, transparency and random-shadow branches
    // remain separate pending transcriptions of their original helpers.
    i32 APIDrawCharacterModel(CHARACTERMODEL_s *model, CHARACTERDATA *, ANIMPACKET_s *animation, NUMTX *matrix, NUMTX *,
                              NUMTX *reflection_matrix, i32, NUMTX *, GameObject_s *object, u32 flags,
                              NUJOINTANIM_s *joint_overrides, i32 joint_override_count, WORLDINFO_s *, f32,
                              NUMTX *output_matrices, i32, void *) {
        drawcharactermodel_locatorsupdated = 0;
        if (model == NULL || model->hierarchy == NULL || matrix == NULL) {
            if (animation != NULL && drawcharactermodel_keepmergeaction == 0) {
                animation->frame = 0xffff;
            }
            return 0;
        }

        bool evaluate_only = false;
        if (object == NULL || (object->apiobj.field_0x1f4 & 0x200) == 0) {
            if (NuCameraClipTestExtents(&model->hierarchy->bounds_min, &model->hierarchy->bounds_max, matrix,
                                        character_farclip, 0) == 0) {
                evaluate_only = true;
            }
        }

        i16 render_indices[32];
        const i32 render_count = MakeLayerList != NULL ? MakeLayerList(model, render_indices, flags) : 0;
        if (render_count <= 0) {
            if (animation != NULL && drawcharactermodel_keepmergeaction == 0) {
                animation->frame = 0xffff;
            }
            return 0;
        }

        auto animation_at = [model](i32 index) -> ani3_animheader_s * {
            if (index < 0 || apicharsys == NULL || index >= apicharsys->model_id_capacity ||
                model->model_data_b == NULL) {
                return NULL;
            }
            return static_cast<ani3_animheader_s *>(model->model_data_b[index]);
        };
        auto animation_flags = [model](i32 index) -> u32 {
            if (index < 0 || apicharsys == NULL || index >= apicharsys->model_id_capacity ||
                model->model_data_a == NULL || model->model_data_a[index] == NULL) {
                return 0;
            }
            return static_cast<CHARACTERANIM_s *>(model->model_data_a[index])->flags;
        };

        bool evaluated = false;
        if (animation != NULL && drawcharactermodel_noani == 0 && drawcharactermodel_restpose == 0) {
            if (animation->frame != 0xffff) {
                const i32 first_index = animation->field_0x3a;
                const i32 second_index = animation->frame;
                ani3_animheader_s *first = animation_at(first_index);
                ani3_animheader_s *second = animation_at(second_index);
                if (first != NULL && second != NULL) {
                    NuHGobjEvalAnimBlend2(model->hierarchy, first, animation->time, second, animation->time,
                                          animation->field_0x44, joint_override_count, joint_overrides,
                                          output_matrices);
                    evaluated = true;
                }
            } else if (animation->blending != 0) {
                const i32 first_index = animation->blend_animation_a;
                const i32 second_index = animation->blend_animation_b;
                ani3_animheader_s *first = animation_at(first_index);
                ani3_animheader_s *second = animation_at(second_index);
                if (first != NULL && second != NULL) {
                    f32 blend =
                        animation->blend_duration != 0.0f ? animation->blend_elapsed / animation->blend_duration : 0.0f;
                    const u32 combined_flags = animation_flags(first_index) | animation_flags(second_index);
                    if ((combined_flags & 0x20) != 0) {
                        NuHGobjEvalAnimBlend2Root(model->hierarchy, first, animation->time, second, animation->time2,
                                                  blend, joint_override_count, joint_overrides, output_matrices,
                                                  BlendRootFn, object);
                    } else {
                        NuHGobjEvalAnimBlend2(model->hierarchy, first, animation->time, second, animation->time2, blend,
                                              joint_override_count, joint_overrides, output_matrices);
                    }
                    evaluated = true;
                }
            } else {
                const i32 index = animation->animation_index;
                ani3_animheader_s *selected = animation_at(index);
                if (selected != NULL) {
                    const u32 selected_flags = animation_flags(index);
                    if ((selected_flags & 0x20) != 0) {
                        NUHGOBJROOTFN root_fn = (selected_flags & 0x200) != 0 ? RootFnY : RootFn;
                        NuHGobjEvalAnim2Root(model->hierarchy, selected, animation->field_0x00, joint_override_count,
                                             joint_overrides, output_matrices, root_fn, object);
                    } else {
                        NuHGobjEvalAnim2(model->hierarchy, selected, animation->field_0x00, joint_override_count,
                                         joint_overrides, output_matrices);
                    }
                    evaluated = true;
                }
            }
        }

        if (!evaluated) {
            NuHGobjEval(model->hierarchy, joint_override_count,
                        reinterpret_cast<nuhgobjjointoverride_s *>(joint_overrides), output_matrices);
        }
        drawcharactermodel_locatorsupdated = 1;

        i32 result = 0;
        if (!evaluate_only) {
            const i32 render_flags = object == NULL || (object->apiobj.field_0x1f4 & 0x200) == 0;
            result = NuHGobjRndrMtxDwa(model->hierarchy, matrix, render_count, render_indices, output_matrices, NULL,
                                       render_flags);
            if (reflection_matrix != NULL) {
                NuHGobjRndrMtxDwa(model->hierarchy, reflection_matrix, render_count, render_indices, output_matrices,
                                  NULL, render_flags);
            }
        }

        if (animation != NULL && drawcharactermodel_keepmergeaction == 0) {
            animation->frame = 0xffff;
        }
        return result;
    }

    void APIDumpCharacterModels(i32) {
    }

    void APILoadCharacterModels(APICHARACTERMODELLIST_s *list, i32 area_animation, VARIPTR *buf, VARIPTR buf_end,
                                i32 area_models) {
        if (apiloadcharactermodels_append == 0) {
            APIResetCharacterRemap();
            for (i32 model_index = 0; model_index < apicharsys->permanent_model_count; ++model_index) {
                APICHARACTERMODEL &model = apicharsys->models[model_index];
                for (i32 animation_id = 0; animation_id < apicharsys->model_id_capacity; ++animation_id) {
                    if ((model.model_data_b[animation_id] != NULL || model.model_data_c[animation_id] != NULL) &&
                        (static_cast<CHARACTERANIM_s *>(model.model_data_a[animation_id])->flags & 1) == 0) {
                        model.model_data_a[animation_id] = NULL;
                        model.model_data_b[animation_id] = NULL;
                        model.model_data_c[animation_id] = NULL;
                    }
                }
            }
            for (i32 model_index = apicharsys->permanent_model_count; model_index < apicharsys->model_capacity;
                 ++model_index) {
                apicharsys->models[model_index].hierarchy = NULL;
            }
            apicharsys->loaded_model_count = apicharsys->permanent_model_count;
        }
        apiloadcharactermodels_append = 0;

        while (list != NULL && list->model_id != -1 && apicharsys->loaded_model_count < apicharsys->model_capacity) {
            const i32 model_id = list->model_id;
            CHARACTERDATA &character = apicharsys->char_data[model_id];

            char directory[0x40];
            NuStrCpy(directory, "chars\\");
            NuStrCat(directory, character.dir);
            NuStrCat(directory, "\\");

            APICHARACTERMODEL *model;
            bool model_loaded = false;
            if (apicharsys->playermodelids[model_id] != -1) {
                model = &apicharsys->models[apicharsys->playermodelids[model_id]];
            } else {
                model = &apicharsys->models[apicharsys->loaded_model_count];
                APICharacterModelReset(model);

                char hierarchy_path[0x200];
                char directory_pack_path[0x100];
                char model_pack_path[0x100];
                NuStrCpy(hierarchy_path, directory);
                NuStrCat(hierarchy_path, character.file);
                NuStrCat(hierarchy_path, ".ghg");
                NuStrCpy(directory_pack_path, directory);
                NuStrCat(directory_pack_path, character.dir);
                NuStrCat(directory_pack_path, ".fpk");
                NuStrCpy(model_pack_path, directory);
                NuStrCat(model_pack_path, character.file);
                NuStrCat(model_pack_path, ".fpk");

                model->hierarchy = NuGHGRead(hierarchy_path, buf, buf_end);
                if (model->hierarchy == NULL) {
                    ++list;
                    continue;
                }
                for (i32 poi = 0; poi < 16; ++poi) {
                    model->points_of_interest[poi] = NuHGobjGetPOI(model->hierarchy, poi);
                }
                model_loaded = true;

                CHARACTERANIM_s *animations = area_models != 0 && list->count != 0 ? character.animations : NULL;
                void *pak = NULL;
                if (apiloadcharactermodels_nopakfile == 0) {
                    pak = NuFilePakLoad(directory_pack_path, buf, buf_end, 0x10);
                    if (pak == NULL) {
                        pak = NuFilePakLoad(model_pack_path, buf, buf_end, 0x10);
                    }
                }

                if (pak != NULL) {
                    for (CHARACTERANIM_s *animation = animations; animation != NULL && animation->name != NULL;
                         ++animation) {
                        if (!ShouldLoadAnimation(*animation, area_animation)) {
                            continue;
                        }
                        char animation_path[0x200];
                        if ((animation->flags & 4) != 0 && model->model_data_b[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".an3");
                            i32 item = NuFilePakGetItem(pak, animation_path);
                            if (item != 0) {
                                NuFilePakSetItemRequired(pak, item, 1);
                            }
                        }
                        if ((animation->flags & 8) != 0 && model->model_data_c[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".bsa");
                            i32 item = NuFilePakGetItem(pak, animation_path);
                            if (item != 0) {
                                NuFilePakSetItemRequired(pak, item, 1);
                            }
                        }
                    }

                    buf->addr -= NuFilePakCondense(pak);
                    for (CHARACTERANIM_s *animation = animations; animation != NULL && animation->name != NULL;
                         ++animation) {
                        if (!ShouldLoadAnimation(*animation, area_animation)) {
                            continue;
                        }
                        char animation_path[0x200];
                        if ((animation->flags & 4) != 0 && model->model_data_b[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".an3");
                            i32 item = NuFilePakGetItem(pak, animation_path);
                            void *data;
                            i32 size;
                            if (item != 0 && NuFilePakGetItemInfo(pak, item, &data, &size) != 0) {
                                const bool pointer_block = static_cast<i32 *>(data)[1] > static_cast<i32>(0x414e4934);
                                ani3_animheader_s *joint_animation = reinterpret_cast<ani3_animheader_s *>(
                                    pointer_block ? static_cast<u8 *>(data) + 4 : data);
                                if ((joint_animation->field_12 & 0xff) == 0) {
                                    if (pointer_block) {
                                        NuPtrBlockFix(data);
                                    } else {
                                        NuAnimData2Fixup(size, &data);
                                    }
                                    joint_animation->field_12 |= 1;
                                }
                                model->model_data_b[animation->animation_id] = joint_animation;
                                model->model_data_a[animation->animation_id] = animation;
                            }
                        }
                        if ((animation->flags & 8) != 0 && model->model_data_c[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".bsa");
                            i32 item = NuFilePakGetItem(pak, animation_path);
                            void *data;
                            i32 size;
                            if (item != 0 && NuFilePakGetItemInfo(pak, item, &data, &size) != 0) {
                                model->model_data_c[animation->animation_id] =
                                    LoadAnimFromPAK(animation_path, area_animation, data, size);
                                if (model->model_data_c[animation->animation_id] != NULL) {
                                    model->model_data_a[animation->animation_id] = animation;
                                }
                            }
                        }
                    }
                } else {
                    for (CHARACTERANIM_s *animation = animations; animation != NULL && animation->name != NULL;
                         ++animation) {
                        if (!ShouldLoadAnimation(*animation, area_animation)) {
                            continue;
                        }
                        char animation_path[0x200];
                        if ((animation->flags & 4) != 0 && model->model_data_b[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".an3");
                            if (NuFileExists(animation_path) != 0) {
                                model->model_data_b[animation->animation_id] =
                                    LoadAnim(animation_path, area_animation, buf, buf_end);
                            }
                            if (model->model_data_b[animation->animation_id] != NULL) {
                                model->model_data_a[animation->animation_id] = animation;
                            }
                        }
                        if ((animation->flags & 8) != 0 && model->model_data_c[animation->animation_id] == NULL) {
                            GetAnimationPath(animation_path, directory, animation, ".bsa");
                            model->model_data_c[animation->animation_id] =
                                LoadAnim(animation_path, area_animation, buf, buf_end);
                            if (model->model_data_c[animation->animation_id] != NULL) {
                                model->model_data_a[animation->animation_id] = animation;
                            }
                        }
                    }
                }
            }

            if (model_loaded) {
                model->model_id = model_id;
                model->flags = (model->flags & ~1) | (area_models != 0 && list->count != 0 ? 1 : 0);
                apicharsys->playermodelids[model_id] = apicharsys->loaded_model_count;
                if (area_animation != 0) {
                    ++apicharsys->permanent_model_count;
                    character.model_flags |= 2;
                }
                ++apicharsys->loaded_model_count;
            }
            ++list;
        }
    }

    void APIResetCharacterRemap(void) {
        for (i32 i = 0; i < apicharsys->character_count; ++i) {
            if ((apicharsys->char_data[i].model_flags & 2) == 0) {
                apicharsys->playermodelids[i] = -1;
            }
        }
    }

    void APITransparentCharDraw(void) {
    }

    void APITransparentInit(void) {
    }

    void InitFn_GetCharacterGoalSpeedFn(void) {
    }

    void InitFn_GetHearDistance(void) {
    }

    void InitFn_GetViewRange(void) {
    }

    void InitFn_GlobalCharacterHGobj(void) {
    }

    void InitFn_GlobalCharacterName(void) {
    }

    void InitFn_GlobalCharacterRender(void) {
    }

    void InitFn_GlobalCharacterTypeID(void) {
    }

    void InitFn_GlobalGetMaxViewHeight(void) {
    }

    void InitFn_GlobalGetMinViewHeight(void) {
    }

    void InitFn_LevelCharacterGlobalID(void) {
    }

    void InitFn_LevelCharacterName(void) {
    }

    void InitFn_LevelCharacterTypeID(void) {
    }

} // extern "C"
