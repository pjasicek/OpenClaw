#ifndef __ACTOR_DEFINITIONS_H__
#define __ACTOR_DEFINITIONS_H__

#include "Interfaces.h"
#include "Util/Point.h"

//-------------------------------------------------------------------------------------------------
// ActorFixtureDef - Physics
//-------------------------------------------------------------------------------------------------

struct ActorFixtureDef
{
    ActorFixtureDef()
    {
        fixtureType = FixtureType_None;
        collisionShape = "Rectangle";
        isSensor = false;
        size = Point(0, 0);
        offset = Point(0, 0);

        friction = 0.0f;
        density = 0.0f;
        restitution = 0.0f;

        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
    }

    FixtureType fixtureType;
    std::string collisionShape;
    bool isSensor;
    Point size;
    Point offset;

    float friction;
    float density;
    float restitution;

    CollisionFlag collisionFlag;
    uint32 collisionMask;
};

//-------------------------------------------------------------------------------------------------
// ActorBodyDef - Physics
//-------------------------------------------------------------------------------------------------

struct ActorBodyDef
{
    ActorBodyDef()
    {
        bodyType = b2_dynamicBody;
        addFootSensor = false;
        makeCapsule = false;
        makeBullet = false;
        makeSensor = true;
        fixtureType = FixtureType_None;
        position = Point(0, 0);
        positionOffset = Point(0, 0);
        collisionShape = "Rectangle";
        size = Point(0, 0);
        gravityScale = 1.0f;
        setInitialSpeed = false;
        setInitialImpulse = false;
        initialSpeed = Point(0, 0);
        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
        prefabType = "";

        friction = 0.0f;
        density = 0.0f;
        restitution = 0.0f;

        isActive = true;
    }

    WeakActorPtr pActor;
    b2BodyType bodyType;
    bool addFootSensor;
    bool makeCapsule;
    bool makeBullet;
    bool makeSensor;
    FixtureType fixtureType;
    Point position;
    Point positionOffset;
    std::string collisionShape;
    Point size;
    float gravityScale;
    bool setInitialSpeed;
    bool setInitialImpulse;
    Point initialSpeed;
    CollisionFlag collisionFlag;
    uint32 collisionMask;
    float friction;
    float density;
    float restitution;
    std::string prefabType;
    // TODO: This is a bit hacky - used for ducking 
    bool isActive;

    std::vector<ActorFixtureDef> fixtureList;
};

//-------------------------------------------------------------------------------------------------
// ElevatorDef - KinematicComponent
//-------------------------------------------------------------------------------------------------

