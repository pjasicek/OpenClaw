#ifndef __ACTOR_TEMPLATES_H__
#define __ACTOR_TEMPLATES_H__

#include "../SharedDefines.h"

// Structs for creating components
struct BaseAuraComponentDef
{
    BaseAuraComponentDef()
    {
        isPulsating = false;
        isGroupPulse = false;
        applyAuraOnEnter = true;
        removeActorAfterPulse = false;
        pulseIntrval = 0;
    }

    ActorFixtureDef auraFixtureDef;
    bool isPulsating;
    bool isGroupPulse;
    bool applyAuraOnEnter;
    bool removeActorAfterPulse;
    int pulseIntrval;
};

struct DamageAuraComponentDef
{
    DamageAuraComponentDef()
    {
        damage = 0;
    }

    BaseAuraComponentDef baseAuraComponentDef;
    int damage;
};

typedef std::map<std::string, std::string> ParamMap;

class XmlNodeOverride
{
public:
    XmlNodeOverride(const std::string& xmlNodePath)
        :
        m_XmlNodePath(xmlNodePath)
    {

    }

    //
    // Most commonly used
    //

    template <typename T>
    XmlNodeOverride(const std::string& xmlNodePath, const T& nodeValue)
        :
        m_XmlNodePath(xmlNodePath)
    {
        m_NodeText = ToStr(nodeValue);
    }

    template <typename T>
    XmlNodeOverride(const std::string& xmlNodePath, 
        const std::string& key, const T& value)
        :
        m_XmlNodePath(xmlNodePath)
    {
        std::string valStr = ToStr(value);

        m_ValueKeyToValueMap.insert(std::make_pair(key, valStr));
    }

    template <typename T>
    XmlNodeOverride(const std::string& xmlNodePath, 
        const std::string& key1, const T& value1,
        const std::string& key2, const T& value2)
        :
        m_XmlNodePath(xmlNodePath)
    {
        std::string valStr1 = ToStr(value1);
        std::string valStr2 = ToStr(value2);

        m_ValueKeyToValueMap.insert(std::make_pair(key1, valStr1));
        m_ValueKeyToValueMap.insert(std::make_pair(key2, valStr2));
    }

    template <typename T>
    inline void SetNodeText(const T& nodeValue) { m_NodeText = ToStr(nodeValue); }

    template <typename T>
    inline void AddValue(const std::string& valueKey, const T& value)
    {
        std::string valStr = ToStr(value);

        m_ValueKeyToValueMap.insert(std::make_pair(valueKey, valStr));
    }

    bool Apply(TiXmlElement* pRootElem)
    {
        bool bIsApplied = false;
        if (!m_NodeText.empty())
        {
            if (!SetTiXmlNodeValue(pRootElem, m_XmlNodePath, m_NodeText))
            {
                LOG_ASSERT("Could not set: " + m_XmlNodePath + " to value: [" + m_NodeText + "]");
            }
            bIsApplied = true;
        }

        for (const auto& keyValuePair : m_ValueKeyToValueMap)
        {
            if (!SetTiXmlNode1Attribute(pRootElem, m_XmlNodePath, keyValuePair.first, keyValuePair.second))
            {
                LOG_ASSERT("Could not set: " + m_XmlNodePath + " to keypair: [" + keyValuePair.first + "," + keyValuePair.second + "]");
            }
            bIsApplied = true;
        }

        return bIsApplied;
    }

private:
    XmlNodeOverride() { }

    std::string m_XmlNodePath;
    std::string m_NodeText;
    std::map<std::string, std::string> m_ValueKeyToValueMap;
};

enum class zIndexes : int32 {
    MinIndex = 0,
    ScorePopup = 0,
    SingleAnimation = 0,
    Glitter = 1010,
    FollowableComponent = 1020,
    ClawProjectile = 2000,
    Treasure = 4000,
    AmmoPickup = 4000,
    LifePickup = 4000,
    HealthPickup = 4000,
    PowerupPickup = 4000,
    WarpPickup = 4000,
    ClawActor = 4000,
    EndLevelPickup = 5000,
    Projectile = 5000,
    PowerupSparkle = 8000,
    MaxIndex = 9000
};

