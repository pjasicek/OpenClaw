#include "TriggerComponent.h"
#include "../PositionComponent.h"
#include "../../../GameApp/BaseGameApp.h"
#include "../../../GameApp/BaseGameLogic.h"
#include "../RenderComponent.h"
#include "../../../Graphics2D/Image.h"
#include "../../../Physics/ClawPhysics.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* TriggerComponent::g_Name = "TriggerComponent";

TriggerComponent::TriggerComponent()
    :
    /*m_IsTriggerOnce(false),
    m_TriggerRemaining(0),*/
    m_IsTriggerUnlimited(true),
    m_Size(Point(0, 0)),
    m_IsStatic(false)
{ }

TriggerComponent::~TriggerComponent()
{
    m_pPhysics->VRemoveActor(m_pOwner->GetGUID());
}

bool TriggerComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    m_pPhysics = g_pApp->GetGameLogic()->VGetGamePhysics();
    if (!m_pPhysics)
    {
        LOG_ERROR("Invalid physics");
        return false;
    }

    ParseValueFromXmlElem(&m_Size, data->FirstChildElement("Size"), "width", "height");
    ParseValueFromXmlElem(&m_IsStatic, data->FirstChildElement("IsStatic"));
    ParseValueFromXmlElem(&m_IsTriggerUnlimited, data->FirstChildElement("IsTriggerUnlimited"));

    /*if (TiXmlElement* pElem = data->FirstChildElement("IsTriggerOnce"))
    {
        m_IsTriggerOnce = std::string(pElem->GetText()) == "true";
    }
    if (TiXmlElement* pElem = data->FirstChildElement("IsTriggerFinitedTimes"))
    {
        m_TriggerRemaining = std::stoi(std::string(pElem->GetText()));
    }*/

    //ParseValueFromXmlElem(&m_EnterSound, data->FirstChildElement("EnterSound"));

    for (TiXmlElement* pElem = data->FirstChildElement("EnterSound");
        pElem != NULL;
        pElem = pElem->NextSiblingElement("EnterSound"))
    {
        std::string fixtureTypeStr;
        std::string sound;

        DO_AND_CHECK(ParseAttributeFromXmlElem(&fixtureTypeStr, "TriggerFixtureType", pElem));
        DO_AND_CHECK(ParseAttributeFromXmlElem(&sound, "Sound", pElem));

        FixtureType fixtureType = FixtureTypeStringToEnum(fixtureTypeStr);

        m_TriggerEnterSoundMap.insert(std::make_pair(fixtureType, sound));
    }

    for (TiXmlElement* pElem = data->FirstChildElement("LeaveSound");
        pElem != NULL;
        pElem = pElem->NextSiblingElement("LeaveSound"))
    {
        std::string fixtureTypeStr;
        std::string sound;

        DO_AND_CHECK(ParseAttributeFromXmlElem(&fixtureTypeStr, "TriggerFixtureType", pElem));
        DO_AND_CHECK(ParseAttributeFromXmlElem(&sound, "Sound", pElem));

        FixtureType fixtureType = FixtureTypeStringToEnum(fixtureTypeStr);

        m_TriggerLeaveSoundMap.insert(std::make_pair(fixtureType, sound));
    }

    return true;
}