struct ElevatorDef
{
    ElevatorDef()
    {
        hasTriggerBehaviour = false;
        hasStartBehaviour = false;
        hasStopBehaviour = false;
        hasOneWayBehaviour = false;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pElevatorComponent = new TiXmlElement("KinematicComponent");

        XML_ADD_2_PARAM_ELEMENT("Speed", "x", ToStr(speed.x).c_str(), "y", ToStr(speed.y).c_str(), pElevatorComponent);
        XML_ADD_2_PARAM_ELEMENT("MinPosition", "x", ToStr(minPosition.x).c_str(), "y", ToStr(minPosition.y).c_str(), pElevatorComponent);
        XML_ADD_2_PARAM_ELEMENT("MaxPosition", "x", ToStr(maxPosition.x).c_str(), "y", ToStr(maxPosition.y).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasTriggerBehaviour", ToStr(hasTriggerBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasStartBehaviour", ToStr(hasStartBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasStopBehaviour", ToStr(hasStopBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasOneWayBehaviour", ToStr(hasOneWayBehaviour).c_str(), pElevatorComponent);

        return pElevatorComponent;
    }

    static ElevatorDef CreateFromXml(TiXmlElement* pElem)
    {
        ElevatorDef def;
        def.LoadFromXml(pElem);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem)
    {
        SetPointIfDefined(&speed, pElem->FirstChildElement("Speed"), "x", "y");
        SetPointIfDefined(&minPosition, pElem->FirstChildElement("MinPosition"), "x", "y");
        SetPointIfDefined(&maxPosition, pElem->FirstChildElement("MaxPosition"), "x", "y");
        ParseValueFromXmlElem(&hasTriggerBehaviour, pElem->FirstChildElement("HasTriggerBehaviour"));
        ParseValueFromXmlElem(&hasStartBehaviour, pElem->FirstChildElement("HasStartBehaviour"));
        ParseValueFromXmlElem(&hasStopBehaviour, pElem->FirstChildElement("HasStopBehaviour"));
        ParseValueFromXmlElem(&hasOneWayBehaviour, pElem->FirstChildElement("HasOneWayBehaviour"));
    }

    Point speed;
    Point minPosition;
    Point maxPosition;

    bool hasTriggerBehaviour;
    bool hasStartBehaviour;
    bool hasStopBehaviour;
    bool hasOneWayBehaviour;
};

//-------------------------------------------------------------------------------------------------
// TogglePegDef - TogglePegAIComponent
//-------------------------------------------------------------------------------------------------

struct TogglePegDef
{
    TogglePegDef()
    {
        isAlwaysOn = false;
        timeOff = 0;
        timeOn = 0;
        toggleFrameIdx = 0;
        delay = 0;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pTogglePegComponent = new TiXmlElement("TogglePegAIComponent");

        XML_ADD_TEXT_ELEMENT("AlwaysOn", ToStr(isAlwaysOn).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("TimeOff", ToStr(timeOff).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("TimeOn", ToStr(timeOn).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("ToggleFrameIdx", ToStr(toggleFrameIdx).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("Delay", ToStr(delay).c_str(), pTogglePegComponent);

        return pTogglePegComponent;
    }

    static TogglePegDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        TogglePegDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&isAlwaysOn, pElem->FirstChildElement("AlwaysOn")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOff, pElem->FirstChildElement("TimeOff")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOn, pElem->FirstChildElement("TimeOn")));
        cond_assert(strict, ParseValueFromXmlElem(&toggleFrameIdx, pElem->FirstChildElement("ToggleFrameIdx")));
        cond_assert(strict, ParseValueFromXmlElem(&toggleSound, pElem->FirstChildElement("ToggleSound")));
        cond_assert(strict, ParseValueFromXmlElem(&delay, pElem->FirstChildElement("Delay")));
    }

    // All time values in miliseconds (ms)

    bool isAlwaysOn;
    int timeOff;
    int timeOn;
    int toggleFrameIdx;
    std::string toggleSound;
    int delay;
};

//-------------------------------------------------------------------------------------------------
// CrumblingPegDef - CrumblingPegAIComponent
//-------------------------------------------------------------------------------------------------

struct CrumblingPegDef
{
    CrumblingPegDef()
    {
        crumbleDelay = 0;
        crumbleFrameIdx = 0;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pTogglePegComponent = new TiXmlElement("CrumblingPegAIComponent");

        assert(false && "Unimplemented");

        return pTogglePegComponent;
    }

    static CrumblingPegDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        CrumblingPegDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&crumbleDelay, pElem->FirstChildElement("CrumbleDelay")));
        cond_assert(strict, ParseValueFromXmlElem(&crumbleFrameIdx, pElem->FirstChildElement("CrumbleFrameIdx")));
        cond_assert(strict, ParseValueFromXmlElem(&crumbleSound, pElem->FirstChildElement("CrumbleSound")));
    }

    int crumbleDelay;
    int crumbleFrameIdx;
    std::string crumbleSound;
};

//-------------------------------------------------------------------------------------------------
// ProjectileDef - ProjectileAIComponent
//-------------------------------------------------------------------------------------------------

struct ProjectileDef
{
    ProjectileDef()
    {
        damage = 0;
        damageTypeStr = "DamageType_None";
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pProjectileComponent = new TiXmlElement("ProjectileAIComponent");

        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pProjectileComponent);
        XML_ADD_TEXT_ELEMENT("ProjectileType", damageTypeStr.c_str(), pProjectileComponent);
        XML_ADD_2_PARAM_ELEMENT("ProjectileSpeed", "x", ToStr(projectileSpeed.x).c_str(), "y", ToStr(projectileSpeed.y).c_str(), pProjectileComponent);

        return pProjectileComponent;
    }

    static ProjectileDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        ProjectileDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&damage, pElem->FirstChildElement("Damage")));
        cond_assert(strict, ParseValueFromXmlElem(&damageTypeStr, pElem->FirstChildElement("ProjectileType")));
        cond_assert(strict, ParseValueFromXmlElem(&projectileSpeed, pElem->FirstChildElement("ProjectileSpeed"), "x", "y"));
    }

    int damage;
    std::string damageTypeStr;
    Point projectileSpeed;
};

//-------------------------------------------------------------------------------------------------
// ProjectileSpawnerDef - ProjectileSpawnerComponent
//-------------------------------------------------------------------------------------------------

struct ProjectileSpawnerDef
{
    ProjectileSpawnerDef()
    {
        isAlwaysOn = false;
        projectileProto = ActorPrototype_None;
        projectileDirection = Direction_None;
    }

