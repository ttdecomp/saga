#include "game/character.h"

#include "globals.h"
#include "nu2api.saga/nucore/nustring.h"

extern "C" {
    int16_t id_WEIRDO1 = -1;
    int16_t id_WEIRDO2 = -1;
    int16_t id_BATMAN = -1;
    int16_t id_ROBIN = -1;
    int16_t id_HENCHMAN = -1;
    int16_t id_BUGGY = -1;
    int16_t id_GLIDEPACK = -1;
    int16_t id_RADIOCAR = -1;
    int16_t id_POLICECAR = -1;
    int16_t id_TWOFACE = -1;
    int16_t id_HARLEYQUINN = -1;
    int16_t id_CATWOMAN = -1;
    int16_t id_WHIP = -1;
    int16_t id_PENGUIN_BOMB = -1;
    int16_t id_PENGUIN_GOON = -1;
    int16_t id_PENGUIN_GOON_GUN = -1;
    int16_t id_JOKER_GOON = -1;
    int16_t id_JOKER_GOON_GUN = -1;
    int16_t id_RIDDLER_GOON = -1;
    int16_t id_RIDDLER_GOON_GUN = -1;
    int16_t id_JACKINABOX = -1;
    int16_t id_MOWER = -1;
    int16_t id_DODGEM = -1;
    int16_t id_GYROCOPTER = -1;
    int16_t id_CLAYFACE = -1;
    int16_t id_PENGUIN = -1;
    int16_t id_UMBRELLA = -1;
    int16_t id_POISONIVY = -1;
    int16_t id_QUIGONJINN = -1;
    int16_t id_MACEWINDU = -1;
    int16_t id_OBIWANKENOBI = -1;
    int16_t id_PRINCESSLEIA = -1;
    int16_t id_PRINCESSLEIASLAVE = -1;
    int16_t id_PRINCESSLEIABOUSHH = -1;
    int16_t id_CAPTAINANTILLES = -1;
    int16_t id_CAPTAINTARPALS = -1;
    int16_t id_IMPERIALGUARD = -1;
    int16_t id_BODYGUARD = -1;
    int16_t id_DARTHMAUL = -1;
    int16_t id_GAMORREANGUARD = -1;
    int16_t id_BATTLEDROID = -1;
    int16_t id_BATTLEDROIDCOMMANDER = -1;
    int16_t id_BATTLEDROIDGEONOSIAN = -1;
    int16_t id_BATTLEDROIDSECURITY = -1;
    int16_t id_CLONEEP3 = -1;
    int16_t id_CLONEEP3SAND = -1;
    int16_t id_NAFFDROID1 = -1;
    int16_t id_NAFFDROID2 = -1;
    int16_t id_NAFFDROID3 = -1;
    int16_t id_NAFFDROID4 = -1;
    int16_t id_MOUSEDROID = -1;
    int16_t id_PROBEDROID = -1;
    int16_t id_PKDROID = -1;
    int16_t id_SNAKE = -1;
    int16_t id_WOMPRAT = -1;
    int16_t id_WAMPA = -1;
    int16_t id_HANINCARBONITE = -1;
    int16_t id_YODAGHOST = -1;
    int16_t id_MOSEISLEYCITIZEN = -1;
    int16_t id_CANTINAALIEN = -1;
    int16_t id_CLOUDCITYCITIZEN = -1;
    int16_t id_GEONOSIAN = -1;
    int16_t id_WATTO = -1;
    int16_t id_CHEWBACCA = -1;
    int16_t id_WOOKIEE = -1;
    int16_t id_ATST_LOWRES = -1;
    int16_t id_BARMAN = -1;
    int16_t id_DROIDEKA = -1;
    int16_t id_SUPERBATTLEDROID = -1;
    int16_t id_BOBAFETT = -1;
    int16_t id_TRAININGREMOTE = -1;
    int16_t id_SERVICECAR = -1;
    int16_t id_DRAGBOMB = -1;
    int16_t id_CLONEWALKER = -1;
    int16_t id_WICKET = -1;
    int16_t id_CATAPULT = -1;
    int16_t id_BASKETCANNON = -1;
    int16_t id_BANTHA = -1;
    int16_t id_BOMARRMONK = -1;
    int16_t id_DEWBACK = -1;
    int16_t id_LANDSPEEDER = -1;
    int16_t id_TAUNTAUN = -1;
    int16_t id_SPEEDERBIKE = -1;
    int16_t id_HEAVYREPEATINGCANNON = -1;
    int16_t id_BIGGUN = -1;
    int16_t id_TROOPERCANNON = -1;
    int16_t id_MOSCANNON = -1;
    int16_t id_CANNON = -1;
    int16_t id_JABBA = -1;
    int16_t id_BOSSNASS = -1;
    int16_t id_SLAVE1 = -1;
    int16_t id_YWING = -1;
    int16_t id_TIEBOMBER = -1;
    int16_t id_GRIEVOUS = -1;
    int16_t id_TIEFIGHTERDARTH = -1;
    int16_t id_TIEFIGHTER = -1;
    int16_t id_GRABCONTROL = -1;
    int16_t id_GRABR2CONTROL = -1;
    int16_t id_GRABMACHINE = -1;
    int16_t id_GRABMAGNET = -1;
    int16_t id_ROBOTBASE = -1;
    int16_t id_LUKESKYWALKERDAGOBAH = -1;
    int16_t id_SNOWMOB = -1;
    int16_t id_SNOWTROOPER = -1;
    int16_t id_DEATHSTARTROOPER = -1;
    int16_t id_DARTHVADER = -1;
    int16_t id_THEEMPEROR = -1;
    int16_t id_MILLENNIUMFALCON = -1;
    int16_t id_SPEEDERBIKESNOW = -1;
    int16_t id_KAADU = -1;
    int16_t id_GUNGAN = -1;
    int16_t id_FALUMPASET = -1;
    int16_t id_STAP2 = -1;
    int16_t id_JUMBOHOMINGDROID = -1;
    int16_t id_JARJAR = -1;
    int16_t id_PADMECLAWED = -1;
    int16_t id_ANAKINPADAWAN = -1;
    int16_t id_OBIWANKENOBIJEDIMASTER = -1;
    int16_t id_SHAAKTI = -1;
    int16_t id_LUMINARA = -1;
    int16_t id_JANGOFETT = -1;
    int16_t id_JEDISTARFIGHTERYELLOWEP3 = -1;
    int16_t id_JEDISTARFIGHTERREDEP3 = -1;
    int16_t id_RANCOR = -1;
    int16_t id_OBIWANKENOBIEP3 = -1;
    int16_t id_ANAKINJEDI = -1;
    int16_t id_ANAKINJEDISCARRED = -1;
    int16_t id_REPUBLICGUNSHIP = -1;
    int16_t id_REPUBLICGUNSHIP_GREEN = -1;
    int16_t id_NEW_REPUBLIC_GUNSHIP = -1;
    int16_t id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    int16_t id_COUNTDOOKU = -1;
    int16_t id_PALPATINE = -1;
    int16_t id_KITFISTO = -1;
    int16_t id_XWING = -1;
    int16_t id_ROYALGUARD = -1;
    int16_t id_SNOWSPEEDER = -1;
    int16_t id_TIEINTERCEPTOR = -1;
    int16_t id_IMPERIALSHUTTLE = -1;
    int16_t id_UGNAUGHT = -1;
    int16_t id_SENTRYDROID = -1;
    int16_t id_KAMINOANDROID = -1;
    int16_t id_STORMTROOPER = -1;
    int16_t id_BEACHTROOPER = -1;
    int16_t id_IMPERIALSHUTTLEPILOT = -1;
    int16_t id_IMPERIALOFFICER = -1;
    int16_t id_GRANDMOFFTARKIN = -1;
    int16_t id_GONKDROID = -1;
    int16_t id_MOONCAR = -1;
    int16_t id_MAPCAR = -1;
    int16_t id_WOOKIEFLYER = -1;
    int16_t id_IMPERIALSPY = -1;
    int16_t id_GREEDO = -1;
    int16_t id_BOSSK = -1;
    int16_t id_CANTINABAND = -1;
    int16_t id_PITDROID = -1;
    int16_t id_TOWNCAR = -1;
    int16_t id_TRACTOR = -1;
    int16_t id_FIRETRUCK = -1;
    int16_t id_LIFEBOAT = -1;
    int16_t id_LAMASU = -1;
    int16_t id_TAUNWE = -1;
    int16_t id_DEXTER = -1;
    int16_t id_BIBFORTUNA = -1;
    int16_t id_ADMIRALACKBAR = -1;
    int16_t id_LOBOT = -1;
    int16_t id_BESPINGUARD = -1;
    int16_t id_TUSKENRAIDER = -1;
    int16_t id_BUZZDROID = -1;
    int16_t id_CLONEARC = -1;
    int16_t id_NABOOSTARFIGHTER = -1;
    int16_t id_NABOOSTARFIGHTERLIME = -1;
    int16_t id_FLASHSPEEDER = -1;
    int16_t id_SKELETON = -1;
    int16_t id_MINIXWING = -1;
    int16_t id_MINIYWING = -1;
    int16_t id_MINITIEINTERCEPTOR = -1;
    int16_t id_MINITIEBOMBER = -1;
    int16_t id_MINIATAT = -1;
    int16_t id_MINISTARDESTROYER = -1;
    int16_t id_MINIROYALSTARSHIP = -1;
    int16_t id_MINIIMPERIALSHUTTLE = -1;
    int16_t id_MINIMILLENNIUMFALCON = -1;
    int16_t id_MINIATST = -1;
    int16_t id_MINIATTE = -1;
    int16_t id_MINISLAVE1 = -1;
    int16_t id_MINIDROIDEKA = -1;
    int16_t id_MINITIEFIGHTER = -1;
    int16_t id_MINITIEADVANCED = -1;
    int16_t id_MINISITHINFILTRATOR = -1;
    int16_t id_MINISOLARSAILOR = -1;
    int16_t id_MINISANDCRAWLER = -1;
    int16_t id_ANAKINSPOD = -1;
    int16_t id_ANAKINSPODGREEN = -1;
    int16_t id_SEBULBASPOD = -1;
    int16_t id_GASGANOSPOD = -1;
    int16_t id_ANOTHERMISCPOD = -1;
    int16_t id_ANOTHERMISCPOD2 = -1;
    int16_t id_ANAKINSNEWPOD = -1;
    int16_t id_ANAKINSNEWPODGREEN = -1;
    int16_t id_ANAKINSSPEEDER = -1;
    int16_t id_ANAKINSSPEEDER_GREEN = -1;
    int16_t id_ZAMSSPEEDER = -1;
    int16_t id_VULTUREDROID = -1;
    int16_t id_DROIDTRIFIGHTER = -1;
    int16_t id_DROIDSTARFIGHTER = -1;

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

int32_t CHARCOUNT = 0;

CHARACTERDATA *CDataList = NULL;

int32_t CharIDFromName(char *name) {
    int iVar1;
    int offset;
    int i;

    if (0 < CHARCOUNT) {
        offset = 0;
        i = 0;
        do {
            iVar1 = NuStrICmp(*(char **)((int)CDataList->names + offset), name);
            if (iVar1 == 0) {
                return i;
            }
            i = i + 1;
            offset = offset + 0x4c;
        } while (i < CHARCOUNT);
    }
    return -1;
}