void TriggerComponent::VPostInit()
{
    if (m_IsStatic)
    {
        LOG_ERROR("Creating physics body for fixture like this is deprecated ! Offending actor: " + m_pOwner->GetName());
        assert(false && "Deprecated !");

        int offsetX = 0;
        int offsetY = 0;

        // Set size from current image if necessary
        if (fabs(m_Size.x) < DBL_EPSILON || fabs(m_Size.y) < DBL_EPSILON)
        {
            shared_ptr<ActorRenderComponent> pRenderComponent =
                MakeStrongPtr(m_pOwner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
            //assert(pRenderComponent);
            if (pRenderComponent == nullptr)
            {
                return;
            }

            shared_ptr<Image> pImage = MakeStrongPtr(pRenderComponent->GetCurrentImage());

            m_Size.x = pImage->GetWidth();
            m_Size.y = pImage->GetHeight();

            offsetX = pImage->GetOffsetX();
            offsetY = pImage->GetOffsetY();
        }

        shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
        assert(pPositionComponent);

        Point physPos = Point(pPositionComponent->GetX() + offsetX, pPositionComponent->GetY() + offsetY);

        m_pPhysics->VCreateTrigger(m_pOwner, physPos , m_Size, m_IsStatic);
    }
}

TiXmlElement* TriggerComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void TriggerComponent::OnActorEntered(Actor* pActor, FixtureType triggerType)
{
    if (m_DeactivatedTriggerTypesMap[triggerType] == true)
    {
        return;
    }

    // Part of the actor may already be inside the trigger. If it is, ignore it
    if (HasOverlappingActor(pActor))
    {
        AddOverlappingActor(pActor);
        return;
    }

    auto findIt = m_TriggerEnterSoundMap.find(triggerType);
    if (findIt != m_TriggerEnterSoundMap.end())
    {
        SoundInfo soundInfo(findIt->second);
        soundInfo.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
        soundInfo.setPositionEffect = true;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(soundInfo)));
    }

    AddOverlappingActor(pActor);

    NotifyEnterTrigger(pActor, triggerType);

    /*m_TriggerRemaining--;
    if (!m_IsTriggerUnlimited && (m_IsTriggerOnce || (m_TriggerRemaining <= 0)))
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }*/

    /*if (!m_IsTriggerUnlimited)
    {
        shared_ptr<EventData_Destroy_Actor> pEvent(new EventData_Destroy_Actor(m_pOwner->GetGUID()));
        IEventMgr::Get()->VQueueEvent(pEvent);
    }*/
}

void TriggerComponent::OnActorLeft(Actor* pActor, FixtureType triggerType)
{
    if (m_DeactivatedTriggerTypesMap[triggerType] == true)
    {
        return;
    }

    RemoveOverlappingActor(pActor);
    if (HasOverlappingActor(pActor))
    {
        return;
    }

    auto findIt = m_TriggerLeaveSoundMap.find(triggerType);
    if (findIt != m_TriggerLeaveSoundMap.end())
    {
        SoundInfo soundInfo(findIt->second);
        soundInfo.soundSourcePosition = m_pOwner->GetPositionComponent()->GetPosition();
        soundInfo.setPositionEffect = true;
        IEventMgr::Get()->VTriggerEvent(IEventDataPtr(new EventData_Request_Play_Sound(soundInfo)));
    }

    NotifyLeaveTrigger(pActor, triggerType);
}

SDL_Rect TriggerComponent::GetTriggerArea()
{
    SDL_Rect triggerArea = { 0 };

    shared_ptr<PositionComponent> pPositionComponent = m_pOwner->GetPositionComponent();
    if (!pPositionComponent)
    {
        return triggerArea;
    }

    Point triggerPosition = pPositionComponent->GetPosition();
    triggerArea = { (int)triggerPosition.x, (int)triggerPosition.y, (int)m_Size.x, (int)m_Size.y };

    return triggerArea;
}

void TriggerComponent::AddOverlappingActor(Actor* pActor)
{
    m_ActorsInsideList.push_back(pActor);
}

void TriggerComponent::RemoveOverlappingActor(Actor* pActor)
{
    for (auto iter = m_ActorsInsideList.begin(); iter != m_ActorsInsideList.end(); ++iter)
    {
        if ((*iter) == pActor)
        {
            m_ActorsInsideList.erase(iter);
            return;
        }
    }
    LOG_WARNING("Could not remove overlapping actor - no such actor found")
}

bool TriggerComponent::HasOverlappingActor(Actor* pActor)
{
    if (std::find(m_ActorsInsideList.begin(), m_ActorsInsideList.end(), pActor) != m_ActorsInsideList.end())
    {
        return true;
    }

    return false;
}

//=====================================================================================================================
// TriggerSubject implementation
//=====================================================================================================================

void TriggerSubject::NotifyEnterTrigger(Actor* pActorWhoEntered, FixtureType triggerType)
{
    for (TriggerObserver* pObserver : m_Observers)
    {
        pObserver->VOnActorEnteredTrigger(pActorWhoEntered, triggerType);
    }
}

void TriggerSubject::NotifyLeaveTrigger(Actor* pActorWhoLeft, FixtureType triggerType)
{
    for (TriggerObserver* pObserver : m_Observers)
    {
        pObserver->VOnActorLeftTrigger(pActorWhoLeft, triggerType);
    }
}