    static ProjectileSpawnerDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        ProjectileSpawnerDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        // TODO:
        return NULL;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&idleAnim, pElem->FirstChildElement("IdleAnim")));
        cond_assert(strict, ParseValueFromXmlElem(&fireAnim, pElem->FirstChildElement("FireAnim")));
        cond_assert(strict, ParseValueFromXmlElem(&isAlwaysOn, pElem->FirstChildElement("IsAlwaysOn")));
        cond_assert(strict, ParseValueFromXmlElem(&startSpawnDelay, pElem->FirstChildElement("StartSpawnDelay")));
        cond_assert(strict, ParseValueFromXmlElem(&minSpawnDelay, pElem->FirstChildElement("MinSpawnDelay")));
        cond_assert(strict, ParseValueFromXmlElem(&maxSpawnDelay, pElem->FirstChildElement("MaxSpawnDelay")));

        cond_assert(strict, ParseValueFromXmlElem(&triggerAreaSize, pElem->FirstChildElement("TriggerAreaSize"), "width", "height"));
        cond_assert(strict, ParseValueFromXmlElem(&triggerAreaOffset, pElem->FirstChildElement("TriggerAreaOffset"), "x", "y"));
        cond_assert(strict, ParseValueFromXmlElem(&triggerCollisionMask, pElem->FirstChildElement("TriggerCollisionMask")));

        cond_assert(strict, ParseValueFromXmlElem(&projectileSpawnAnimFrameIdx, pElem->FirstChildElement("ProjectileSpawnAnimFrameIdx")));

        std::string projectileProtoStr;
        cond_assert(strict, ParseValueFromXmlElem(&projectileProtoStr, pElem->FirstChildElement("ProjectilePrototype")));
        projectileProto = StringToEnum_ActorPrototype(projectileProtoStr);

        std::string directionStr;
        cond_assert(strict, ParseValueFromXmlElem(&directionStr, pElem->FirstChildElement("ProjectileDirection")));
        projectileDirection = StringToEnum_Direction(directionStr);

        cond_assert(strict, ParseValueFromXmlElem(&projectileSpawnOffset, pElem->FirstChildElement("ProjectileSpawnOffset"), "x", "y"));
    }

    std::string idleAnim;
    std::string fireAnim;
    bool isAlwaysOn;
    int startSpawnDelay;
    int minSpawnDelay;
    int maxSpawnDelay;
    Point triggerAreaSize;
    Point triggerAreaOffset;
    uint32 triggerCollisionMask;
    int projectileSpawnAnimFrameIdx;
    ActorPrototype projectileProto;
    Direction projectileDirection;
    Point projectileSpawnOffset;
};

struct LocalAmbientSoundDef
{
    LocalAmbientSoundDef()
    {
    }

    static LocalAmbientSoundDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        LocalAmbientSoundDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        // TODO:
        return NULL;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&sound, pElem->FirstChildElement("Sound")));
        cond_assert(strict, ParseValueFromXmlElem(&volume, pElem->FirstChildElement("Volume")));
        cond_assert(strict, ParseValueFromXmlElem(&soundAreaSize, pElem->FirstChildElement("SoundAreaSize"), "width", "height"));
        cond_assert(strict, ParseValueFromXmlElem(&soundAreaOffset, pElem->FirstChildElement("SoundAreaOffset"), "x", "y"));
    }

    std::string sound;
    int volume;
    Point soundAreaSize;
    Point soundAreaOffset;
};

//-------------------------------------------------------------------------------------------------
// DestroyableComponentDef - DestroyableComponent
//-------------------------------------------------------------------------------------------------

struct DestroyableComponentDef
{
    DestroyableComponentDef()
    {
        deleteImmediately = false;
        deleteDelay = 0;
        blinkOnDestruction = false;
        deleteOnDestruction = true;
        removeFromPhysics = true;
        deathAnimationName = "";
    }

    bool deleteImmediately;
    int deleteDelay;
    bool blinkOnDestruction;
    bool deleteOnDestruction;
    bool removeFromPhysics;
    std::string deathAnimationName;
    std::vector<std::string> deathSoundList;
};

//-------------------------------------------------------------------------------------------------
// ElevatorStepDef - PathElevatorComponent
//-------------------------------------------------------------------------------------------------

struct ElevatorStepDef
{
    ElevatorStepDef()
    {
        direction = Direction_None;
        isWaiting = false;
        waitMsTime = 0;
    }