namespace ActorTemplates
{
    // Actor prototypes
    TiXmlElement* CreateXmlData_Actor(ActorPrototype actorProto, const Point &position);
    TiXmlElement* CreateXmlData_Actor(ActorPrototype actorProto, std::vector<XmlNodeOverride>& overrides);
    TiXmlElement* CreateXmlData_EnemyAIActor(ActorPrototype enemyType, const Point& position, const std::vector<PickupType>& loot, int32 minPatrolX, int32 maxPatrolX, bool isAlwaysIdle, bool isMirrored);
    TiXmlElement* CreateXmlData_ElevatorActor(ActorPrototype elevatorProto, const Point& position, const std::string& imagePath, const ElevatorDef& elevatorDef);
    TiXmlElement* CreateXmlData_TogglePegActor(ActorPrototype togglePegProto, const Point& position, const TogglePegDef& togglePegDef);
    TiXmlElement* CreateXmlData_ProjectileActor(ActorPrototype proto, const Point& position, Direction dir, int sourceActorId);
    TiXmlElement* CreateXmlData_LocalAmbientSound(ActorPrototype proto, const Point& position, const LocalAmbientSoundDef& soundDef);
    TiXmlElement* CreateXmlData_StaticImage(ActorPrototype proto, const Point& position, const std::string& imagePath, const AnimationDef& aniDef);
    TiXmlElement* CreateXmlData_PathElevator(ActorPrototype proto, const Point& position, const std::string& imagePath, const PathElevatorDef& def);
    TiXmlElement* CreateXmlData_Rope(ActorPrototype proto, const Point& position, const std::string& imagePath, const RopeDef& def);
    TiXmlElement* CreateXmlData_SteppingGround(ActorPrototype proto, const Point& position, const SteppingGroundDef& def);
    TiXmlElement* CreateXmlData_SpringBoard(ActorPrototype proto, const Point& position, const SpringBoardDef& def);
    TiXmlElement* CreateXmlData_CrumblingPeg(ActorPrototype proto, const Point& position, const std::string& imagePath, int crumbleDelay);
    TiXmlElement* CreateXmlData_LootContainer(ActorPrototype proto, const Point& position, const std::vector<PickupType>& loot, int zCoord);
    TiXmlElement* CreateXmlData_ActorSpawner(ActorPrototype proto, const Point& position, const Point& spawnOffset, const Point& spawnSize, const std::vector<ActorSpawnInfo>& spawnedActorList);
    TiXmlElement* CreateXmlData_ProjectileSpawner(ActorPrototype proto, const Point& position, Direction shootDir);

    StrongActorPtr CreateActor(ActorPrototype proto, const Point& position);
    StrongActorPtr CreateActor_Projectile(ActorPrototype proto, const Point& position, Direction dir, int sourceActorId);

    StrongActorPtr CreateActor_StaticImage(ActorPrototype proto, const Point& position, const std::string& imagePath, const AnimationDef& aniDef);

    StrongActorPtr CreateActor_Trigger(const ActorBodyDef& triggerBodyDef, const Point& position);

    // Exposed Component Xml data generating functions
    TiXmlElement* CreateXmlData_GlitterComponent(const std::string &glitterType, bool spawnImmediate, bool followOwner);
    TiXmlElement* CreatePhysicsComponent(const ActorBodyDef* pBodyDef);
    TiXmlElement* CreatePredefinedMoveComponent(std::vector<PredefinedMove>& moves, bool isInfinite);
    TiXmlElement* CreateFollowableComponent(const Point& offset, const std::string &imageSet, const std::string &animPath);

    TiXmlElement* CreateDamageAuraComponent(const DamageAuraComponentDef &aurDef);

