#include "ActorTemplates.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"

#include <time.h>

namespace ActorTemplates
{
    typedef TiXmlElement* (*PickupCreationFunction)(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);

    struct PickupCreationTable
    {
        PickupType pickupType;
        PickupCreationFunction creationFunction;
    };

    // Need to forward declare this
    TiXmlElement* CreateXmlData_TreasurePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_AmmoPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_PowerupPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_HealthPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_LifePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_WarpPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);
    TiXmlElement* CreateXmlData_MappiecePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap);

    PickupCreationTable g_PickupCreationTable[] =
    {
        { PickupType_Default,                   &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Goldbars,         &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Rings_Red,        &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Rings_Green,      &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Rings_Blue,       &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Rings_Purple,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Necklace,         &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crosses_Red,      &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crosses_Green,    &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crosses_Blue,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crosses_Purple,   &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Scepters_Red,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Scepters_Green,   &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Scepters_Blue,    &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Scepters_Purple,  &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Geckos_Red,       &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Geckos_Green,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Geckos_Blue,      &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Geckos_Purple,    &CreateXmlData_TreasurePickupActor },
        { PickupType_Ammo_Deathbag,             &CreateXmlData_AmmoPickupActor },
        { PickupType_Ammo_Shot,                 &CreateXmlData_AmmoPickupActor },
        { PickupType_Ammo_Shotbag,              &CreateXmlData_AmmoPickupActor },
        { PickupType_Powerup_Catnip_1,          &CreateXmlData_PowerupPickupActor },
        { PickupType_Powerup_Catnip_2,          &CreateXmlData_PowerupPickupActor },
        { PickupType_Health_Breadwater,         &CreateXmlData_HealthPickupActor },
        { PickupType_Health_25,                 &CreateXmlData_HealthPickupActor },
        { PickupType_Health_10,                 &CreateXmlData_HealthPickupActor },
        { PickupType_Health_15,                 &CreateXmlData_HealthPickupActor },
        { PickupType_Ammo_Magic_5,              &CreateXmlData_AmmoPickupActor },
        { PickupType_Ammo_Magic_10,             &CreateXmlData_AmmoPickupActor },
        { PickupType_Ammo_Magic_25,             &CreateXmlData_AmmoPickupActor },
        { PickupType_Mappiece,                  &CreateXmlData_MappiecePickupActor },
        { PickupType_Warp,                      &CreateXmlData_WarpPickupActor },
        { PickupType_Treasure_Coins,            &CreateXmlData_TreasurePickupActor },
        { PickupType_Ammo_Dynamite,             &CreateXmlData_AmmoPickupActor },
        { PickupType_Curse_Ammo,                NULL },
        { PickupType_Curse_Magic,               NULL },
        { PickupType_Curse_Health,              NULL },
        { PickupType_Curse_Death,               NULL },
        { PickupType_Curse_Treasure,            NULL },
        { PickupType_Curse_Freeze,              NULL },
        { PickupType_Treasure_Chalices_Red,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Chalices_Green,   &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Chalices_Blue,    &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Chalices_Purple,  &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crowns_Red,       &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crowns_Green,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crowns_Blue,      &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Crowns_Purple,    &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Skull_Red,        &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Skull_Green,      &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Skull_Blue,       &CreateXmlData_TreasurePickupActor },
        { PickupType_Treasure_Skull_Purple,     &CreateXmlData_TreasurePickupActor },
        { PickupType_Powerup_Invisibility,      &CreateXmlData_PowerupPickupActor },
        { PickupType_Powerup_Invincibility,     &CreateXmlData_PowerupPickupActor },
        { PickupType_Powerup_Life,              &CreateXmlData_LifePickupActor },
        { PickupType_Powerup_FireSword,         &CreateXmlData_PowerupPickupActor },
        { PickupType_Powerup_LightningSword,    &CreateXmlData_PowerupPickupActor },
        { PickupType_Powerup_FrostSword,        &CreateXmlData_PowerupPickupActor },
        { PickupType_BossWarp,                  &CreateXmlData_WarpPickupActor },
        { PickupType_Level2_Gem,                &CreateXmlData_MappiecePickupActor },
        { PickupType_Max,                       NULL },
    };

    PickupType StringToEnum_ImageSetToPickupType(const std::string& pickupImageSet)
    {
        static const std::map<std::string, PickupType> s_ImageSetToPickupTypeMap =
        {
            { "GAME_TREASURE_COINS", PickupType_Default },
            { "GAME_TREASURE_GOLDBARS", PickupType_Treasure_Goldbars },
            { "GAME_TREASURE_RINGS_RED", PickupType_Treasure_Rings_Red },
            { "GAME_TREASURE_RINGS_GREEN", PickupType_Treasure_Rings_Green },
            { "GAME_TREASURE_RINGS_BLUE", PickupType_Treasure_Rings_Blue },
            { "GAME_TREASURE_RINGS_PURPLE", PickupType_Treasure_Rings_Purple },
            { "GAME_TREASURE_NECKLACE", PickupType_Treasure_Necklace },
            { "GAME_TREASURE_CROSSES_RED", PickupType_Treasure_Crosses_Red },
            { "GAME_TREASURE_CROSSES_GREEN", PickupType_Treasure_Crosses_Green },
            { "GAME_TREASURE_CROSSES_BLUE", PickupType_Treasure_Crosses_Blue },
            { "GAME_TREASURE_CROSSES_PURPLE", PickupType_Treasure_Crosses_Purple },
            { "GAME_TREASURE_SCEPTERS_RED", PickupType_Treasure_Scepters_Red },
            { "GAME_TREASURE_SCEPTERS_GREEN", PickupType_Treasure_Scepters_Green },
            { "GAME_TREASURE_SCEPTERS_BLUE", PickupType_Treasure_Scepters_Blue },
            { "GAME_TREASURE_SCEPTERS_PURPLE", PickupType_Treasure_Scepters_Purple },
            { "GAME_TREASURE_GECKOS_RED", PickupType_Treasure_Geckos_Red },
            { "GAME_TREASURE_GECKOS_GREEN", PickupType_Treasure_Geckos_Green },
            { "GAME_TREASURE_GECKOS_BLUE", PickupType_Treasure_Geckos_Blue },
            { "GAME_TREASURE_GECKOS_PURPLE", PickupType_Treasure_Geckos_Purple },
            { "GAME_AMMO_DEATHBAG", PickupType_Ammo_Deathbag },
            { "GAME_AMMO_SHOT", PickupType_Ammo_Shot },
            { "GAME_AMMO_SHOTBAG", PickupType_Ammo_Shotbag },
            { "GAME_CATNIPS_NIP1", PickupType_Powerup_Catnip_1 },
            { "GAME_CATNIPS_NIP2", PickupType_Powerup_Catnip_2 },
            { "LEVEL_HEALTH", PickupType_Health_Breadwater },
            { "GAME_HEALTH_BREADWATER", PickupType_Health_Breadwater },
            { "GAME_HEALTH_POTION3", PickupType_Health_25 },
            { "GAME_HEALTH_POTION1", PickupType_Health_10 },
            { "GAME_HEALTH_POTION2", PickupType_Health_15 },
            { "GAME_MAGIC_GLOW", PickupType_Ammo_Magic_5 },
            { "GAME_MAGIC_STARGLOW", PickupType_Ammo_Magic_10 },
            { "GAME_MAGIC_MAGICCLAW", PickupType_Ammo_Magic_25 },
            { "GAME_MAPPIECE", PickupType_Mappiece },
            { "GAME_WARP", PickupType_Warp },
            { "GAME_VERTWARP", PickupType_Warp },
            { "GAME_TREASURE_COINS", PickupType_Treasure_Coins },
            { "GAME_DYNAMITE", PickupType_Ammo_Dynamite },
            { "GAME_CURSES_AMMO", PickupType_Curse_Ammo },
            { "GAME_CURSES_MAGIC", PickupType_Curse_Magic },
            { "GAME_CURSES_HEALTH", PickupType_Curse_Health },
            { "GAME_CURSES_DEATJ", PickupType_Curse_Death },
            { "GAME_CURSES_TREASURE", PickupType_Curse_Treasure },
            { "GAME_CURSES_FREEZE", PickupType_Curse_Freeze },
            { "GAME_TREASURE_CHALICES_RED", PickupType_Treasure_Chalices_Red },
            { "GAME_TREASURE_CHALICES_GREEN", PickupType_Treasure_Chalices_Green },
            { "GAME_TREASURE_CHALICES_BLUE", PickupType_Treasure_Chalices_Blue },
            { "GAME_TREASURE_CHALICES_PURPLE", PickupType_Treasure_Chalices_Purple },
            { "GAME_TREASURE_CROWNS_RED", PickupType_Treasure_Crowns_Red },
            { "GAME_TREASURE_CROWNS_GREEN", PickupType_Treasure_Crowns_Green },
            { "GAME_TREASURE_CROWNS_BLUE", PickupType_Treasure_Crowns_Blue },
            { "GAME_TREASURE_CROWNS_PURPLE", PickupType_Treasure_Crowns_Purple },
            { "GAME_TREASURE_JEWELEDSKULL_RED", PickupType_Treasure_Skull_Red },
            { "GAME_TREASURE_JEWELEDSKULL_GREEN", PickupType_Treasure_Skull_Green },
            { "GAME_TREASURE_JEWELEDSKULL_BLUE", PickupType_Treasure_Skull_Blue },
            { "GAME_TREASURE_JEWELEDSKULL_PURPLE", PickupType_Treasure_Skull_Purple },
            { "GAME_POWERUPS_GHOST", PickupType_Powerup_Invisibility },
            { "GAME_POWERUPS_INVULNERABLE", PickupType_Powerup_Invincibility },
            { "GAME_POWERUPS_EXTRALIFE", PickupType_Powerup_Life },
            { "GAME_POWERUPS_LIGHTNINGSWORD", PickupType_Powerup_LightningSword },
            { "GAME_POWERUPS_FIRESWORD", PickupType_Powerup_FireSword }, 
            { "GAME_POWERUPS_ICESWORD", PickupType_Powerup_FrostSword },
            { "GAME_BOSSWARP", PickupType_BossWarp },
        };

        auto findIt = s_ImageSetToPickupTypeMap.find(pickupImageSet);
        if (findIt == s_ImageSetToPickupTypeMap.end())
        {
            LOG_ERROR("Could not find requested enum. Offending key: " + pickupImageSet);
            assert(false);
        }

        return findIt->second;
    }

    std::string EnumToString_PickupTypeToImageSet(PickupType pickupType)
    {
        static const std::map<PickupType, std::string> s_PickupTypeToImageSetMap =
        {
            { PickupType_Default,                   "GAME_TREASURE_COINS" },
            { PickupType_Treasure_Goldbars,         "GAME_TREASURE_GOLDBARS" },
            { PickupType_Treasure_Rings_Red,        "GAME_TREASURE_RINGS_RED" },
            { PickupType_Treasure_Rings_Green,      "GAME_TREASURE_RINGS_GREEN" },
            { PickupType_Treasure_Rings_Blue,       "GAME_TREASURE_RINGS_BLUE" },
            { PickupType_Treasure_Rings_Purple,     "GAME_TREASURE_RINGS_PURPLE" },
            { PickupType_Treasure_Necklace,         "GAME_TREASURE_NECKLACE" },
            { PickupType_Treasure_Crosses_Red,      "GAME_TREASURE_CROSSES_RED" },
            { PickupType_Treasure_Crosses_Green,    "GAME_TREASURE_CROSSES_GREEN" },
            { PickupType_Treasure_Crosses_Blue,     "GAME_TREASURE_CROSSES_BLUE" },
            { PickupType_Treasure_Crosses_Purple,   "GAME_TREASURE_CROSSES_PURPLE" },
            { PickupType_Treasure_Scepters_Red,     "GAME_TREASURE_SCEPTERS_RED" },
            { PickupType_Treasure_Scepters_Green,   "GAME_TREASURE_SCEPTERS_GREEN" },
            { PickupType_Treasure_Scepters_Blue,    "GAME_TREASURE_SCEPTERS_BLUE" },
            { PickupType_Treasure_Scepters_Purple,  "GAME_TREASURE_SCEPTERS_PURPLE" },
            { PickupType_Treasure_Geckos_Red,       "GAME_TREASURE_GECKOS_RED" },
            { PickupType_Treasure_Geckos_Green,     "GAME_TREASURE_GECKOS_GREEN" },
            { PickupType_Treasure_Geckos_Blue,      "GAME_TREASURE_GECKOS_BLUE" },
            { PickupType_Treasure_Geckos_Purple,    "GAME_TREASURE_GECKOS_PURPLE" },
            { PickupType_Ammo_Deathbag,             "GAME_AMMO_DEATHBAG" },
            { PickupType_Ammo_Shot,                 "GAME_AMMO_SHOT" },
            { PickupType_Ammo_Shotbag,              "GAME_AMMO_SHOTBAG" },
            { PickupType_Powerup_Catnip_1,          "GAME_CATNIPS_NIP1" },
            { PickupType_Powerup_Catnip_2,          "GAME_CATNIPS_NIP2" },
            { PickupType_Health_Breadwater,         "LEVEL_HEALTH" },
            { PickupType_Health_25,                 "GAME_HEALTH_POTION3" },
            { PickupType_Health_10,                 "GAME_HEALTH_POTION1" },
            { PickupType_Health_15,                 "GAME_HEALTH_POTION2" },
            { PickupType_Ammo_Magic_5,              "GAME_MAGIC_GLOW" },
            { PickupType_Ammo_Magic_10,             "GAME_MAGIC_STARGLOW" },
            { PickupType_Ammo_Magic_25,             "GAME_MAGIC_MAGICCLAW" },
            { PickupType_Mappiece,                  "GAME_MAPPIECE" },
            { PickupType_Warp,                      "GAME_WARP" },
            { PickupType_Treasure_Coins,            "GAME_TREASURE_COINS" },
            { PickupType_Ammo_Dynamite,             "GAME_DYNAMITE" },
            { PickupType_Curse_Ammo,                "GAME_CURSES_AMMO" },
            { PickupType_Curse_Magic,               "GAME_CURSES_MAGIC" },
            { PickupType_Curse_Health,              "GAME_CURSES_HEALTH" },
            { PickupType_Curse_Death,               "GAME_CURSES_DEATJ" },
            { PickupType_Curse_Treasure,            "GAME_CURSES_TREASURE" },
            { PickupType_Curse_Freeze,              "GAME_CURSES_FREEZE" },
            { PickupType_Treasure_Chalices_Red,     "GAME_TREASURE_CHALICES_RED" },
            { PickupType_Treasure_Chalices_Green,   "GAME_TREASURE_CHALICES_GREEN" },
            { PickupType_Treasure_Chalices_Blue,    "GAME_TREASURE_CHALICES_BLUE" },
            { PickupType_Treasure_Chalices_Purple,  "GAME_TREASURE_CHALICES_PURPLE" },
            { PickupType_Treasure_Crowns_Red,       "GAME_TREASURE_CROWNS_RED" },
            { PickupType_Treasure_Crowns_Green,     "GAME_TREASURE_CROWNS_GREEN" },
            { PickupType_Treasure_Crowns_Blue,      "GAME_TREASURE_CROWNS_BLUE" },
            { PickupType_Treasure_Crowns_Purple,    "GAME_TREASURE_CROWNS_PURPLE" },
            { PickupType_Treasure_Skull_Red,        "GAME_TREASURE_JEWELEDSKULL_RED" },
            { PickupType_Treasure_Skull_Green,      "GAME_TREASURE_JEWELEDSKULL_GREEN" },
            { PickupType_Treasure_Skull_Blue,       "GAME_TREASURE_JEWELEDSKULL_BLUE" },
            { PickupType_Treasure_Skull_Purple,     "GAME_TREASURE_JEWELEDSKULL_PURPLE" },
            { PickupType_Powerup_Invisibility,      "GAME_POWERUPS_GHOST" },
            { PickupType_Powerup_Invincibility,     "GAME_POWERUPS_INVULNERABLE" },
            { PickupType_Powerup_Life,              "GAME_POWERUPS_EXTRALIFE" },
            { PickupType_Powerup_FireSword,         "GAME_POWERUPS_FIRESWORD" },
            { PickupType_Powerup_LightningSword,    "GAME_POWERUPS_LIGHTNINGSWORD" },
            { PickupType_Powerup_FrostSword,        "GAME_POWERUPS_ICESWORD" },
            { PickupType_BossWarp,                  "GAME_BOSSWARP" },
            { PickupType_Level2_Gem,                "LEVEL_GEM" },
        };

        auto findIt = s_PickupTypeToImageSetMap.find(pickupType);
        if (findIt == s_PickupTypeToImageSetMap.end())
        {
            LOG_ERROR("Could not find requested enum. Offending key: " + ToStr((int)pickupType));
            assert(false);
        }

        return findIt->second;
    }

    std::map<PickupType, std::string> g_PickupTypeToPickupSoundMap =
    {
        { PickupType_Default,                   SOUND_GAME_TREASURE_COIN },
        { PickupType_Treasure_Goldbars,         SOUND_GAME_TREASURE_GOLDBAR },
        { PickupType_Treasure_Rings_Red,        SOUND_GAME_TREASURE_RING },
        { PickupType_Treasure_Rings_Green,      SOUND_GAME_TREASURE_RING },
        { PickupType_Treasure_Rings_Blue,       SOUND_GAME_TREASURE_RING },
        { PickupType_Treasure_Rings_Purple,     SOUND_GAME_TREASURE_RING },
        { PickupType_Treasure_Necklace,         SOUND_GAME_TREASURE_GECKO },
        { PickupType_Treasure_Crosses_Red,      SOUND_GAME_TREASURE_CROSS },
        { PickupType_Treasure_Crosses_Green,    SOUND_GAME_TREASURE_CROSS },
        { PickupType_Treasure_Crosses_Blue,     SOUND_GAME_TREASURE_CROSS },
        { PickupType_Treasure_Crosses_Purple,   SOUND_GAME_TREASURE_CROSS },
        { PickupType_Treasure_Scepters_Red,     SOUND_GAME_TREASURE_SCEPTER },
        { PickupType_Treasure_Scepters_Green,   SOUND_GAME_TREASURE_SCEPTER },
        { PickupType_Treasure_Scepters_Blue,    SOUND_GAME_TREASURE_SCEPTER },
        { PickupType_Treasure_Scepters_Purple,  SOUND_GAME_TREASURE_SCEPTER },
        { PickupType_Treasure_Geckos_Red,       SOUND_GAME_TREASURE_GECKO },
        { PickupType_Treasure_Geckos_Green,     SOUND_GAME_TREASURE_GECKO },
        { PickupType_Treasure_Geckos_Blue,      SOUND_GAME_TREASURE_GECKO },
        { PickupType_Treasure_Geckos_Purple,    SOUND_GAME_TREASURE_GECKO },
        { PickupType_Ammo_Deathbag,             SOUND_GAME_PICKUP_AMMUNITION },
        { PickupType_Ammo_Shot,                 SOUND_GAME_PICKUP_AMMUNITION },
        { PickupType_Ammo_Shotbag,              SOUND_GAME_PICKUP_AMMUNITION },
        { PickupType_Powerup_Catnip_1,          SOUND_GAME_PICKUP_CATNIP },
        { PickupType_Powerup_Catnip_2,          SOUND_GAME_PICKUP_CATNIP },
        { PickupType_Health_Breadwater,         SOUND_GAME_PICKUP_FOODITEM },
        { PickupType_Health_25,                 SOUND_GAME_PICKUP_POTION },
        { PickupType_Health_10,                 SOUND_GAME_PICKUP_POTION },
        { PickupType_Health_15,                 SOUND_GAME_PICKUP_POTION },
        { PickupType_Ammo_Magic_5,              SOUND_GAME_PICKUP_MAGIC },
        { PickupType_Ammo_Magic_10,             SOUND_GAME_PICKUP_MAGIC },
        { PickupType_Ammo_Magic_25,             SOUND_GAME_PICKUP_MAGIC },
        { PickupType_Mappiece,                  SOUND_GAME_MAPPIECE },
        { PickupType_Warp,                      SOUND_GAME_ENTER_WARP },
        { PickupType_Treasure_Coins,            SOUND_GAME_TREASURE_COIN },
        { PickupType_Ammo_Dynamite,             SOUND_GAME_PICKUP_AMMUNITION },
        { PickupType_Curse_Ammo, "" },
        { PickupType_Curse_Magic, "" },
        { PickupType_Curse_Health, "" },
        { PickupType_Curse_Death, "" },
        { PickupType_Curse_Treasure, "" },
        { PickupType_Curse_Freeze, "" },
        { PickupType_Treasure_Chalices_Red,     SOUND_GAME_TREASURE_CHALICE },
        { PickupType_Treasure_Chalices_Green,   SOUND_GAME_TREASURE_CHALICE },
        { PickupType_Treasure_Chalices_Blue,    SOUND_GAME_TREASURE_CHALICE },
        { PickupType_Treasure_Chalices_Purple,  SOUND_GAME_TREASURE_CHALICE },
        { PickupType_Treasure_Crowns_Red,       SOUND_GAME_TREASURE_CROWN },
        { PickupType_Treasure_Crowns_Green,     SOUND_GAME_TREASURE_CROWN },
        { PickupType_Treasure_Crowns_Blue,      SOUND_GAME_TREASURE_CROWN },
        { PickupType_Treasure_Crowns_Purple,    SOUND_GAME_TREASURE_CROWN },
        { PickupType_Treasure_Skull_Red,        SOUND_GAME_TREASURE_SKULL },
        { PickupType_Treasure_Skull_Green,      SOUND_GAME_TREASURE_SKULL },
        { PickupType_Treasure_Skull_Blue,       SOUND_GAME_TREASURE_SKULL },
        { PickupType_Treasure_Skull_Purple,     SOUND_GAME_TREASURE_SKULL },
        { PickupType_Powerup_Invisibility,      SOUND_GAME_PICKUP_MAGIC },
        { PickupType_Powerup_Invincibility,     SOUND_GAME_PICKUP_MAGIC },
        { PickupType_Powerup_Life,              SOUND_GAME_EXTRA_LIFE },
        { PickupType_Powerup_FireSword,         SOUND_CLAW_PICKUP_FIRE_SWORD },
        { PickupType_Powerup_LightningSword,    SOUND_CLAW_PICKUP_LIGHTNING_SWORD },
        { PickupType_Powerup_FrostSword,        SOUND_CLAW_PICKUP_FROST_SWORD },
        { PickupType_BossWarp,                  SOUND_GAME_ENTER_WARP },
        { PickupType_Level2_Gem,                SOUND_GAME_MAPPIECE },
    };

    //=====================================================================================================================
    // Helper functions
    //=====================================================================================================================

    StrongActorPtr CreateAndReturnActor(TiXmlElement* pData)
    {
        assert(pData && "Failed to create xml data for actor");

        StrongActorPtr pActor = g_pApp->GetGameLogic()->VCreateActor(pData, NULL);
        assert(pActor && "Failed to create actor");

        // Just to be consistent
        shared_ptr<EventData_New_Actor> pNewActorEvent(new EventData_New_Actor(pActor->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pNewActorEvent);

        SAFE_DELETE(pData);

        return pActor;
    }

    void ImageSetToWildcardImagePath(std::string& imageSet)
    {
        std::replace(imageSet.begin(), imageSet.end(), '_', '/');
        if (imageSet.find("GAME/") != std::string::npos)
        {
            imageSet.replace(imageSet.begin(), imageSet.begin() + strlen("GAME/"), "/GAME/IMAGES/");
        }
        else if (imageSet.find("STATES/") != std::string::npos)
        {
            imageSet.replace(imageSet.begin(), imageSet.begin() + strlen("STATES/"), "/STATES/IMAGES/");
        }
        else if (imageSet.find("LEVEL/") != std::string::npos)
        {
            assert(g_pApp->GetGameLogic()->GetCurrentLevelData() != nullptr);
            int levelNumber = g_pApp->GetGameLogic()->GetCurrentLevelData()->GetLevelNumber();

            std::string level = "/LEVEL" + ToStr(levelNumber);
            imageSet.replace(imageSet.begin(), imageSet.begin() + strlen("LEVEL/"), level + "/IMAGES/");
        }
        else
        {
            /*LOG_WARNING("Conflicting image set: " + imageSet);
            LOG_WARNING("Assuming you know what you doing - be careful");*/

            return;

            assert(false && "Not implemented image set");
        }

        imageSet += "/*.PID";
    }

    uint32 GetScorePointsFromImageSet(const std::string& imageSet)
    {
        uint32 points = 0;
        if (imageSet == "GAME_TREASURE_COINS") points = 100;
        else if (imageSet == "GAME_TREASURE_GOLDBARS") points = 500;
        else if (imageSet == "GAME_TREASURE_NECKLACE") points = 10000;
        else if (imageSet.find("GAME_TREASURE_RINGS") != std::string::npos) points = 1500;
        else if (imageSet.find("GAME_TREASURE_CHALICES") != std::string::npos) points = 2500;
        else if (imageSet.find("GAME_TREASURE_CROSSES") != std::string::npos) points = 5000;
        else if (imageSet.find("GAME_TREASURE_SCEPTERS") != std::string::npos) points = 7500;
        else if (imageSet.find("GAME_TREASURE_GECKOS") != std::string::npos) points = 10000;
        else if (imageSet.find("GAME_TREASURE_CROWNS") != std::string::npos) points = 15000;
        else if (imageSet.find("GAME_TREASURE_JEWELEDSKULL") != std::string::npos) points = 25000;

        if (points == 0)
        {
            assert(false && "Invalid treasure");
        }

        return points;
    }

    uint32 GetHealthCountFromImageSet(const std::string& imageSet)
    {
        uint32 healthCount = 0;
        if (imageSet == "GAME_HEALTH_POTION1") healthCount = 10;
        else if (imageSet == "GAME_HEALTH_POTION2") healthCount = 15;
        else if (imageSet == "GAME_HEALTH_POTION3") healthCount = 25;
        else if (imageSet == "GAME_HEALTH_BREADWATER") healthCount = 5;
        else if (imageSet == "LEVEL_HEALTH") healthCount = 5;

        if (healthCount == 0)
        {
            LOG_ERROR("Offending imageset: " + imageSet);
            assert(false && "Invalid health pickup");
        }

        return healthCount;
    }

    std::pair<std::string, uint32> GetAmmoCountAndTypeFromImageset(const std::string& imageSet)
    {
        if (imageSet == "GAME_AMMO_SHOT") return std::make_pair("Pistol", 10);
        else if (imageSet == "GAME_AMMO_SHOTBAG") return std::make_pair("Pistol", 15);
        else if (imageSet == "GAME_AMMO_DEATHBAG") return std::make_pair("Pistol", 25);
        else if (imageSet == "GAME_MAGIC_GLOW") return std::make_pair("Magic", 5);
        else if (imageSet == "GAME_MAGIC_STARGLOW") return std::make_pair("Magic", 10);
        else if (imageSet == "GAME_MAGIC_MAGICCLAW") return std::make_pair("Magic", 25);
        else if (imageSet == "GAME_DYNAMITE") return std::make_pair("Dynamite", 5);
        else
        {
            assert(false && "Invalid ammo image set");
        }

        return { "", 0 };
    }

    std::pair<std::string, uint32> GetPowerupTypeAndDurationFromImageset(const std::string& imageSet)
    {
        if (imageSet == "GAME_CATNIPS_NIP1") return std::make_pair("Catnip", 15000);
        else if (imageSet == "GAME_CATNIPS_NIP2") return std::make_pair("Catnip", 30000);
        else if (imageSet == "GAME_POWERUPS_GHOST") return std::make_pair("Invisibility", 30000);
        else if (imageSet == "GAME_POWERUPS_INVULNERABLE") return std::make_pair("Invulnerability", 30000);
        else if (imageSet == "GAME_POWERUPS_LIGHTNINGSWORD") return std::make_pair("LightningSword", 30000);
        else if (imageSet == "GAME_POWERUPS_FIRESWORD") return std::make_pair("FireSword", 30000);
        else if (imageSet == "GAME_POWERUPS_ICESWORD") return std::make_pair("IceSword", 30000);
        else
        {
            assert(false && "Invalid ammo image set");
        }

        return { "", 0 };
    }

    std::string GetImageSetFromClawAmmoType(AmmoType ammoType)
    {
        if (ammoType == AmmoType_Pistol)
        {
            return "GAME_BULLETS";
        }
        else if (ammoType == AmmoType_Magic)
        {
            return "GAME_MAGICCLAW";
        }
        else if (ammoType == AmmoType_Dynamite)
        {
            return "GAME_DYNAMITELIT";
        }

        assert(false && "Unknwon ammo type");

        return "Unknown";
    }

    std::string GetImageSetFromGlitterType(const std::string& glitterType)
    {
        if (glitterType == "Glitter_Yellow") { return "GAME_GLITTER"; }
        else if (glitterType == "Glitter_Red") { return "GAME_GLITTERRED"; }
        else if (glitterType == "Glitter_Green") { return "GAME_GREENGLITTER"; }

        LOG_ERROR("Offending glitter type: " + glitterType);
        assert(false && "Invalid glitter type");

        return "Unknown";
    }

    std::string GetImageSetFromScoreCount(int score)
    {
        if (score == 0) { return "/GAME/IMAGES/POINTS/FRAME0*"; }
        if (score == 100) { return "/GAME/IMAGES/POINTS/FRAME1*"; }
        else if (score == 500) { return "/GAME/IMAGES/POINTS/FRAME2*"; }
        else if (score == 1500) { return "/GAME/IMAGES/POINTS/FRAME3*"; }
        else if (score == 2500) { return "/GAME/IMAGES/POINTS/FRAME4*"; }
        else if (score == 5000) { return "/GAME/IMAGES/POINTS/FRAME5*"; }
        else if (score == 7500) { return "/GAME/IMAGES/POINTS/FRAME6*"; }
        else if (score == 10000) { return "/GAME/IMAGES/POINTS/FRAME7*"; }
        else if (score == 15000) { return "/GAME/IMAGES/POINTS/FRAME8*"; }
        else if (score == 25000) { return "/GAME/IMAGES/POINTS/FRAME9*"; }
        else
        {
            LOG_ERROR("Conflicting score: " + ToStr(score));
            assert(false && "Invalid score number");
        }

        // Never reached but stops compiler from complaining
        return "";
    }

    int GetOffsetYByZCoord(int zCoord)
    {
        // Max Z coordinate from wwd file.
        const int maxZCoord = (int) zIndexes::MaxIndex;
        // Ground height in px (It's about a half tile height)
        const int maxOffset = 32;
        // Offset of object with middle Z Coord (maxZCoord/2)
        // Ground already has a fixture ~8px
        const int middleOffset = (maxOffset / 2) - 8;

        const int normalized = zCoord - maxZCoord / 2;
        const double coef = (double) maxZCoord / maxOffset;
        return (int) -(normalized / coef + middleOffset);
    }

    std::string FixtureTypeToString(FixtureType fixtureType)
    {
        std::string fixtureTypeStr;

        if (fixtureType == FixtureType_Solid) { fixtureTypeStr = "Solid"; }
        else if (fixtureType == FixtureType_Ground) { fixtureTypeStr = "Ground"; }
        else if (fixtureType == FixtureType_Climb) { fixtureTypeStr = "Climb"; }
        else if (fixtureType == FixtureType_Death) { fixtureTypeStr = "Death"; }
        else if (fixtureType == FixtureType_Trigger) { fixtureTypeStr = "Trigger"; }
        else if (fixtureType == FixtureType_Controller) { fixtureTypeStr = "Controller"; }
        else if (fixtureType == FixtureType_Projectile) { fixtureTypeStr = "Projectile"; }
        else if (fixtureType == FixtureType_Crate) { fixtureTypeStr = "Crate"; }
        else if (fixtureType == FixtureType_Pickup) { fixtureTypeStr = "Pickup"; }
        else if (fixtureType == FixtureType_Trigger) { fixtureTypeStr = "Trigger"; }
        else if (fixtureType == FixtureType_PowderKeg) { fixtureTypeStr = "PowderKeg"; }
        else if (fixtureType == FixtureType_Explosion) { fixtureTypeStr = "Explosion"; }
        else if (fixtureType == FixtureType_EnemyAI) { fixtureTypeStr = "EnemyAI"; }
        else if (fixtureType == FixtureType_EnemyAIMeleeSensor) { fixtureTypeStr = "EnemyAIMeleeSensor"; }
        else if (fixtureType == FixtureType_EnemyAIDuckMeleeSensor) { fixtureTypeStr = "EnemyAIDuckMeleeSensor"; }
        else if (fixtureType == FixtureType_EnemyAIRangedSensor) { fixtureTypeStr = "EnemyAIRangedSensor"; }
        else if (fixtureType == FixtureType_EnemyAIDuckRangedSensor) { fixtureTypeStr = "EnemyAIDuckRangedSensor"; }
        else if (fixtureType == FixtureType_DamageAura) { fixtureTypeStr = "DamageAura"; }
        else
        {
            assert(false && "Unknown body type");
        }

        return fixtureTypeStr;
    }

    std::string BodyTypeToString(b2BodyType bodyType)
    {
        std::string bodyTypeStr;

        if (bodyType == b2_staticBody) { bodyTypeStr = "Static"; }
        else if (bodyType == b2_kinematicBody) { bodyTypeStr = "Kinematic"; }
        else if (bodyType == b2_dynamicBody) { bodyTypeStr = "Dynamic"; }
        else
        {
            assert(false && "Unknown body type");
        }

        return bodyTypeStr;
    }

    
    std::vector<std::pair<std::string, std::string>> GetSoundsFromActorLogic(const std::string& logic)
    {
        std::vector <std::pair<std::string, std::string>> soundTypeAndNamePairList;
        if (logic == "Soldier")
        {
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("TakeDamage"), std::string(SOUND_LEVEL1_SOLDIER_DAMAGED)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("Death"), std::string(SOUND_LEVEL1_SOLDIER_DEATH)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("MeleeAttack"), std::string(SOUND_LEVEL1_SOLDIER_MELEE_ATTACK)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("RangedAttack"), std::string(SOUND_LEVEL1_SOLDIER_SHOOT)));

        }
        else if (logic == "Officer")
        {
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("TakeDamage"), std::string(SOUND_LEVEL1_TAKE_DAMAGE1)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("Death"), std::string(SOUND_LEVEL1_OFFICER_DEATH1)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("Death"), std::string(SOUND_LEVEL1_OFFICER_DEATH2)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("MeleeAttack"), std::string(SOUND_LEVEL1_OFFICER_MELEE_ATTACK)));
        }
        else if (logic == "Rat")
        {
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("Death"), std::string(SOUND_LEVEL1_RAT_DEATH)));
            soundTypeAndNamePairList.push_back(std::make_pair(
                std::string("RangedAttack"), std::string(SOUND_LEVEL1_RAT_THROW_BOMB)));
        }
        else
        {
            LOG_ERROR("Unsupported logic: " + logic);
            assert(false && "Unsupported logic");
        }

        return soundTypeAndNamePairList;
    }

    std::string GetSoundPathFromClawPath(const std::string& sound)
    {
        if (sound.find("_") == std::string::npos)
        {
            LOG_WARNING("Cannot convert sound: " + sound);
            return sound;
        }

        std::string soundName = sound;
        std::replace(soundName.begin(), soundName.end(), '_', '/');
        if (soundName.find("GAME/") != std::string::npos)
        {
            soundName.replace(soundName.begin(), soundName.begin() + strlen("GAME/"), "/GAME/SOUNDS/");
            soundName += ".WAV";
        }
        else if (soundName.find("STATES/") != std::string::npos)
        {
            soundName.replace(soundName.begin(), soundName.begin() + strlen("STATES/"), "/STATES/SOUNDS/");
            soundName += ".WAV";
        }
        else if (soundName.find("LEVEL/") != std::string::npos)
        {
            int currentLevel = g_pApp->GetGameLogic()->GetCurrentLevelData()->GetLevelNumber();
            std::string levelName = "LEVEL" + ToStr(currentLevel);
            soundName.replace(soundName.begin(), soundName.begin() + strlen("LEVEL/"), 
                "/" + levelName + "/SOUNDS/");
            soundName += ".WAV";
        }
        else
        {
            LOG_ERROR("Conflicting sound: " + sound);
            assert(false && "Invalid sound");
        }

        return soundName;
    }

    //=====================================================================================================================
    // General functions for creating components
    //=====================================================================================================================
    TiXmlElement* CreatePositionComponent(double x, double y)
    {
        TiXmlElement* pComponent = new TiXmlElement("PositionComponent");
        XML_ADD_2_PARAM_ELEMENT("Position", "x", (int)x, "y", (int)y, pComponent);
        return pComponent;
    }

    TiXmlElement* CreateActorRenderComponent(const std::vector<std::string>& imagePaths, int32 zCoord, bool isVisible = true, bool isMirrored = false, bool isInverted = false)
    {
        TiXmlElement* pComponent = new TiXmlElement("ActorRenderComponent");
        XML_ADD_TEXT_ELEMENT("Visible", ToStr(isVisible).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("Mirrored", ToStr(isMirrored).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("Inverted", ToStr(isInverted).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(zCoord).c_str(), pComponent);

        for (const std::string &imagePath : imagePaths)
        {
            XML_ADD_TEXT_ELEMENT("ImagePath", imagePath.c_str(), pComponent);
        }

        return pComponent;
    }

    TiXmlElement* CreateActorRenderComponent(const std::string& imageSet, uint32 zCoord, bool isVisible = true, bool isMirrored = false, bool isInverted = false, bool convertImageSetToWildcard = true)
    {
        TiXmlElement* pComponent = new TiXmlElement("ActorRenderComponent");
        XML_ADD_TEXT_ELEMENT("Visible", ToStr(isVisible).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("Mirrored", ToStr(isMirrored).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("Inverted", ToStr(isInverted).c_str(), pComponent);
        XML_ADD_TEXT_ELEMENT("ZCoord", ToStr(zCoord).c_str(), pComponent);

        if (convertImageSetToWildcard)
        {
            std::string wildcardImagePath = imageSet;
            ImageSetToWildcardImagePath(wildcardImagePath);
            XML_ADD_TEXT_ELEMENT("ImagePath", wildcardImagePath.c_str(), pComponent);
        } else {
            XML_ADD_TEXT_ELEMENT("ImagePath", imageSet.c_str(), pComponent);
        }

        return pComponent;
    }

    TiXmlElement* CreateAnimationComponent(const std::string& animationPath, bool pauseOnStart)
    {
        TiXmlElement* pAnimElem = new TiXmlElement("AnimationComponent");
        XML_ADD_TEXT_ELEMENT("AnimationPath", animationPath.c_str(), pAnimElem);
        XML_ADD_TEXT_ELEMENT("PauseOnStart", ToStr(pauseOnStart).c_str(), pAnimElem);

        return pAnimElem;
    }

    TiXmlElement* CreateCycleAnimationComponent(uint32 cycleTime, bool pauseOnStart = false)
    {
        TiXmlElement* pAnimElem = new TiXmlElement("AnimationComponent");
        std::string cycleStr = "cycle" + ToStr(cycleTime);
        XML_ADD_1_PARAM_ELEMENT("Animation", "type", cycleStr.c_str(), pAnimElem);
        XML_ADD_TEXT_ELEMENT("PauseOnStart", ToStr(pauseOnStart).c_str(), pAnimElem);

        return pAnimElem;
    }

    TiXmlElement* CreateTriggerComponent(int enterCount, bool onceALife, bool isStatic)
    {
        TiXmlElement* pTriggerComponent = new TiXmlElement("TriggerComponent");
        if (onceALife)
        {
            XML_ADD_TEXT_ELEMENT("TriggerOnce", "true", pTriggerComponent);
        }
        else if (enterCount > 0)
        {
            XML_ADD_TEXT_ELEMENT("TriggerFinitedTimes", ToStr(enterCount).c_str(), pTriggerComponent);
        }
        else
        {
            XML_ADD_TEXT_ELEMENT("TriggerUnlimited", "true", pTriggerComponent);
        }
        if (isStatic)
        {
            XML_ADD_TEXT_ELEMENT("IsStatic", "true", pTriggerComponent);
        }

        return pTriggerComponent;
    }

    TiXmlElement* CreatePhysicsComponent(
        const std::string& bodyTypeStr,
        bool hasFootSensor,
        bool hasCapsuleShape,
        bool hasBulletBehaviour,
        bool hasSensorBehaviour,
        const std::string& fixtureTypeStr,
        const Point& position,
        const Point& positionOffset,
        const std::string& collisionShape,
        const Point& size,
        float gravityScale,
        bool hasInitialSpeed,
        bool hasInitialImpulse,
        const Point& initialSpeed,
        /*CollisionFlag*/ uint32 collisionFlag,
        uint32 collisionMask,
        float density,
        float friction,
        float restitution,
        const std::string& prefabType = "")
    {
        TiXmlElement* pPhysicsComponent = new TiXmlElement("PhysicsComponent");
        XML_ADD_TEXT_ELEMENT("BodyType", bodyTypeStr.c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasFootSensor", ToStr(hasFootSensor).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasCapsuleShape", ToStr(hasCapsuleShape).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasBulletBehaviour", ToStr(hasBulletBehaviour).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasSensorBehaviour", ToStr(hasSensorBehaviour).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("FixtureType", fixtureTypeStr.c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("PositionOffset", "x", positionOffset.x, "y", positionOffset.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionShape", collisionShape.c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("CollisionSize", "width", size.x, "height", size.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("GravityScale", ToStr(gravityScale).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasInitialSpeed", ToStr(hasInitialSpeed).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasInitialImpulse", ToStr(hasInitialImpulse).c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("InitialSpeed", "x", initialSpeed.x, "y", initialSpeed.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionFlag", ToStr(collisionFlag).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionMask", ToStr(collisionMask).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Friction", ToStr(friction).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Density", ToStr(density).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Restitution", ToStr(restitution).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("PrefabType", prefabType.c_str(), pPhysicsComponent);

        return pPhysicsComponent;
    }

    TiXmlElement* CreatePhysicsComponent(const ActorBodyDef* pBodyDef)
    {
        TiXmlElement* pPhysicsComponent = new TiXmlElement("PhysicsComponent");

        std::string bodyTypeStr = BodyTypeToString(pBodyDef->bodyType);
        std::string fixtureTypeStr = FixtureTypeToString(pBodyDef->fixtureType);

        XML_ADD_TEXT_ELEMENT("BodyType", bodyTypeStr.c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasFootSensor", ToStr(pBodyDef->addFootSensor).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasCapsuleShape", ToStr(pBodyDef->makeCapsule).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasBulletBehaviour", ToStr(pBodyDef->makeBullet).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasSensorBehaviour", ToStr(pBodyDef->makeSensor).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("FixtureType", fixtureTypeStr.c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("PositionOffset", "x", pBodyDef->positionOffset.x, "y", pBodyDef->positionOffset.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionShape", pBodyDef->collisionShape.c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("CollisionSize", "width", pBodyDef->size.x, "height", pBodyDef->size.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("GravityScale", ToStr(pBodyDef->gravityScale).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasInitialSpeed", ToStr(pBodyDef->setInitialSpeed).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("HasInitialImpulse", ToStr(pBodyDef->setInitialImpulse).c_str(), pPhysicsComponent);
        XML_ADD_2_PARAM_ELEMENT("InitialSpeed", "x", pBodyDef->initialSpeed.x, "y", pBodyDef->initialSpeed.y, pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionFlag", ToStr(pBodyDef->collisionFlag).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("CollisionMask", ToStr(pBodyDef->collisionMask).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Friction", ToStr(pBodyDef->friction).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Density", ToStr(pBodyDef->density).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("Restitution", ToStr(pBodyDef->restitution).c_str(), pPhysicsComponent);
        XML_ADD_TEXT_ELEMENT("PrefabType", pBodyDef->prefabType.c_str(), pPhysicsComponent);

        // Convert actor's fixtures to xml
        for (const ActorFixtureDef &fixture : pBodyDef->fixtureList)
        {
            pPhysicsComponent->LinkEndChild(ActorFixtureDefToXml(&fixture));
        }

        return pPhysicsComponent;
    }

    TiXmlElement* CreateFollowableComponent(const Point& offset, const std::string& imageSet, const std::string& animPath)
    {
        TiXmlElement* pFollowableComponentElem = new TiXmlElement("FollowableComponent");
        XML_ADD_2_PARAM_ELEMENT("Offset", "x", offset.x, "y", offset.y, pFollowableComponentElem);
        XML_ADD_TEXT_ELEMENT("ImageSet", imageSet.c_str(), pFollowableComponentElem);
        XML_ADD_TEXT_ELEMENT("AnimationPath", animPath.c_str(), pFollowableComponentElem);

        return pFollowableComponentElem;
    }

    TiXmlElement* CreateLootComponent(const std::vector<PickupType>& loot, int lootSoundChance = 0)
    {
        TiXmlElement* pLootComponent = new TiXmlElement("LootComponent");
        
        for (auto item : loot)
        {
            XML_ADD_TEXT_ELEMENT("Item", ToStr(item).c_str(), pLootComponent);
        }

        if (loot.empty())
        {
            XML_ADD_TEXT_ELEMENT("Item", ToStr(PickupType_Treasure_Coins).c_str(), pLootComponent);
        }

        XML_ADD_TEXT_ELEMENT("LootSoundChance", ToStr(lootSoundChance).c_str(), pLootComponent);

        return pLootComponent;
    }

    TiXmlElement* CreateDestroyableComponent(bool deleteOnDestruction, const std::string& deathAnimName, const std::vector<std::string>& deathSounds, bool removeFromPhysics = true)
    {
        TiXmlElement* pDestroyableComponent = new TiXmlElement("DestroyableComponent");

        XML_ADD_TEXT_ELEMENT("DeleteOnDestruction", ToStr(deleteOnDestruction).c_str(), pDestroyableComponent);
        XML_ADD_TEXT_ELEMENT("RemoveFromPhysics", ToStr(removeFromPhysics).c_str(), pDestroyableComponent);
        if (!deathAnimName.empty())
        {
            XML_ADD_TEXT_ELEMENT("DeathAnimationName", deathAnimName.c_str(), pDestroyableComponent);
        }
        for (auto &deathSound : deathSounds)
        {
            XML_ADD_TEXT_ELEMENT("DeathSound", deathSound.c_str(), pDestroyableComponent);
        }

        return pDestroyableComponent;
    }

    TiXmlElement* CreateDestroyableComponent(const DestroyableComponentDef& def)
    {
        TiXmlElement* pDestroyableComponent = new TiXmlElement("DestroyableComponent");

        AddXmlTextElement("DeleteImmediately", def.deleteImmediately, pDestroyableComponent);
        AddXmlTextElement("DeleteDelay", def.deleteDelay, pDestroyableComponent);
        AddXmlTextElement("BlinkOnDestruction", def.blinkOnDestruction, pDestroyableComponent);
        AddXmlTextElement("DeleteOnDestruction", def.deleteOnDestruction, pDestroyableComponent);
        AddXmlTextElement("RemoveFromPhysics", def.removeFromPhysics, pDestroyableComponent);
        AddXmlTextElement("DeathAnimationName", def.deathAnimationName, pDestroyableComponent);
        for (const std::string& deathSound : def.deathSoundList)
        {
            AddXmlTextElement("DeathSound", deathSound, pDestroyableComponent);
        }

        return pDestroyableComponent;
    }

    TiXmlElement* CreateHealthComponent(int currentHealth, int maxHealth)
    {
        TiXmlElement* pHealthComponent = new TiXmlElement("HealthComponent");

        XML_ADD_TEXT_ELEMENT("Health", ToStr(currentHealth).c_str(), pHealthComponent);
        XML_ADD_TEXT_ELEMENT("MaxHealth", ToStr(maxHealth).c_str(), pHealthComponent);

        return pHealthComponent;
    }

    TiXmlElement* CreateExplodeableComponent(const Point& explosionSize, int damage, int explodingTime = 100)
    {
        TiXmlElement* pExplodeableComponent = new TiXmlElement("ExplodeableComponent");

        XML_ADD_2_PARAM_ELEMENT("ExplosionSize", "width", ToStr(explosionSize.x).c_str(), "height", ToStr(explosionSize.y).c_str(), pExplodeableComponent);
        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pExplodeableComponent);
        XML_ADD_TEXT_ELEMENT("ExplodingTime", ToStr(explodingTime).c_str(), pExplodeableComponent);

        return pExplodeableComponent;
    }

    TiXmlElement* CreateAreaDamageComponent(int damage, int areaDuration, DamageType damageType, Direction hitDirection, int sourceActorId)
    {
        TiXmlElement* pAreaDamageComponent = new TiXmlElement("AreaDamageComponent");

        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pAreaDamageComponent);
        XML_ADD_TEXT_ELEMENT("Duration", ToStr(areaDuration).c_str(), pAreaDamageComponent);
        XML_ADD_TEXT_ELEMENT("DamageType", ToStr((int)damageType).c_str(), pAreaDamageComponent);
        XML_ADD_TEXT_ELEMENT("HitDirection", ToStr((int)hitDirection).c_str(), pAreaDamageComponent);
        XML_ADD_TEXT_ELEMENT("SourceActorId", ToStr((int)sourceActorId).c_str(), pAreaDamageComponent);

        return pAreaDamageComponent;
    }

    TiXmlElement* CreateXmlData_GlitterComponent(const std::string& glitterType, bool spawnImmediate, bool followOwner)
    {
        TiXmlElement* pGlitterComponent = new TiXmlElement("GlitterComponent");

        XML_ADD_TEXT_ELEMENT("GlitterType", glitterType.c_str(), pGlitterComponent);
        XML_ADD_TEXT_ELEMENT("SpawnImmediate", ToStr(spawnImmediate).c_str(), pGlitterComponent);
        XML_ADD_TEXT_ELEMENT("FollowOwner", ToStr(followOwner).c_str(), pGlitterComponent);

        return pGlitterComponent;
    }

    

    ActorFixtureDef CreateActorAgroRangeFixture(const Point& size, const Point& offset, FixtureType fixtureType)
    {
        ActorFixtureDef fixtureDef;

        fixtureDef.fixtureType = fixtureType;
        fixtureDef.collisionFlag = CollisionFlag_DynamicActor;
        fixtureDef.collisionMask = CollisionFlag_Controller;
        fixtureDef.isSensor = true;
        fixtureDef.size = size;
        fixtureDef.offset = offset;

        return fixtureDef;
    }

    TiXmlElement* CreateXmlData_TakeDamageState(const std::vector<std::string>& takeDamageAnims, int invulnerabilityTime)
    {
        TiXmlElement* pTakeDamageStateElem = new TiXmlElement("TakeDamageAIStateComponent");
        for (const std::string& anim : takeDamageAnims)
        {
            XML_ADD_TEXT_ELEMENT("TakeDamageAnimation", anim.c_str(), pTakeDamageStateElem);
        }
        XML_ADD_TEXT_ELEMENT("InvulnerabilityTime", ToStr(invulnerabilityTime).c_str(), pTakeDamageStateElem);

        return pTakeDamageStateElem;
    }

    TiXmlElement* CreateXmlData_PatrolState(uint32 animationDelay, double patrolSpeed, int leftPatrolBorder, int rightPatrolBorder, const std::string& walkAnimation, const std::vector<std::string>& idleAnimations, bool retainDirection = false)
    {
        TiXmlElement* pPatrolStateElem = new TiXmlElement("PatrolEnemyAIStateComponent");
        XML_ADD_TEXT_ELEMENT("PatrolSpeed", ToStr(patrolSpeed).c_str(), pPatrolStateElem);
        XML_ADD_TEXT_ELEMENT("LeftPatrolBorder", ToStr(leftPatrolBorder).c_str(), pPatrolStateElem);
        XML_ADD_TEXT_ELEMENT("RightPatrolBorder", ToStr(rightPatrolBorder).c_str(), pPatrolStateElem);
        XML_ADD_TEXT_ELEMENT("RetainDirection", ToStr(retainDirection).c_str(), pPatrolStateElem);

        TiXmlElement* pWalkActionElem = new TiXmlElement("WalkAction");
        XML_ADD_TEXT_ELEMENT("Animation", walkAnimation.c_str(), pWalkActionElem);
        pPatrolStateElem->LinkEndChild(pWalkActionElem);

        if (!idleAnimations.empty())
        {
            TiXmlElement* pIdleActionElem = new TiXmlElement("IdleAction");
            XML_ADD_TEXT_ELEMENT("AnimationDelay", ToStr(animationDelay).c_str(), pIdleActionElem);
            for (auto &idleAnim : idleAnimations)
            {
                XML_ADD_TEXT_ELEMENT("Animation", idleAnim.c_str(), pIdleActionElem);
            }
            pPatrolStateElem->LinkEndChild(pIdleActionElem);
        }

        return pPatrolStateElem;
    }

    TiXmlElement* CreatePredefinedMoveComponent(std::vector<PredefinedMove>& moves, bool isInfinite)
    {
        TiXmlElement* pPredefinedMovesElem = new TiXmlElement("PredefinedMoveComponent");

        for (PredefinedMove& move : moves)
        {
            TiXmlElement* pMoveElem = new TiXmlElement("PredefinedMove");
            XML_ADD_TEXT_ELEMENT("DurationMiliseconds", ToStr(move.msDuration).c_str(), pMoveElem);
            XML_ADD_2_PARAM_ELEMENT("PixelsPerSecond", "x", ToStr(move.pixelsPerSecond.x).c_str(), 
                "y", ToStr(move.pixelsPerSecond.y).c_str(), pMoveElem);
            XML_ADD_TEXT_ELEMENT("Sound", move.soundToPlay.c_str(), pMoveElem);
            pPredefinedMovesElem->LinkEndChild(pMoveElem);
        }

        XML_ADD_TEXT_ELEMENT("IsInfinite", ToStr(isInfinite).c_str(), pPredefinedMovesElem);

        return pPredefinedMovesElem;
    }

    TiXmlElement* CreatePredefinedMoveComponent(const PredefinedMove& move, bool isInfinite)
    {
        std::vector<PredefinedMove> moves = { move };
        return CreatePredefinedMoveComponent(moves, isInfinite);
    }

    TiXmlElement* CreateXmlData_EnemyAttackActionState(std::vector<EnemyAttackAction>& attackActions)
    {
        assert(!attackActions.empty());

        DamageType attackDamageType = attackActions[0].attackDamageType;
        std::string componentName;

        switch (attackDamageType)
        {
            case DamageType_MeleeAttack:
                componentName = "MeleeAttackAIStateComponent";
                break;
            
            case DamageType_Bullet:
            case DamageType_Trident:
            case DamageType_SirenProjectile:
                componentName = "RangedAttackAIStateComponent";
                break;

            default:
                assert(false && "Unknown damage type");
        }

        TiXmlElement* pAttackStateElem = new TiXmlElement(componentName.c_str());

        TiXmlElement* pAttacksElem = new TiXmlElement("Attacks");
        for (auto &attackAction : attackActions)
        {
            TiXmlElement* pAttackActionElem = new TiXmlElement("AttackAction");
            XML_ADD_TEXT_ELEMENT("Animation", attackAction.animation.c_str(), pAttackActionElem);
            XML_ADD_TEXT_ELEMENT("AttackAnimFrameIdx", ToStr(attackAction.attackAnimFrameIdx).c_str(), pAttackActionElem);
            XML_ADD_TEXT_ELEMENT("AttackType", ToStr(attackAction.attackDamageType).c_str(), pAttackActionElem);
            XML_ADD_TEXT_ELEMENT("AttackFxImageSet", attackAction.attackFxImageSet.c_str(), pAttackActionElem);
            XML_ADD_2_PARAM_ELEMENT("AttackSpawnPositionOffset", "x", attackAction.attackSpawnPositionOffset.x, "y", attackAction.attackSpawnPositionOffset.y, pAttackActionElem);
            XML_ADD_2_PARAM_ELEMENT("AttackAreaSize", "width", attackAction.attackAreaSize.x, "height", attackAction.attackAreaSize.y, pAttackActionElem);
            XML_ADD_TEXT_ELEMENT("Damage", ToStr(attackAction.damage).c_str(), pAttackActionElem);

            pAttacksElem->LinkEndChild(pAttackActionElem);
        }

        pAttackStateElem->LinkEndChild(pAttacksElem);

        return pAttackStateElem;
    }

    //=====================================================================================================================
    // Specific functions for creating specific actors
    //=====================================================================================================================

    TiXmlElement* CreateXmlData_SoundTriggerActor(const std::string& sound, const Point& position, const Point& size, int enterCount, bool activateDialog)
    {
        // General stuff

        TiXmlElement* pActorElem = new TiXmlElement("Actor");
        pActorElem->SetAttribute("Type", sound.c_str());

        pActorElem->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActorElem->LinkEndChild(CreateTriggerComponent(enterCount, false, false));

        ActorBodyDef bodyDef;
        bodyDef.makeSensor = true;
        bodyDef.bodyType = b2_staticBody;
        bodyDef.fixtureType = FixtureType_Pickup;
        bodyDef.position = position;
        bodyDef.collisionFlag = CollisionFlag_Pickup;
        bodyDef.collisionMask = CollisionFlag_Controller | CollisionFlag_InvisibleController;
        bodyDef.fixtureType = FixtureType_Trigger;
        bodyDef.size = size;

        pActorElem->LinkEndChild(CreatePhysicsComponent(&bodyDef));

        // SoundTrigger specific
        TiXmlElement* pSoundTriggerElem = new TiXmlElement("SoundTriggerComponent");

        XML_ADD_TEXT_ELEMENT("EnterCount", ToStr(enterCount).c_str(), pSoundTriggerElem);
        XML_ADD_TEXT_ELEMENT("ActivateDialog", ToStr(activateDialog).c_str(), pSoundTriggerElem);

        std::string soundPath = GetSoundPathFromClawPath(sound);
        XML_ADD_TEXT_ELEMENT("Sound", soundPath.c_str(), pSoundTriggerElem);

        pActorElem->LinkEndChild(pSoundTriggerElem);

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_SoundTriggerActor(const std::string& sound, const std::string& logicName, const Point& position, const SDL_Rect& presetPosition, int enterCount)
    {
        Point size;
        if (logicName.find("Tiny") != std::string::npos)
        {
            size.Set(32, 32);
        }
        else if (logicName.find("Small") != std::string::npos)
        {
            size.Set(64, 64);
        }
        else if (logicName.find("Big") != std::string::npos)
        {
            size.Set(256, 256);
        }
        else if (logicName.find("Huge") != std::string::npos)
        {
            size.Set(512, 512);
        }
        else if (logicName.find("Wide") != std::string::npos)
        {
            size.Set(200, 64);
        }
        else if (logicName.find("Tall") != std::string::npos)
        {
            size.Set(64, 200);
        }
        else
        {
            size.Set(128, 128);
        }

        bool activateDialog = logicName.find("ClawDialog") != std::string::npos;

        // Position is already preset
        if (presetPosition.x != 0)
        {
            //position.Set(presetPosition.x, presetPosition.y);
            //size.Set(presetPosition.w - presetPosition.x, presetPosition.h - presetPosition.y);

            size.x = std::min((int)size.x, presetPosition.w - presetPosition.x);
            size.y = std::min((int)size.y, presetPosition.h - presetPosition.y);
        }

        return CreateXmlData_SoundTriggerActor(sound, position, size, enterCount, activateDialog);
    }

    TiXmlElement* CreateXmlData_GlobalAmbientSoundActor(
        const std::string& sound,
        int soundVolume,
        int minTimeOff,
        int maxTimeOff,
        int minTimeOn,
        int maxTimeOn,
        bool isLooping)
    {
        TiXmlElement* pActorElem = new TiXmlElement("Actor");
        pActorElem->SetAttribute("Type", sound.c_str());

        std::string soundPath = GetSoundPathFromClawPath(sound);

        TiXmlElement* pGlobalSoundComponent = new TiXmlElement("GlobalAmbientSoundComponent");
        XML_ADD_TEXT_ELEMENT("Sound", soundPath.c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("SoundVolume", ToStr(soundVolume).c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("MinTimeOff", ToStr(minTimeOff).c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("MaxTimeOff", ToStr(maxTimeOff).c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("MinTimeOn", ToStr(minTimeOn).c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("MaxTimeOn", ToStr(maxTimeOn).c_str(), pGlobalSoundComponent);
        XML_ADD_TEXT_ELEMENT("IsLooping", ToStr(isLooping).c_str(), pGlobalSoundComponent);

        pActorElem->LinkEndChild(pGlobalSoundComponent);

        return pActorElem;
    }

    template <typename K, typename V>
    bool IsKeyInMap(const K& key, const std::map<K, V>& m)
    {
        return m.count(key);
    }

    template <typename K, typename V>
    V StrictFindValueInMap(const K& key, const std::map<K, V>& m)
    {
        auto findIt = m.find(key);
        if (findIt == m.end())
        {
            LOG_ERROR("Could not find key in specified map. Key: [" + ToStr(key) + "]");
            assert(false);
        }

        return findIt->second;
    }

    template <typename K, typename V>
    V TryGetValueFromMap(const K& key, const std::map<K, V>& m, const V& defaultVal)
    {
        if (IsKeyInMap(key, m))
        {
            return StrictFindValueInMap(key, m);
        }

        return defaultVal;
    }

    // TODO: What is this function specifically for ? Only dynamically spawned pickups ?
    TiXmlElement* CreateXmlData_GeneralPickupActor(const std::string& imageSet, const Point& position, zIndexes zCoord, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActorElem = new TiXmlElement("Actor");
        pActorElem->SetAttribute("Type", imageSet.c_str());

        bool isMirrored = TryGetValueFromMap(std::string("IsMirrored"), paramMap, std::string("")) == "true";
        bool isInverted = TryGetValueFromMap(std::string("IsInverted"), paramMap, std::string("")) == "true";

        pActorElem->LinkEndChild(CreatePositionComponent(position.x, position.y));

        std::string wildcardImagePath = imageSet;

        ImageSetToWildcardImagePath(wildcardImagePath);
        std::vector<std::string> imagePaths;
        imagePaths.push_back(wildcardImagePath);
        pActorElem->LinkEndChild(CreateActorRenderComponent(imagePaths, (int32) zCoord, true, isMirrored, isInverted));

        pActorElem->LinkEndChild(CreateTriggerComponent(1, false, false));

        srand((long)pActorElem + time(NULL));
        double speedX = 0.5 + (rand() % 100) / 50.0;
        double speedY = -(1 + (rand() % 100) / 50.0);

        if (rand() % 2 == 1) { speedX *= -1; }

        ActorBodyDef bodyDef;
        if (isStatic)
        {
            bodyDef.bodyType = b2_staticBody;
            bodyDef.makeSensor = true;
        }
        else
        {
            bodyDef.bodyType = b2_dynamicBody;
            bodyDef.makeSensor = false;
        }
        
        bodyDef.fixtureType = FixtureType_Pickup;
        bodyDef.position = position;
        bodyDef.gravityScale = 0.8f;
        bodyDef.setInitialSpeed = true;
        bodyDef.initialSpeed = Point(speedX, speedY);
        bodyDef.collisionFlag = CollisionFlag_Pickup;
        bodyDef.collisionMask = (CollisionFlag_Death | CollisionFlag_Ground | CollisionFlag_Solid);
        bodyDef.density = 10.0f;
        bodyDef.friction = 0.18f;
        bodyDef.restitution = 0.5f;

        ActorFixtureDef fixtureDef;
        fixtureDef.fixtureType = FixtureType_Trigger;
        fixtureDef.collisionFlag = CollisionFlag_Pickup;
        fixtureDef.collisionMask = CollisionFlag_Controller | CollisionFlag_InvisibleController;
        fixtureDef.isSensor = true;
        bodyDef.fixtureList.push_back(fixtureDef);

        pActorElem->LinkEndChild(CreatePhysicsComponent(&bodyDef));

        if (!(TryGetValueFromMap(std::string("IsGlitter"), paramMap, std::string("")) == "false"))
        {
            pActorElem->LinkEndChild(CreateXmlData_GlitterComponent("Glitter_Yellow", isStatic, false));
        }

        if (!isStatic)
        {
            DestroyableComponentDef destroyableDef;
            destroyableDef.blinkOnDestruction = true;
            destroyableDef.deleteDelay = 3149;
            destroyableDef.deleteOnDestruction = true;
            destroyableDef.removeFromPhysics = true;
            pActorElem->LinkEndChild(CreateDestroyableComponent(destroyableDef));

            pActorElem->LinkEndChild(CreateHealthComponent(1, 1));
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_TreasurePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::Treasure, isStatic, paramMap);

        TiXmlElement* pTreasurePickupComponent = new TiXmlElement("TreasurePickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pTreasurePickupComponent);
        XML_ADD_TEXT_ELEMENT("ScorePoints", ToStr(GetScorePointsFromImageSet(imageSet)).c_str(), pTreasurePickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pTreasurePickupComponent);
        pActor->LinkEndChild(pTreasurePickupComponent);

        // Coins have animation
        if (imageSet.find("COIN") != std::string::npos)
        {
            pActor->LinkEndChild(CreateCycleAnimationComponent(99, false));
        }

        return pActor;
    }

    TiXmlElement* CreateXmlData_AmmoPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::AmmoPickup, isStatic, paramMap);

        TiXmlElement* pAmmoPickupComponent = new TiXmlElement("AmmoPickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pAmmoPickupComponent);
        std::pair<std::string, uint32> ammoPair = GetAmmoCountAndTypeFromImageset(imageSet);
        XML_ADD_2_PARAM_ELEMENT("Ammo", "ammoType", ammoPair.first.c_str(), "ammoCount", ammoPair.second, pAmmoPickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pAmmoPickupComponent);
        pActor->LinkEndChild(pAmmoPickupComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_LifePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::LifePickup, isStatic, paramMap);

        TiXmlElement* pLifePickupComponent = new TiXmlElement("LifePickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pLifePickupComponent);
        XML_ADD_TEXT_ELEMENT("Lives", ToStr(1).c_str(), pLifePickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pLifePickupComponent);
        pActor->LinkEndChild(pLifePickupComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_HealthPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::HealthPickup, isStatic, paramMap);

        TiXmlElement* pHealthPickupComponent = new TiXmlElement("HealthPickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pHealthPickupComponent);
        XML_ADD_TEXT_ELEMENT("Health", ToStr(GetHealthCountFromImageSet(imageSet)).c_str(), pHealthPickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pHealthPickupComponent);
        pActor->LinkEndChild(pHealthPickupComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_PowerupPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::PowerupPickup, isStatic, paramMap);

        TiXmlElement* pPowerupPickupComponent = new TiXmlElement("PowerupPickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pPowerupPickupComponent);
        std::pair<std::string, uint32> powerupPair = GetPowerupTypeAndDurationFromImageset(imageSet);
        if (IsKeyInMap(std::string("PowerupDuration"), paramMap))
        {
            powerupPair.second = std::stoul(StrictFindValueInMap(std::string("PowerupDuration"), paramMap));
        }
        XML_ADD_TEXT_ELEMENT("Type", powerupPair.first.c_str(), pPowerupPickupComponent);
        XML_ADD_TEXT_ELEMENT("Duration", ToStr(powerupPair.second).c_str(), pPowerupPickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pPowerupPickupComponent);
        pActor->LinkEndChild(pPowerupPickupComponent);

        if (pickupType != PickupType_Powerup_Catnip_1 &&
            pickupType != PickupType_Powerup_Catnip_2)
        {
            pActor->LinkEndChild(CreateCycleAnimationComponent(75, false));
        }

        return pActor;
    }

    TiXmlElement* CreateXmlData_WarpPickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::WarpPickup, isStatic, paramMap);

        std::string destinationX = StrictFindValueInMap(std::string("DestinationX"), paramMap);
        std::string destinationY = StrictFindValueInMap(std::string("DestinationY"), paramMap);

        TiXmlElement* pWarpPickupComponent = new TiXmlElement("TeleportPickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pWarpPickupComponent);
        AddXmlTextElement("IsBossWarp", pickupType == PickupType_BossWarp, pWarpPickupComponent);
        XML_ADD_2_PARAM_ELEMENT("Destination", "x", destinationX.c_str(), "y", destinationY.c_str(), pWarpPickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pWarpPickupComponent);
        pActor->LinkEndChild(pWarpPickupComponent);

        pActor->LinkEndChild(CreateCycleAnimationComponent(125));

        return pActor;
    }

    TiXmlElement* CreateXmlData_MappiecePickupActor(PickupType pickupType, const std::string& imageSet, const std::string& pickupSound, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        TiXmlElement* pActor = CreateXmlData_GeneralPickupActor(imageSet, position, zIndexes::EndLevelPickup, isStatic, paramMap);

        TiXmlElement* pMappiecePickupComponent = new TiXmlElement("EndLevelPickupComponent");
        AddXmlTextElement("PickupType", (int)pickupType, pMappiecePickupComponent);
        XML_ADD_TEXT_ELEMENT("PickupSound", pickupSound.c_str(), pMappiecePickupComponent);
        pActor->LinkEndChild(pMappiecePickupComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_PowerupSparkleActor(const std::string& imageSet, int cycleDuration)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(0, 0));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zIndexes::PowerupSparkle, false));
        pActor->LinkEndChild(CreateCycleAnimationComponent(cycleDuration));

        TiXmlElement* pPowerupSparkleAIComponent = new TiXmlElement("PowerupSparkleAIComponent");
        pActor->LinkEndChild(pPowerupSparkleAIComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_ClawProjectileActor(AmmoType ammoType, Direction direction, const Point& position, int sourceActorId, const Point& initialImpulse)
    {
        std::string imageSet = GetImageSetFromClawAmmoType(ammoType);

        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        Point speed(9.5, 0);
        if (direction == Direction_Left) { speed.x *= -1; }

        CollisionFlag collisionFlag = CollisionFlag_Bullet;
        if (ammoType == AmmoType_Magic) { collisionFlag = CollisionFlag_Magic; }
        else if (ammoType == AmmoType_Dynamite) { collisionFlag = CollisionFlag_Explosion; }

        bool hasInitialImpulse = ammoType == AmmoType_Dynamite;
        bool hasInitialSpeed = !hasInitialImpulse;

        int colMask = (CollisionFlag_Crate | CollisionFlag_PowderKeg | CollisionFlag_DynamicActor | CollisionFlag_Solid);
        int gravityScale = 0.0f;
        if (ammoType == AmmoType_Dynamite)
        {
            colMask = (CollisionFlag_Solid | CollisionFlag_Ground);
            gravityScale = 2.0f;
            speed = initialImpulse;
            if (direction == Direction_Left)
            {
                speed.x *= -1.0;
            }
        }

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zIndexes::ClawProjectile));
        pActor->LinkEndChild(CreatePhysicsComponent(
            "Dynamic",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            true,       // Has bullet behaviour ?
            ammoType != AmmoType_Dynamite,       // Has sensor behaviour ?
            "Projectile",  // Fixture type
            position,      // Position
            Point(0, 0),   // Offset - where to move the body upon its placement
            "Rectangle",   // Body shape - "Rectangle" or "Circle"
            Point(0, 0),   // Size - Leave blank if you want size to be determined by its default image
            gravityScale,          // Gravity scale - set to 0.0f if no gravity is desired
            hasInitialSpeed,          // Has any initial speed ?
            hasInitialImpulse,
            speed, // If it does, specify it here
            collisionFlag,  // Collision flag - e.g. What is this actor ?
            colMask,  // Collision mask - e.g. With what does this actor collide with ?
            10.0f,  // Density - determines if this character bounces
            0.5f, // Friction - with floor and so
            0.4f)); // Restitution - makes object bounce

        if (ammoType == AmmoType_Magic)
        {
            pActor->LinkEndChild(CreateCycleAnimationComponent(75));
        }
        else if (ammoType == AmmoType_Dynamite)
        {
            pActor->LinkEndChild(CreateAnimationComponent("/GAME/ANIS/DYNAMITELIT.ANI", false));
        }

        std::string projectileTypeStr;
        int32 damage = 0;
        if (ammoType == AmmoType_Pistol) { projectileTypeStr = "DamageType_Bullet"; damage = 20; }
        else if (ammoType == AmmoType_Magic) { projectileTypeStr = "DamageType_Magic"; damage = 50; }
        else if (ammoType == AmmoType_Dynamite) { projectileTypeStr = "DamageType_Explosion"; damage = 25; }
        else { assert(false && "Unknown projectile"); }

        TiXmlElement* pProjectileAIComponent = new TiXmlElement("ProjectileAIComponent");
        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pProjectileAIComponent);
        XML_ADD_TEXT_ELEMENT("ProjectileType", projectileTypeStr.c_str(), pProjectileAIComponent);
        XML_ADD_TEXT_ELEMENT("SourceActorId", ToStr(sourceActorId).c_str(), pProjectileAIComponent);
        XML_ADD_2_PARAM_ELEMENT("Speed", "x", ToStr(speed.x).c_str(), "y", "0", pProjectileAIComponent);
        if (ammoType == AmmoType_Dynamite)
        {
            XML_ADD_TEXT_ELEMENT("DetonationTime", "1000", pProjectileAIComponent);
            XML_ADD_TEXT_ELEMENT("NumSparkles", "10", pProjectileAIComponent);
        }
        else if (ammoType == AmmoType_Magic)
        {
            XML_ADD_TEXT_ELEMENT("NumSparkles", "25", pProjectileAIComponent);
        }
        pActor->LinkEndChild(pProjectileAIComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_ProjectileActor(const std::string& imageSet, uint32 damage, DamageType damageType, Direction direction, const Point& position, CollisionFlag collisionFlag, uint32 collisionMask, int sourceActorId)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        double speed = 9.5;
        if (direction == Direction_Left) { speed *= -1; }

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zIndexes::Projectile));

        ActorBodyDef bodyDef;
        bodyDef.bodyType = b2_dynamicBody;
        bodyDef.makeSensor = true;
        bodyDef.fixtureType = FixtureType_Projectile;
        bodyDef.collisionFlag = collisionFlag;
        bodyDef.collisionMask = collisionMask;
        bodyDef.gravityScale = 0.0f;
        bodyDef.setInitialSpeed = true;
        bodyDef.initialSpeed = Point(speed, 0);

        pActor->LinkEndChild(CreatePhysicsComponent(&bodyDef));

        TiXmlElement* pProjectileAIComponent = new TiXmlElement("ProjectileAIComponent");
        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pProjectileAIComponent);
        XML_ADD_TEXT_ELEMENT("ProjectileType", "DamageType_Bullet", pProjectileAIComponent);
        XML_ADD_TEXT_ELEMENT("SourceActorId", ToStr(sourceActorId).c_str(), pProjectileAIComponent);
        XML_ADD_2_PARAM_ELEMENT("Speed", "x", ToStr(speed).c_str(), "y", "0", pProjectileAIComponent);
        pActor->LinkEndChild(pProjectileAIComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_CrateActor(const std::string& imageSet, const Point& position, const std::vector<PickupType>& loot, uint32 health, int32 zCoord)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, zCoord));
        pActor->LinkEndChild(CreatePhysicsComponent(
            "Dynamic",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            true,       // Has bullet behaviour ?
            false,       // Has sensor behaviour ?
            "Crate",    // Fixture type
            position,      // Position
            Point(0, GetOffsetYByZCoord(zCoord)),   // Offset - where to move the body upon its placement
            "Rectangle",   // Body shape - "Rectangle" or "Circle"
            Point(44, 40),   // Size - Leave blank if you want size to be determined by its default image
            0.8f,          // Gravity scale - set to 0.0f if no gravity is desired
            true,          // Has any initial speed ?
            false,         // Has initial impulse ?
            Point(0, 0), // If it does, specify it here
            CollisionFlag_Crate,  // Collision flag - e.g. What is this actor ?
            (CollisionFlag_Crate | CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_Bullet | CollisionFlag_Magic | CollisionFlag_Explosion | CollisionFlag_ClawAttack | CollisionFlag_EnemyAIProjectile),  // Collision mask - e.g. With what does this actor collide with ?
            0.0f,  // Density - determines if this character bounces
            0.0f,  // Friction - with floor and so
            0.0f)); // Restitution - makes object bounce

        pActor->LinkEndChild(CreateCycleAnimationComponent(75, true));
        pActor->LinkEndChild(CreateLootComponent(loot, 33));
        pActor->LinkEndChild(CreateDestroyableComponent(true, "DEFAULT", { SOUND_GAME_CRATE_BREAK1, SOUND_GAME_CRATE_BREAK2 }));
        pActor->LinkEndChild(CreateHealthComponent(health, health));

        return pActor;
    }

    TiXmlElement* CreateXmlData_PowderKegActor(const std::string& imageSet, const Point& position, int32 damage, int32 zCoord)
    {
        // TODO: create xml actor prototype and implement lift-throw logic
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, zCoord));

        /*ActorBodyDef bodyDef;
        bodyDef.bodyType = b2_dynamicBody;
        bodyDef.makeSensor = false;
        bodyDef.collisionShape = "Rectangle";
        bodyDef.position = position;
        bodyDef.positionOffset = Point(0, -50);
        bodyDef.fixtureType = FixtureType_PowderKeg;
        bodyDef.collisionFlag = CollisionFlag_PowderKeg;
        bodyDef.collisionMask = (CollisionFlag_Crate | CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_Bullet | CollisionFlag_Explosion);
        bodyDef.gravityScale = 1.0f;

        pActor->LinkEndChild(CreatePhysicsComponent(&bodyDef));*/

        pActor->LinkEndChild(CreatePhysicsComponent(
            "Dynamic",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            true,       // Has bullet behaviour ?
            false,       // Has sensor behaviour ?
            "PowderKeg",    // Fixture typeactor
            position,      // Position
            Point(0, GetOffsetYByZCoord(zCoord)),   // Offset - where to move the body upon its placement
            "Rectangle",   // Body shape - "Rectangle" or "Circle"
            Point(0, 0),   // Size - Leave blank if you want size to be determined by its default image
            0.8f,          // Gravity scale - set to 0.0f if no gravity is desired
            false,          // Has any initial speed ?
            false,         // Has initial impulse ?
            Point(0, 0), // If it does, specify it here
            CollisionFlag_PowderKeg,  // Collision flag - e.g. What is this actor ?
            (CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_Bullet | CollisionFlag_Explosion),  // Collision mask - e.g. With what does this actor collide with ?
            0.0f,  // Friction - with floor and so
            0.0f,  // Density - determines if this character bounces
            0.0f)); // Restitution - makes object bounce

        //AddXmlTextElement("ClampToGround", true, pActor->FirstChildElement("PhysicsComponent"));

        pActor->LinkEndChild(CreateAnimationComponent("/LEVEL1/ANIS/POWDERKEG/EXPLODE.ANI", true));
        pActor->LinkEndChild(CreateDestroyableComponent(true, "explode", { SOUND_LEVEL1_KEG_EXPLODE }));
        pActor->LinkEndChild(CreateHealthComponent(1, 1));
        pActor->LinkEndChild(CreateExplodeableComponent(Point(140, 140), damage));

        return pActor;
    }

    TiXmlElement* CreateXmlData_AreaDamageActor(const Point& position, const Point& size, int32 damage, CollisionFlag collisionFlag, const std::string& shape, DamageType damageType,
            Direction hitDirection, int sourceActorId, const Point& positionOffset, const std::string& imageSet = "", zIndexes zCoord = zIndexes::MinIndex)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", "AreaDamage");

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        if (!imageSet.empty())
        {
            pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zCoord));
            pActor->LinkEndChild(CreateCycleAnimationComponent(75, false));
        }

        uint32 collisionMask = 0;
        if (collisionFlag == CollisionFlag_ClawAttack)
        {
            collisionMask = (CollisionFlag_Crate | CollisionFlag_DynamicActor);
        }
        else if (collisionFlag == CollisionFlag_EnemyAIAttack)
        {
            collisionMask = (CollisionFlag_Controller | CollisionFlag_InvisibleController);
        }
        else if (collisionFlag == CollisionFlag_Explosion)
        {
            collisionMask = (CollisionFlag_Crate | 
                CollisionFlag_PowderKeg | 
                CollisionFlag_DynamicActor | 
                CollisionFlag_Controller | 
                CollisionFlag_InvisibleController);
        }
        else
        {
            assert(false && "Unknown collision flag");
        }

        pActor->LinkEndChild(CreatePhysicsComponent(
            "Dynamic",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            true,       // Has bullet behaviour ?
            true,       // Has sensor behaviour ?
            "Trigger",    // Fixture type
            position,      // Position
            positionOffset,   // Offset - where to move the body upon its placement
            shape,         // Body shape - "Rectangle" or "Circle"
            size,          // Size - Leave blank if you want size to be determined by its default image
            0.0f,          // Gravity scale - set to 0.0f if no gravity is desired
            false,          // Has any initial speed ?
            false,         // Has initial impulse ?
            Point(0, 0), // If it does, specify it here
            collisionFlag,  // Collision flag - e.g. What is this actor ?
            collisionMask,  // Collision mask - e.g. With what does this actor collide with ?
            0.0f,  // Friction - with floor and so
            0.0f,  // Density - determines if this character bounces
            0.0f)); // Restitution - makes object bounce*/

        
        // TODO: This does not work for some reason
        /*ActorBodyDef bodyDef;
        bodyDef.bodyType = b2_dynamicBody;
        bodyDef.makeSensor = true;
        bodyDef.collisionShape = shape;
        bodyDef.position = position;
        bodyDef.positionOffset = positionOffset;
        bodyDef.size = size;
        bodyDef.fixtureType = FixtureType_Trigger;
        bodyDef.collisionFlag = collisionFlag;
        bodyDef.collisionMask = collisionMask;
        bodyDef.gravityScale = 0.0f;

        pActor->LinkEndChild(CreatePhysicsComponent(&bodyDef));*/

        pActor->LinkEndChild(CreateTriggerComponent(-1, false, false));
        pActor->LinkEndChild(CreateAreaDamageComponent(damage, 150, damageType, hitDirection, sourceActorId));

        return pActor;
    }

    TiXmlElement* CreateXmlData_CrumblingPeg(const std::string& imageSet, const Point& position, int32 zCoord)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, zCoord));
        pActor->LinkEndChild(CreateCycleAnimationComponent(50, true));
        pActor->LinkEndChild(CreatePhysicsComponent(
            "Static",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            true,       // Has bullet behaviour ?
            false,       // Has sensor behaviour ?
            "Ground",    // Fixture type
            position,      // Position
            Point(0, 0),   // Offset - where to move the body upon its placement
            "Rectangle",   // Body shape - "Rectangle" or "Circle"
            Point(0, 0),          // Size - Leave blank if you want size to be determined by its default image
            0.0f,          // Gravity scale - set to 0.0f if no gravity is desired
            false,          // Has any initial speed ?
            false,         // Has initial impulse ?
            Point(0, 0), // If it does, specify it here
            CollisionFlag_Ground,  // Collision flag - e.g. What is this actor ?
            // TODO:
            CollisionFlag_Controller | CollisionFlag_InvisibleController,  // Collision mask - e.g. With what does this actor collide with ?
            0.0f,  // Friction - with floor and so
            0.0f,  // Density - determines if this character bounces
            0.0f)); // Restitution - makes object bounce

        TiXmlElement* pCrumblingPegAIComponent = new TiXmlElement("CrumblingPegAIComponent");
        XML_ADD_TEXT_ELEMENT("FloorOffset", "10", pCrumblingPegAIComponent);
        XML_ADD_TEXT_ELEMENT("CrumbleFrameIdx", "10", pCrumblingPegAIComponent);
        pActor->LinkEndChild(pCrumblingPegAIComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_GlitterActor(const std::string& glitterType, const Point& position, zIndexes zCoord)
    {
        std::string imageSet = GetImageSetFromGlitterType(glitterType);

        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zCoord));
        pActor->LinkEndChild(CreateCycleAnimationComponent(99, false));

        return pActor;
    }

    TiXmlElement* CreateXmlData_ScorePopupActor(const Point& position, int score)
    {
        std::string imageSet = GetImageSetFromScoreCount(score);
        //std::string imageSet = "GAME_POINTS";

        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zIndexes::ScorePopup, true, false, false, false));

        PredefinedMove move;
        move.msDuration = 650;
        move.pixelsPerSecond = Point(0, -50);
        pActor->LinkEndChild(CreatePredefinedMoveComponent(move, false));

        return pActor;
    }

    TiXmlElement* CreateXmlData_CheckpointActor(const std::string& imageSet, const Point& position, int32 zCoord, const Point& spawnPosition, bool isSaveCheckpoint, uint32 saveCheckpointNumber)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, zCoord));
        pActor->LinkEndChild(CreateAnimationComponent("/GAME/ANIS/CHECKPOINT/*", true));
        pActor->LinkEndChild(CreateTriggerComponent(10, false, false));
        pActor->LinkEndChild(CreatePhysicsComponent(
            "Dynamic",  // Type - "Dynamic", "Kinematic", "Static"
            false,      // Has foot sensor ?
            false,      // Has capsule shape ?
            false,       // Has bullet behaviour ?
            false,       // Has sensor behaviour ?
            "Trigger",    // Fixture type
            position,      // Position
            Point(0, 0),   // Offset - where to move the body upon its placement
            "Rectangle",   // Body shape - "Rectangle" or "Circle"
            Point(0, 0),          // Size - Leave blank if you want size to be determined by its default image
            0.0f,          // Gravity scale - set to 0.0f if no gravity is desired
            false,          // Has any initial speed ?
            false,         // Has initial impulse ?
            Point(0, 0), // If it does, specify it here
            CollisionFlag_Checkpoint,  // Collision flag - e.g. What is this actor ?
            // TODO:
            CollisionFlag_Controller | CollisionFlag_InvisibleController,  // Collision mask - e.g. With what does this actor collide with ?
            0.0f,  // Friction - with floor and so
            0.0f,  // Density - determines if this character bounces
            0.0f)); // Restitution - makes object bounce

        TiXmlElement* pCheckpointComponent = new TiXmlElement("CheckpointComponent");
        XML_ADD_2_PARAM_ELEMENT("SpawnPosition", "x", ToStr(spawnPosition.x).c_str(), "y", ToStr(spawnPosition.y).c_str(), pCheckpointComponent);
        XML_ADD_TEXT_ELEMENT("IsSaveCheckpoint", ToStr(isSaveCheckpoint).c_str(), pCheckpointComponent);
        XML_ADD_TEXT_ELEMENT("SaveCheckpointNumber", ToStr(saveCheckpointNumber).c_str(), pCheckpointComponent);
        pActor->LinkEndChild(pCheckpointComponent);

        return pActor;
    }

    TiXmlElement* CreateXmlData_Actor(ActorPrototype proto, const Point &position)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_Actor(ActorPrototype actorProto, std::vector<XmlNodeOverride>& overrides)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(actorProto);
        assert(pActorElem != NULL);

        for (XmlNodeOverride& xmlOverride : overrides)
        {
            xmlOverride.Apply(pActorElem);
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_EnemyAIActor(ActorPrototype enemyType, const Point& position, const std::vector<PickupType>& loot, int32 minPatrolX, int32 maxPatrolX, bool isAlwaysIdle, bool isMirrored)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(enemyType);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        // ----------- RenderComponent - Mirrored ?
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.Mirrored", isMirrored));

        //----------- Loot
        TiXmlElement* pLootComponentElem = GetTiXmlElementFromPath(pActorElem, "Actor.LootComponent");
        assert(pLootComponentElem != NULL);
        for (PickupType item : loot)
        {
            XML_ADD_TEXT_ELEMENT("Item", ToStr((int)item).c_str(), pLootComponentElem);
        }

        //----------- Patrol borders
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.PatrolEnemyAIStateComponent.IsAlwaysIdle", isAlwaysIdle));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.PatrolEnemyAIStateComponent.LeftPatrolBorder", minPatrolX));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.PatrolEnemyAIStateComponent.RightPatrolBorder", maxPatrolX));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_ElevatorActor(ActorPrototype elevatorProto, const Point& position, const std::string& imagePath, const ElevatorDef& elevatorDef)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(elevatorProto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ActorRenderComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ImagePath", imagePath));

        //----------- Elevator properties
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.KinematicComponent.Speed",
            "x", (int)elevatorDef.speed.x, "y", (int)elevatorDef.speed.y));
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.KinematicComponent.MinPosition",
            "x", (int)elevatorDef.minPosition.x, "y", (int)elevatorDef.minPosition.y));
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.KinematicComponent.MaxPosition",
            "x", (int)elevatorDef.maxPosition.x, "y", (int)elevatorDef.maxPosition.y));

        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.KinematicComponent.HasTriggerBehaviour", elevatorDef.hasTriggerBehaviour));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.KinematicComponent.HasStartBehaviour", elevatorDef.hasStartBehaviour));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.KinematicComponent.HasStopBehaviour", elevatorDef.hasStopBehaviour));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.KinematicComponent.HasOneWayBehaviour", elevatorDef.hasOneWayBehaviour));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_TogglePegActor(ActorPrototype togglePegProto, const Point& position, const TogglePegDef& togglePegDef)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(togglePegProto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- TogglePeg properties
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.TogglePegAIComponent.Delay", togglePegDef.delay));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.TogglePegAIComponent.TimeOn", togglePegDef.timeOn));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.TogglePegAIComponent.TimeOff", togglePegDef.timeOff));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.TogglePegAIComponent.AlwaysOn", togglePegDef.isAlwaysOn));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_CrumblingPeg(ActorPrototype proto, const Point& position, const std::string& imagePath, int crumbleDelay)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ActorRenderComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ImagePath", imagePath));

        //----------- TogglePeg properties
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.CrumblingPegAIComponent.CrumbleDelay", crumbleDelay));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_LootContainer(ActorPrototype proto, const Point& position, const std::vector<PickupType>& loot, int zCoord)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- Z Coord
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ZCoord", zCoord));

        //----------- Loot
        TiXmlElement* pLootComponentElem = GetTiXmlElementFromPath(pActorElem, "Actor.LootComponent");
        assert(pLootComponentElem != NULL);
        for (PickupType item : loot)
        {
            XML_ADD_TEXT_ELEMENT("Item", ToStr((int)item).c_str(), pLootComponentElem);
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_ActorSpawner(ActorPrototype proto, const Point& position, const Point& spawnAreaOffset, const Point& spawnAreaSize, const std::vector<ActorSpawnInfo>& spawnedActorList)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- Physics
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PhysicsComponent.PositionOffset",
            "x", (int)spawnAreaOffset.x, "y", (int)spawnAreaOffset.y));
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PhysicsComponent.CollisionSize",
            "width", (int)spawnAreaSize.x, "height", (int)spawnAreaSize.y));

        //----------- SpawnAreaComponent
        TiXmlElement* pActorSpawnerComponent = GetTiXmlElementFromPath(pActorElem, "Actor.ActorSpawnerComponent");
        assert(pActorSpawnerComponent != NULL);
        for (const ActorSpawnInfo& spawnInfo : spawnedActorList)
        {
            TiXmlElement* pActorSpawnInfoElem = new TiXmlElement("ActorSpawnInfo");
            AddXmlTextElement("ActorPrototype", EnumToString_ActorPrototype(spawnInfo.actorProto), pActorSpawnInfoElem);
            XML_ADD_2_PARAM_ELEMENT("SpawnPositionOffset", 
                "x", ToStr(spawnInfo.spawnPositionOffset.x).c_str(), 
                "y", ToStr(spawnInfo.spawnPositionOffset.y).c_str(), pActorSpawnInfoElem);
            XML_ADD_2_PARAM_ELEMENT("InitialVelocity", 
                "x", ToStr(spawnInfo.initialVelocity.x).c_str(),
                "y", ToStr(spawnInfo.initialVelocity.y).c_str(), pActorSpawnInfoElem);

            pActorSpawnerComponent->LinkEndChild(pActorSpawnInfoElem);
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_ProjectileSpawner(ActorPrototype proto, const Point& position, Direction shootDir)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ProjectileSpawnerComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ProjectileSpawnerComponent.ProjectileDirection",
            EnumToString_Direction(shootDir)));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_ProjectileActor(ActorPrototype proto, const Point& position, Direction dir, int sourceActorId)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        TiXmlElement* pProjectileAIComponent = GetTiXmlElementFromPath(pActorElem, "Actor.ProjectileAIComponent");
        assert(pProjectileAIComponent != NULL);
        AddXmlTextElement("SourceActorId", sourceActorId, pProjectileAIComponent);

        if (dir == Direction_Left)
        {
            // Invert projectile speed when shooting left

            TiXmlElement* pProjectileSpeedElem = GetTiXmlElementFromPath(pActorElem, "Actor.ProjectileAIComponent.ProjectileSpeed");
            assert(pProjectileSpeedElem != NULL);

            Point projectileSpeed;
            DO_AND_CHECK(ParseValueFromXmlElem(&projectileSpeed, pProjectileSpeedElem, "x", "y"));
            projectileSpeed.x *= -1.0;

            DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.ProjectileAIComponent.ProjectileSpeed",
                "x", (int)projectileSpeed.x, "y", (int)projectileSpeed.y));

            // Mirror render component
            bool isMirroredByDefault = false;
            TiXmlElement* pIsMirroredElem = GetTiXmlElementFromPath(pActorElem, "Actor.ActorRenderComponent.Mirrored");
            if (pIsMirroredElem != NULL)
            {
                ParseValueFromXmlElem(&isMirroredByDefault, pIsMirroredElem);
            }

            DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.Mirrored", isMirroredByDefault == false));
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_StaticImage(ActorPrototype proto, const Point& position, const std::string& imagePath, const AnimationDef& aniDef)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ImagePath", imagePath));

        if (proto == ActorPrototype_StaticAnimatedImage && aniDef.hasAnimation)
        {
            DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.AnimationComponent.SpecialAnimation.Type", "cycle"));
            DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.AnimationComponent.SpecialAnimation.FrameDuration", aniDef.cycleAnimationDuration));
            DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.AnimationComponent.SpecialAnimation.HasPositionDelay", false));

            //pActorElem->Print(stdout, -1);
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_PathElevator(ActorPrototype proto, const Point& position, const std::string& imagePath, const PathElevatorDef& def)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ActorRenderComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ImagePath", imagePath));

        // ---------- PathElevatorComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.PathElevatorComponent.Speed", def.speed));

        TiXmlElement* pPathElevatorStepsElem = GetTiXmlElementFromPath(pActorElem, "Actor.PathElevatorComponent.ElevatorSteps");
        assert(pPathElevatorStepsElem != NULL);

        for (const ElevatorStepDef &stepDef : def.elevatorPath)
        {
            pPathElevatorStepsElem->LinkEndChild(stepDef.ToXml());
        }

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_Rope(ActorPrototype proto, const Point& position, const std::string& imagePath, const RopeDef& def)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ActorRenderComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.ActorRenderComponent.ImagePath", imagePath));

        //----------- AnimationComponent

        // All ropes have 120 animation frames
        int cycleDuration = def.timeToFlayBackAndForth / 120;
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.AnimationComponent.SpecialAnimation.FrameDuration", cycleDuration));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_SteppingGround(ActorPrototype proto, const Point& position, const SteppingGroundDef& def)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- ActorRenderComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.SteppingGroundComponent.TimeOn", def.timeOn));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.SteppingGroundComponent.TimeOff", def.timeOff));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_SpringBoard(ActorPrototype proto, const Point& position, const SpringBoardDef& def)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        //----------- SpringBoardComponent
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.SpringBoardComponent.SpringHeight", def.springHeight));

        return pActorElem;
    }

    StrongActorPtr CreateActor(ActorPrototype proto, const Point& position)
    {
        return CreateAndReturnActor(CreateXmlData_Actor(proto, position));
    }

    StrongActorPtr CreateActor_Projectile(ActorPrototype proto, const Point& position, Direction dir, int sourceActorId)
    {
        return CreateAndReturnActor(CreateXmlData_ProjectileActor(proto, position, dir, sourceActorId));
    }

    StrongActorPtr CreateActor_StaticImage(ActorPrototype proto, const Point& position, const std::string& imagePath, const AnimationDef& aniDef)
    {
        return CreateAndReturnActor(CreateXmlData_StaticImage(proto, position, imagePath, aniDef));
    }

    TiXmlElement* CreateXmlData_LocalAmbientSound(ActorPrototype proto, const Point& position, const LocalAmbientSoundDef& soundDef)
    {
        TiXmlElement* pActorElem = g_pApp->GetActorPrototypeElem(proto);
        assert(pActorElem != NULL);

        //----------- Position
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.PositionComponent.Position",
            "x", (int)position.x, "y", (int)position.y));

        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.LocalAmbientSoundComponent.Sound", soundDef.sound));
        DO_AND_CHECK(SetTiXmlNodeValue(pActorElem, "Actor.LocalAmbientSoundComponent.Volume", soundDef.volume));
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.LocalAmbientSoundComponent.SoundAreaSize",
            "width", (int)soundDef.soundAreaSize.x, "height", (int)soundDef.soundAreaSize.y));
        DO_AND_CHECK(SetTiXmlNode2Attribute(pActorElem, "Actor.LocalAmbientSoundComponent.SoundAreaOffset",
            "x", (int)soundDef.soundAreaOffset.x, "y", (int)soundDef.soundAreaOffset.y));

        return pActorElem;
    }

    TiXmlElement* CreateXmlData_EnemyAIActor(const std::string& imageSet, const std::string& animationSet, const Point& position, const std::vector<PickupType>& loot, const std::string& logicName, int32 zCoord, int32 minPatrolX, int32 maxPatrolX)
    {
        assert(false && "This method is deprecated. Use \"CreateXmlData_EnemyAIActor(ActorPrototype enemyType, Point position, const std::vector<PickupType>& loot, int32 minPatrolX, int32 maxPatrolX)\" instead");

        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", logicName.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, zCoord));
        pActor->LinkEndChild(CreateAnimationComponent(animationSet, false));
        pActor->LinkEndChild(CreateTriggerComponent(1000, false, false));
        pActor->LinkEndChild(CreateHealthComponent(50, 50));
        pActor->LinkEndChild(CreateLootComponent(loot));

        ActorBodyDef bodyDef;
        bodyDef.bodyType = b2_dynamicBody;
        bodyDef.makeSensor = false;
        bodyDef.makeCapsule = true;
        bodyDef.fixtureType = FixtureType_EnemyAI;
        //bodyDef.position = position;
        bodyDef.collisionFlag = CollisionFlag_DynamicActor;
        bodyDef.collisionMask = (CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_Death);
        //bodyDef.collisionMask = (CollisionFlag_Solid | CollisionFlag_Ground | CollisionFlag_Death | CollisionFlag_Controller | CollisionFlag_Bullet | CollisionFlag_Magic | CollisionFlag_ClawAttack);
        //bodyDef.collisionMask = 0xFFFFF;

        if (logicName == "Soldier" || logicName == "Officer")
        {
            bodyDef.size = Point(50, 100);
        }
        else if (logicName == "Rat")
        {
            bodyDef.size = Point(40, 41);
        }

        ActorFixtureDef fixtureDef;
        fixtureDef.fixtureType = FixtureType_EnemyAI;
        fixtureDef.collisionFlag = CollisionFlag_DynamicActor;
        fixtureDef.collisionMask = (CollisionFlag_Death | 
            CollisionFlag_Controller | 
            CollisionFlag_Bullet | 
            CollisionFlag_Magic | 
            CollisionFlag_ClawAttack | 
            CollisionFlag_Explosion |
            CollisionFlag_InvisibleController);
        fixtureDef.isSensor = true;
        fixtureDef.size = Point(40, 100); // Generic value
        if (logicName == "Rat")
        {
            fixtureDef.size = Point(40, 40);
        }
        bodyDef.fixtureList.push_back(fixtureDef);

        

        TiXmlElement* pEnemyAIElem = new TiXmlElement("EnemyAIComponent");
        if (logicName == "Soldier")
        {
            XML_ADD_TEXT_ELEMENT("DeathAnimation", "killfall", pEnemyAIElem);

            pActor->LinkEndChild(pEnemyAIElem);

            //=========================================================================================================
            // TakeDamage

            std::vector<std::string> takeDamageAnims = { "hithigh" };
            pActor->LinkEndChild(CreateXmlData_TakeDamageState(takeDamageAnims, 0));

            //=========================================================================================================
            // Patrol

            std::string walkAnimation = "fastadvance";

            std::vector<std::string> idleAnimations;
            idleAnimations.push_back("stand1");
            idleAnimations.push_back("stand");
            idleAnimations.push_back("stand2");

            pActor->LinkEndChild(CreateXmlData_PatrolState(750, 1.2, minPatrolX, maxPatrolX, walkAnimation, idleAnimations));

            //=========================================================================================================
            // Melee

            std::vector<EnemyAttackAction> meleeAttacks;

            EnemyAttackAction meleeAttackAction;
            meleeAttackAction.animation = "strike1";
            meleeAttackAction.attackAnimFrameIdx = 4;
            meleeAttackAction.attackDamageType = DamageType_MeleeAttack;
            meleeAttackAction.attackFxImageSet = "NONE";
            meleeAttackAction.attackSpawnPositionOffset = Point(-30, 0);
            meleeAttackAction.attackAreaSize = Point(110, 40);
            meleeAttackAction.damage = 10;

            meleeAttacks.push_back(meleeAttackAction);

            pActor->LinkEndChild(CreateXmlData_EnemyAttackActionState(meleeAttacks));

            //=========================================================================================================
            // Ranged

            std::vector<EnemyAttackAction> rangedAttacks;

            EnemyAttackAction rangedAttackAction;
            rangedAttackAction.animation = "strike";
            rangedAttackAction.attackAnimFrameIdx = 2;
            rangedAttackAction.attackDamageType = DamageType_Bullet;
            rangedAttackAction.attackFxImageSet = "/LEVEL1/IMAGES/MUSKETBALL/*";
            rangedAttackAction.attackSpawnPositionOffset = Point(-42, -38);
            rangedAttackAction.attackAreaSize = Point(0, 0);
            rangedAttackAction.damage = 10;

            rangedAttacks.push_back(rangedAttackAction);

            pActor->LinkEndChild(CreateXmlData_EnemyAttackActionState(rangedAttacks));


            bodyDef.fixtureList.push_back(
                CreateActorAgroRangeFixture(Point(180, 50), Point(0, 0), FixtureType_EnemyAIMeleeSensor));
            bodyDef.fixtureList.push_back(
                CreateActorAgroRangeFixture(Point(1000, 50), Point(0, -30), FixtureType_EnemyAIRangedSensor));
        }
        else if (logicName == "Officer")
        {
            XML_ADD_TEXT_ELEMENT("DeathAnimation", "fall", pEnemyAIElem);

            pActor->LinkEndChild(pEnemyAIElem);

            //=========================================================================================================
            // TakeDamage

            std::vector<std::string> takeDamageAnims = { "hithigh" };
            pActor->LinkEndChild(CreateXmlData_TakeDamageState(takeDamageAnims, 0));

            //=========================================================================================================
            // Patrol

            std::string walkAnimation = "fastadvance";

            std::vector<std::string> idleAnimations;
            idleAnimations.push_back("stand1");
            idleAnimations.push_back("stand2");
            idleAnimations.push_back("stand3");
            idleAnimations.push_back("stand4");
            idleAnimations.push_back("stand5");

            pActor->LinkEndChild(CreateXmlData_PatrolState(900, 1.3, minPatrolX, maxPatrolX, walkAnimation, idleAnimations));

            //=========================================================================================================
            // Melee

            std::vector<EnemyAttackAction> meleeAttacks;

            EnemyAttackAction meleeAttackAction;
            meleeAttackAction.animation = "strike";
            meleeAttackAction.attackAnimFrameIdx = 3;
            meleeAttackAction.attackDamageType = DamageType_MeleeAttack;
            meleeAttackAction.attackFxImageSet = "NONE";
            meleeAttackAction.attackSpawnPositionOffset = Point(-30, 0);
            meleeAttackAction.attackAreaSize = Point(130, 30);
            meleeAttackAction.damage = 10;

            meleeAttacks.push_back(meleeAttackAction);

            pActor->LinkEndChild(CreateXmlData_EnemyAttackActionState(meleeAttacks));


            bodyDef.fixtureList.push_back(
                CreateActorAgroRangeFixture(Point(180, 50), Point(0, 0), FixtureType_EnemyAIMeleeSensor));
        }
        else if (logicName == "Rat")
        {
            XML_ADD_TEXT_ELEMENT("DeathAnimation", "dead", pEnemyAIElem);

            pActor->LinkEndChild(pEnemyAIElem);

            //=========================================================================================================
            // Patrol

            std::string walkAnimation = "walk";

            std::vector<std::string> idleAnimations;

            pActor->LinkEndChild(CreateXmlData_PatrolState(900, 1.3, minPatrolX, maxPatrolX, walkAnimation, idleAnimations, true));

            //=========================================================================================================
            // Ranged

            std::vector<EnemyAttackAction> rangedAttacks;

            EnemyAttackAction rangedAttackAction;
            rangedAttackAction.animation = "throweastwest";
            rangedAttackAction.attackAnimFrameIdx = 2;
            rangedAttackAction.attackDamageType = DamageType_Bullet;
            rangedAttackAction.attackFxImageSet = "/LEVEL1/IMAGES/RATBOMB/*";
            rangedAttackAction.attackSpawnPositionOffset = Point(-15, 0);
            rangedAttackAction.attackAreaSize = Point(0, 0);
            rangedAttackAction.damage = 20;

            rangedAttacks.push_back(rangedAttackAction);

            pActor->LinkEndChild(CreateXmlData_EnemyAttackActionState(rangedAttacks));

            bodyDef.fixtureList.push_back(
                CreateActorAgroRangeFixture(Point(1000, 50), Point(0, 0), FixtureType_EnemyAIRangedSensor));
        }

        // Add sounds associated to given enemy
        auto soundTypeAndNamePairs = GetSoundsFromActorLogic(logicName);
        for (std::pair<std::string, std::string> &soundTypeAndNamePair : soundTypeAndNamePairs)
        {
            XML_ADD_2_PARAM_ELEMENT("Sound", "SoundType", soundTypeAndNamePair.first.c_str(), 
                "SoundName", soundTypeAndNamePair.second.c_str(), pEnemyAIElem);
        }

        pActor->LinkEndChild(CreatePhysicsComponent(&bodyDef));

        // Damage aura - if Claw enters it, he gets damaged
        DamageAuraComponentDef auraDef;
        auraDef.baseAuraComponentDef.applyAuraOnEnter = true;
        auraDef.baseAuraComponentDef.isGroupPulse = false;
        auraDef.baseAuraComponentDef.isPulsating = true;
        auraDef.baseAuraComponentDef.removeActorAfterPulse = false;
        auraDef.baseAuraComponentDef.pulseIntrval = 2000;

        auraDef.baseAuraComponentDef.auraFixtureDef.collisionFlag = CollisionFlag_DamageAura;
        auraDef.baseAuraComponentDef.auraFixtureDef.collisionMask = CollisionFlag_Controller | CollisionFlag_InvisibleController;
        auraDef.baseAuraComponentDef.auraFixtureDef.collisionShape = "Rectangle";
        auraDef.baseAuraComponentDef.auraFixtureDef.fixtureType = FixtureType_DamageAura;
        auraDef.baseAuraComponentDef.auraFixtureDef.isSensor = true;
        // Size should be the same as body
        auraDef.baseAuraComponentDef.auraFixtureDef.size = Point(bodyDef.size.x - 10, bodyDef.size.y - 15);

        auraDef.damage = 10;

        pActor->LinkEndChild(DamageAuraComponentDefToXml(&auraDef));

        /*TiXmlNode* pDup = pActor->Clone();
        TiXmlElement* pDuplicate = pDup->ToElement();
        pDuplicate->Print(stdout, -1);*/

        return pActor;
    }

    //=====================================================================================================================
    // Public API
    //=====================================================================================================================

    StrongActorPtr CreateActor_Trigger(const ActorBodyDef& triggerBodyDef, const Point& position)
    {
        TiXmlElement* pActorElem = new TiXmlElement("Actor");

        pActorElem->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActorElem->LinkEndChild(CreatePhysicsComponent(&triggerBodyDef));
        pActorElem->LinkEndChild(CreateTriggerComponent(1000, false, false));

        return CreateAndReturnActor(pActorElem);
    }

    StrongActorPtr CreateSingleAnimation(const Point& position, AnimationType animType)
    {
        TiXmlElement* pActorElem = new TiXmlElement("Actor");

        pActorElem->LinkEndChild(CreatePositionComponent(position.x, position.y));

        std::string imageSet;
        switch (animType)
        {
            case AnimationType_Explosion:
                imageSet = "GAME_EXPLOS_FIRE";
                pActorElem->LinkEndChild(CreateCycleAnimationComponent(50));
                break;

            case AnimationType_RedHitPoint:
                imageSet = "GAME_ENEMYHIT";
                pActorElem->LinkEndChild(CreateCycleAnimationComponent(50));
                break;

            case AnimationType_BlueHitPoint:
                imageSet = "GAME_CLAWHIT";
                pActorElem->LinkEndChild(CreateCycleAnimationComponent(50));
                break;

            case AnimationType_TridentExplosion:
                imageSet = "LEVEL_TRIDENT_TRIDENTEXPLOSION";
                pActorElem->LinkEndChild(CreateCycleAnimationComponent(50));
                break;

            case AnimationType_TarSplash:
                imageSet = "LEVEL_SPLASH";
                pActorElem->LinkEndChild(CreateCycleAnimationComponent(75));
                break;

            default:
                LOG_ERROR("Unknown AnimationType: " + ToStr((int)animType));
                return nullptr;
                break;
        }

        pActorElem->SetAttribute("Type", imageSet.c_str());

        pActorElem->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zIndexes::SingleAnimation));

        TiXmlElement* pSingleAnimComponentElem = new TiXmlElement("SingleAnimationComponent");
        pActorElem->LinkEndChild(pSingleAnimComponentElem);

        return CreateAndReturnActor(pActorElem);
    }

    TiXmlElement* CreateXmlData_PickupActor(PickupType pickupType, const Point& position, bool isStatic, const ParamMap& paramMap)
    {
        assert(pickupType >= PickupType_Default && pickupType < PickupType_Max);

        std::string imageSet = EnumToString_PickupTypeToImageSet(pickupType);
        if (imageSet.empty())
        {
            LOG_ERROR("Could not get valid image set for pickup type: " + ToStr(pickupType));
            return NULL;
        }

        std::string pickupSound = g_PickupTypeToPickupSoundMap[pickupType];
        if (pickupSound.empty())
        {
            LOG_ERROR("Could not get valid pickup sound for pickup type: " + ToStr(pickupType));
            return NULL;
        }

        TiXmlElement* pActorXmlData = NULL;
        if (g_PickupCreationTable[pickupType].creationFunction != NULL)
        {
            pActorXmlData = g_PickupCreationTable[pickupType].creationFunction(pickupType, imageSet, pickupSound, position, isStatic, paramMap);
        }
        else
        {
            LOG_ERROR("No creation function for pickup with image set: " + imageSet);
            assert(false);
        }

        return pActorXmlData;
    }

    StrongActorPtr CreateActorPickup(PickupType pickupType, const Point& position, bool isStatic)
    {
        return CreateAndReturnActor(CreateXmlData_PickupActor(pickupType, position, isStatic));
    }

    StrongActorPtr CreateRenderedActor(const Point& position, const std::string& imageSet, const std::string& animPath, zIndexes zCoord)
    {
        TiXmlElement* pActor = new TiXmlElement("Actor");
        pActor->SetAttribute("Type", imageSet.c_str());

        pActor->LinkEndChild(CreatePositionComponent(position.x, position.y));
        pActor->LinkEndChild(CreateActorRenderComponent(imageSet, (int32) zCoord, true, false, false, false));
        if (!animPath.empty())
        {
            pActor->LinkEndChild(CreateAnimationComponent(animPath, true));
        }

        return CreateAndReturnActor(pActor);
    }

    StrongActorPtr CreatePowerupSparkleActor(int cycleDuration)
    {
        return CreateAndReturnActor(CreateXmlData_PowerupSparkleActor("GAME_SPARKLE", cycleDuration));
    }

    StrongActorPtr CreateClawProjectile(AmmoType ammoType, Direction direction, const Point& position, int sourceActorId, const Point& initialImpulse)
    {
        return CreateAndReturnActor(CreateXmlData_ClawProjectileActor(ammoType, direction, position, sourceActorId, initialImpulse));
    }

    StrongActorPtr CreateProjectile(
        const std::string& imageSet,
        uint32 damage, 
        DamageType damageType, 
        Direction direction, 
        const Point& position,
        CollisionFlag collisionFlag, 
        uint32 collisionMask,
        int sourceActorId)
    {
        return CreateAndReturnActor(CreateXmlData_ProjectileActor(
            imageSet, 
            damage, 
            damageType, 
            direction, 
            position, 
            collisionFlag, 
            collisionMask,
            sourceActorId));
    }

    StrongActorPtr CreateAreaDamage(const Point& position, const Point& size, int32 damage, CollisionFlag collisionFlag, const std::string& shape, DamageType damageType, Direction hitDirection, int sourceActorId, const Point& positionOffset, const std::string& imageSet, zIndexes zCoord)
    {
        return CreateAndReturnActor(CreateXmlData_AreaDamageActor(position, size, damage, collisionFlag, shape, damageType, hitDirection, sourceActorId, positionOffset, imageSet, zCoord));
    }

    StrongActorPtr CreateGlitter(const std::string& glitterType, const Point& position, zIndexes zCoord)
    {
        return CreateAndReturnActor(CreateXmlData_GlitterActor(glitterType, position, zCoord));
    }

    StrongActorPtr CreateScorePopupActor(const Point& position, int score)
    {
        return CreateAndReturnActor(CreateXmlData_ScorePopupActor(position, score));
    }

    // From XML to Struct

    ActorFixtureDef XmlToActorFixtureDef(TiXmlElement* pActorFixtureDefElem)
    {
        assert(pActorFixtureDefElem != NULL);

        ActorFixtureDef fixtureDef;

        std::string fixtureTypeStr;
        SetStringIfDefined(&fixtureTypeStr, pActorFixtureDefElem->FirstChildElement("FixtureType"));
        fixtureDef.fixtureType = FixtureTypeStringToEnum(fixtureTypeStr);

        ParseValueFromXmlElem(&fixtureDef.collisionShape, pActorFixtureDefElem->FirstChildElement("CollisionShape"));
        ParseValueFromXmlElem(&fixtureDef.isSensor, pActorFixtureDefElem->FirstChildElement("IsSensor"));
        SetPointIfDefined(&fixtureDef.size, pActorFixtureDefElem->FirstChildElement("Size"), "width", "height");
        SetPointIfDefined(&fixtureDef.offset, pActorFixtureDefElem->FirstChildElement("Offset"), "x", "y");
        SetEnumIfDefined(&fixtureDef.collisionFlag, pActorFixtureDefElem->FirstChildElement("CollisionFlag"), CollisionFlag);
        ParseValueFromXmlElem(&fixtureDef.collisionMask, pActorFixtureDefElem->FirstChildElement("CollisionMask"));
        ParseValueFromXmlElem(&fixtureDef.friction, pActorFixtureDefElem->FirstChildElement("Friction"));
        ParseValueFromXmlElem(&fixtureDef.density, pActorFixtureDefElem->FirstChildElement("Density"));
        ParseValueFromXmlElem(&fixtureDef.restitution, pActorFixtureDefElem->FirstChildElement("Restitution"));

        return fixtureDef;
    }

    TiXmlElement* ActorFixtureDefToXml(const ActorFixtureDef* pFixtureDef)
    {
        assert(pFixtureDef != NULL);

        TiXmlElement* pFixtureElem = new TiXmlElement("ActorFixture");

        XML_ADD_TEXT_ELEMENT("FixtureType", FixtureTypeToString(pFixtureDef->fixtureType).c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("CollisionShape", pFixtureDef->collisionShape.c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("IsSensor", ToStr(pFixtureDef->isSensor).c_str(), pFixtureElem);
        XML_ADD_2_PARAM_ELEMENT("Size", "width", pFixtureDef->size.x, "height", pFixtureDef->size.y, pFixtureElem);
        XML_ADD_2_PARAM_ELEMENT("Offset", "x", pFixtureDef->offset.x, "y", pFixtureDef->offset.y, pFixtureElem);
        XML_ADD_TEXT_ELEMENT("CollisionFlag", ToStr(pFixtureDef->collisionFlag).c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("CollisionMask", ToStr(pFixtureDef->collisionMask).c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("Friction", ToStr(pFixtureDef->friction).c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("Density", ToStr(pFixtureDef->density).c_str(), pFixtureElem);
        XML_ADD_TEXT_ELEMENT("Restitution", ToStr(pFixtureDef->restitution).c_str(), pFixtureElem);

        return pFixtureElem;
    }

    void AppendBaseAuraComponentDefToXml(const BaseAuraComponentDef* pAuraDef, TiXmlElement* pRoot)
    {
        XML_ADD_TEXT_ELEMENT("IsPulsating", ToStr(pAuraDef->isPulsating).c_str(), pRoot);
        XML_ADD_TEXT_ELEMENT("IsGroupPulse", ToStr(pAuraDef->isGroupPulse).c_str(), pRoot);
        XML_ADD_TEXT_ELEMENT("ApplyAuraOnEnter", ToStr(pAuraDef->applyAuraOnEnter).c_str(), pRoot);
        XML_ADD_TEXT_ELEMENT("RemoveActorAfterPulse", ToStr(pAuraDef->removeActorAfterPulse).c_str(), pRoot);
        XML_ADD_TEXT_ELEMENT("PulseInterval", ToStr(pAuraDef->pulseIntrval).c_str(), pRoot);

        // Fixture
        pRoot->LinkEndChild(ActorFixtureDefToXml(&pAuraDef->auraFixtureDef));
    }

    TiXmlElement* DamageAuraComponentDefToXml(const DamageAuraComponentDef* pAuraDef)
    {
        TiXmlElement* pRoot = new TiXmlElement("DamageAuraComponent");

        // Base
        AppendBaseAuraComponentDefToXml(&pAuraDef->baseAuraComponentDef, pRoot);

        // Specific aura
        XML_ADD_TEXT_ELEMENT("Damage", ToStr(pAuraDef->damage).c_str(), pRoot);

        return pRoot;
    }
};