    static ElevatorStepDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        ElevatorStepDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml() const
    {
        TiXmlElement* pElevatorStepElem = new TiXmlElement("ElevatorStep");

        AddXmlTextElement("Direction", EnumToString_Direction(direction), pElevatorStepElem);
        AddXmlTextElement("StepDeltaDistance", stepDeltaDistance, pElevatorStepElem);
        AddXmlTextElement("IsWaiting", isWaiting, pElevatorStepElem);
        AddXmlTextElement("WaitTime", waitMsTime, pElevatorStepElem);

        return pElevatorStepElem;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        std::string directionStr;
        cond_assert(strict, ParseValueFromXmlElem(&directionStr, pElem->FirstChildElement("Direction")));
        direction = StringToEnum_Direction(directionStr);

        cond_assert(strict, ParseValueFromXmlElem(&stepDeltaDistance, pElem->FirstChildElement("StepDeltaDistance")));
        cond_assert(strict, ParseValueFromXmlElem(&isWaiting, pElem->FirstChildElement("IsWaiting")));
        cond_assert(strict, ParseValueFromXmlElem(&waitMsTime, pElem->FirstChildElement("WaitTime")));
    }

    Direction direction;
    int stepDeltaDistance;

    bool isWaiting;
    int waitMsTime;

    Point destinationPosition;
};

//-------------------------------------------------------------------------------------------------
// PathElevatorDef - PathElevatorComponent
//-------------------------------------------------------------------------------------------------

struct PathElevatorDef
{
    PathElevatorDef()
    {
        speed = 0.0;
    }

    static PathElevatorDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        PathElevatorDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pPathElevatorComponent = new TiXmlElement("PathElevatorComponent");

        AddXmlTextElement("Speed", speed, pPathElevatorComponent);

        TiXmlElement* pPathElem = new TiXmlElement("ElevatorSteps");
        pPathElevatorComponent->LinkEndChild(pPathElem);

        for (ElevatorStepDef& step : elevatorPath)
        {
            pPathElem->LinkEndChild(step.ToXml());
        }

        return pPathElevatorComponent;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&speed, pElem->FirstChildElement("Speed")));

        TiXmlElement* pElevatorStepsElem = pElem->FirstChildElement("ElevatorSteps");
        assert(pElevatorStepsElem != NULL);
        for (TiXmlElement* pStepElem = pElevatorStepsElem->FirstChildElement("ElevatorStep");
            pStepElem != NULL;
            pStepElem = pStepElem->NextSiblingElement("ElevatorStep"))
        {
            ElevatorStepDef def;
            def.LoadFromXml(pStepElem, strict);

            elevatorPath.push_back(def);
        }
    }

    double speed;
    std::vector<ElevatorStepDef> elevatorPath;
};

//-------------------------------------------------------------------------------------------------
// RopeDef - RopeComponent
//-------------------------------------------------------------------------------------------------

struct RopeDef
{
    RopeDef()
    {
        timeToFlayBackAndForth = 0;
    }

    static RopeDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        RopeDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        assert(false);
        return NULL;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&timeToFlayBackAndForth, pElem->FirstChildElement("TimeToFlayBackAndForth")));
    }

    int timeToFlayBackAndForth;
};

//-------------------------------------------------------------------------------------------------
// SteppingGroundDef - SteppingGroundComponent
//-------------------------------------------------------------------------------------------------

struct SteppingGroundDef
{
    SteppingGroundDef()
    {
        toggleFrameIdx = 0;
        timeOn = 0;
        timeOff = 0;
    }

    static SteppingGroundDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        SteppingGroundDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        assert(false);
        return NULL;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&toggleFrameIdx, pElem->FirstChildElement("ToggleFrameIdx")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOn, pElem->FirstChildElement("TimeOn")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOff, pElem->FirstChildElement("TimeOff")));
        cond_assert(strict, ParseValueFromXmlElem(&toggleSound, pElem->FirstChildElement("ToggleSound")));
    }

    int toggleFrameIdx;
    int timeOn;
    int timeOff;
    std::string toggleSound;
};

//-------------------------------------------------------------------------------------------------
// SpringBoardDef - SpringBoardComponent
//-------------------------------------------------------------------------------------------------

struct SpringBoardDef
{
    SpringBoardDef()
    {
        springHeight = 0.0;
        springFrameIdx = 0;
        springDelay = 0;
        springSound = "/GAME/SOUNDS/NULL.WAV";
    }