    // Exposed Actor Xml data generating functions.
    TiXmlElement* CreateXmlData_CrateActor(const std::string &imageSet, const Point& position, const std::vector<PickupType>& loot, uint32 health, int32 zCoord);
    TiXmlElement* CreateXmlData_PowderKegActor(const std::string &imageSet, const Point& position, int32 damage, int32 zCoord);
    TiXmlElement* CreateXmlData_CrumblingPeg(const std::string &imageSet, const Point& position, int32 zCoord);
    TiXmlElement* CreateXmlData_GlitterActor(const std::string &glitterType, const Point& position, zIndexes zCoord);
    TiXmlElement* CreateXmlData_CheckpointActor(const std::string &imageSet, const Point& position, int32 zCoord, const Point& spawnPosition, bool isSaveCheckpoint, uint32 saveCheckpointNumber);
    TiXmlElement* CreateXmlData_EnemyAIActor(const std::string &imageSet, const std::string &animationSet, const Point& position, const std::vector<PickupType>& loot, const std::string &logicName, int32 zCoord, int32 minPatrolX, int32 maxPatrolX);
    
    TiXmlElement* CreateXmlData_PickupActor(PickupType pickupType, const Point& position, bool isStatic, const ParamMap& paramMap = ParamMap());

    TiXmlElement* CreateXmlData_SoundTriggerActor(const std::string& sound, const Point& position, const Point& size, int enterCount, bool activateDialog = false);
    TiXmlElement* CreateXmlData_SoundTriggerActor(const std::string& sound, const std::string& logicName, const Point& position, const SDL_Rect &presetPosition, int enterCount);

    TiXmlElement* CreateXmlData_GlobalAmbientSoundActor(const std::string& sound, int soundVolume, int minTimeOff, int maxTimeOff, int minTimeOn, int maxTimeOn, bool isLooping);

    // This is used by the game itself
    StrongActorPtr CreateActorPickup(PickupType pickupType, const Point& position, bool isStatic = false);
    StrongActorPtr CreatePowerupSparkleActor(int cycleDuration);
    StrongActorPtr CreateClawProjectile(AmmoType ammoType, Direction direction, const Point& position, int sourceActorId, const Point& initialImpulse);
    StrongActorPtr CreateProjectile(const std::string &imageSet, uint32 damage, DamageType damageType, Direction direction, const Point& position, CollisionFlag collisionFlag, uint32 collisionMask, int sourceActorId);
    StrongActorPtr CreateAreaDamage(const Point& position, const Point& size, int32 damage, CollisionFlag collisionFlag, const std::string& shape, DamageType damageType, Direction hitDirection, int sourceActorId, const Point& positionOffset = Point(0, 0), const std::string& imageSet = "", zIndexes zCoord = zIndexes::MinIndex);
    StrongActorPtr CreateGlitter(const std::string& glitterType, const Point& position, zIndexes zCoord = zIndexes::Glitter);
    //StrongActorPtr CreatePopupActor(Point position, std::string imageSet, std::vector<PredefinedMove>& moves, int32 zCoord = 0);
    StrongActorPtr CreateScorePopupActor(const Point& position, int score);
    StrongActorPtr CreateRenderedActor(const Point& position, const std::string &imageSet, const std::string& animPath, zIndexes zCoord);

    // This represents (or atleast should represent) single animation, e.g. explosion
    StrongActorPtr CreateSingleAnimation(const Point& position, AnimationType animType);

    ActorFixtureDef XmlToActorFixtureDef(TiXmlElement* pActorFixtureDefElem);
    TiXmlElement* ActorFixtureDefToXml(const ActorFixtureDef* pFixtureDef);

    TiXmlElement* DamageAuraComponentDefToXml(const DamageAuraComponentDef* pAuraDef);

    std::string GetSoundPathFromClawPath(const std::string& sound);

    PickupType StringToEnum_ImageSetToPickupType(const std::string& pickupImageSet);
    std::string EnumToString_PickupTypeToImageSet(PickupType pickupType);
};

#endif