    static SpringBoardDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        SpringBoardDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    TiXmlElement* ToXml()
    {
        assert(false);
        return NULL;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&springHeight, pElem->FirstChildElement("SpringHeight")));
        cond_assert(strict, ParseValueFromXmlElem(&springFrameIdx, pElem->FirstChildElement("SpringFrameIdx")));
        cond_assert(strict, ParseValueFromXmlElem(&idleAnimName, pElem->FirstChildElement("IdleAnimName")));
        cond_assert(strict, ParseValueFromXmlElem(&springAnimName, pElem->FirstChildElement("SpringAnimName")));
        cond_assert(strict, ParseValueFromXmlElem(&springDelay, pElem->FirstChildElement("SpringDelay")));
        ParseValueFromXmlElem(&springSound, pElem->FirstChildElement("SpringSound"));
    }

    double springHeight;
    int springFrameIdx;
    int springDelay;
    std::string idleAnimName;
    std::string springAnimName;
    std::string springSound;
};

//-------------------------------------------------------------------------------------------------
// AnimationDef - AnimationComponent
//-------------------------------------------------------------------------------------------------

struct AnimationDef
{
    AnimationDef()
    {
        hasAnimation = false;
        isCycleAnimation = false;
        cycleAnimationDuration = 0;
    }

    bool hasAnimation;
    bool isCycleAnimation;
    int cycleAnimationDuration;
    std::string animationPath;
};

//-------------------------------------------------------------------------------------------------
// CollisionInfo - Contains collisonFlag an collisionMask for PhysicsComponent
//-------------------------------------------------------------------------------------------------

struct CollisionInfo
{
    CollisionInfo()
    {
        collisionFlag = CollisionFlag_None;
        collisionMask = 0xFFFFFFFF;
    }

    CollisionInfo(CollisionFlag colFlag, uint32 colMask)
    {
        collisionFlag = colFlag;
        collisionMask = colMask;
    }

    CollisionFlag collisionFlag;
    uint32 collisionMask;
};

//-------------------------------------------------------------------------------------------------
// EnemyAttackAction - EnemyAIComponent, EnemyAIStateComponent
//-------------------------------------------------------------------------------------------------

struct EnemyAttackAction
{
    EnemyAttackAction()
    {
        attackAnimFrameIdx = 0;
        attackDamageType = DamageType_None;
        projectileProto = ActorPrototype_None;
        attackFxImageSet = "NONE";
        isMirrored = false;
    }

    ActorPrototype projectileProto;

    std::string animation;
    uint32 attackAnimFrameIdx;
    std::string attackFxImageSet;
    bool isMirrored;

    DamageType attackDamageType;

    // Used to spawn area damage

    // Considering enemy is looking left
    Point attackSpawnPositionOffset;

    // Applicable only to melee attacks
    Point attackAreaSize;

    // Damage which the attack will deal to enemies
    uint32 damage;

    // Enemy agro range bound to this attack action - attack action
    // can only be performed when enemy is within it
    ActorFixtureDef agroSensorFixture;
};

//-------------------------------------------------------------------------------------------------
// PredefinedMove - EnemyAIComponent, EnemyAIStateComponent
//-------------------------------------------------------------------------------------------------

struct PredefinedMove
{
    PredefinedMove()
    {
        msDuration = 0;
        pixelsPerSecond = Point(0, 0);
        soundToPlay = "";
    }

    uint32 msDuration;
    Point pixelsPerSecond;
    std::string soundToPlay;
};

//-------------------------------------------------------------------------------------------------
// SoundInfo - Used as definition of sound being played
//-------------------------------------------------------------------------------------------------

struct SoundInfo
{
    SoundInfo()
    {
        isMusic = false;
        soundVolume = 100;
        loops = 0;
        setPositionEffect = false;
        setDistanceEffect = false;
        maxHearDistance = 0;
        attentuationFactor = 1.0f;
    }

    SoundInfo(std::string sound) : SoundInfo()
    {
        soundToPlay = sound;
    }

    std::string soundToPlay;
    bool isMusic;
    int soundVolume;
    int loops;
    bool setPositionEffect;
    bool setDistanceEffect;
    float maxHearDistance;
    float attentuationFactor;
    Point soundSourcePosition;
};

struct SoundProperties
{
    SoundProperties()
    {
        volume = 100;
        loops = 0;
        angle = 0;
        distance = 0;
    }

    int volume;
    int loops;
    int angle;
    int distance;
};

struct ActorSpawnInfo
{
    ActorSpawnInfo()
    {
        actorProto = ActorPrototype_None;
    }

    ActorPrototype actorProto;
    Point spawnPositionOffset;
    Point initialVelocity;
};

#endif