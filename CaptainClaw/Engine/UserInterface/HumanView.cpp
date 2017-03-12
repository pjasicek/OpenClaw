#include "HumanView.h"
#include "../GameApp/BaseGameApp.h"
#include "../GameApp/BaseGameLogic.h"
#include "../Events/EventMgr.h"
#include "../Events/Events.h"
#include "../Audio/Audio.h"
#include "../Resource/Loaders/MidiLoader.h"
#include "../Resource/Loaders/WavLoader.h"

const uint32 g_InvalidGameViewId = 0xFFFFFFFF;

HumanView::HumanView(SDL_Renderer* renderer)
{
    m_pProcessMgr = new ProcessMgr();

    m_ViewId = INVALID_GAME_VIEW_ID;

    RegisterAllDelegates();

    if (renderer)
    {
        m_pScene.reset(new ScreenElementScene(renderer));
        m_pCamera.reset(new CameraNode(Point(0, 0), 0, 0));
        m_pHUD.reset(new ScreenElementHUD());

        m_pScene->AddChild(INVALID_ACTOR_ID, m_pCamera);
        m_pScene->SetCamera(m_pCamera);

        //m_pConsole = unique_ptr<Console>(new Console(g_pApp->GetWindowSize().x, g_pApp->GetWindowSize().y / 2,
            //g_pApp->GetConsoleFont(), renderer, "console02.tga"));

        m_pConsole = unique_ptr<Console>(new Console(g_pApp->GetConsoleConfig(), renderer, g_pApp->GetWindow()));
    }
}

HumanView::~HumanView()
{
    RemoveAllDelegates();

    while (!m_ScreenElements.empty())
    {
        m_ScreenElements.pop_front();
    }

    SAFE_DELETE(m_pProcessMgr);
}

void HumanView::RegisterConsoleCommandHandler(void(*handler)(const char*, void*), void* userdata)
{
    if (m_pConsole)
    {
        m_pConsole->SetCommandHandler(handler, (void*)m_pConsole.get());
    }
}

void HumanView::VOnRender(uint32 msDiff)
{
    //PROFILE_CPU("HumanView Render");

    m_CurrentTick = SDL_GetTicks();
    if (m_CurrentTick == m_LastDraw)
    {
        return;
    }

    SDL_Renderer* renderer = g_pApp->GetRenderer();

    if (m_RunFullSpeed || true)
    {
        //PROFILE_CPU(".");
        SDL_RenderClear(renderer);

        // Sort screen elements
        m_ScreenElements.sort(SortBy_SharedPtr_Content<IScreenElement>());

        for (shared_ptr<IScreenElement> screenElement : m_ScreenElements)
        {
            
            if (screenElement->VIsVisible())
            {
                screenElement->VOnRender(msDiff);
            }
        }

        g_pApp->GetGameLogic()->VRenderDiagnostics(renderer, m_pCamera);

        m_pConsole->OnRender(renderer);

        SDL_RenderPresent(renderer);
    }
}

void HumanView::VOnUpdate(uint32 msDiff)
{
    m_pProcessMgr->UpdateProcesses(msDiff);

    m_pConsole->OnUpdate(msDiff);

    for (shared_ptr<IScreenElement> element : m_ScreenElements)
    {
        element->VOnUpdate(msDiff);
    }
}

bool HumanView::VOnEvent(SDL_Event& evt)
{
    // First let console try to eat this event
    if (m_pConsole->OnEvent(evt))
    {
        //LOG("command eaten !");
        return true;
    }

    // Then screen layers in reverse order
    for (ScreenElementList::reverse_iterator i = m_ScreenElements.rbegin(); i != m_ScreenElements.rend(); ++i)
    {
        if ((*i)->VIsVisible())
        {
            if ((*i)->VOnEvent(evt))
            {
                return true;
            }
        }
    }

    switch (evt.type)
    {
        case SDL_KEYDOWN:
        {
            if (evt.key.repeat == 0)
            {
                return m_pKeyboardHandler->VOnKeyDown(evt.key.keysym.sym);
            }
        }
        case SDL_KEYUP:
        {
            if (evt.key.repeat == 0)
            {
                return m_pKeyboardHandler->VOnKeyUp(evt.key.keysym.sym);
            }
        }

        case SDL_MOUSEMOTION:
        {
            return m_pPointerHandler->VOnPointerMove(evt.motion);
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            return m_pPointerHandler->VOnPointerButtonDown(evt.button);
        }
        case SDL_MOUSEBUTTONUP:
        {
            return m_pPointerHandler->VOnPointerButtonUp(evt.button);
        }
        default:
            return false;
    }

    return false;
}

void HumanView::VOnLostDevice()
{

}

bool HumanView::LoadGame(TiXmlElement* pLevelData)
{
    m_pScene->SortSceneNodesByZCoord();

    return VLoadGameDelegate(pLevelData);
}

void HumanView::VPushElement(shared_ptr<IScreenElement> element)
{
    m_ScreenElements.push_front(element);
}

void HumanView::VRemoveElement(shared_ptr<IScreenElement> element)
{
    m_ScreenElements.remove(element);
}

void HumanView::VSetCameraOffset(int32 offsetX, int32 offsetY)
{
    if (m_pCamera)
    {
        m_pCamera->SetCameraOffset(offsetX, offsetY);
    }
    else
    {
        LOG_WARNING("Trying to set offset to nonexistant camera");
    }
}

void HumanView::RegisterAllDelegates()
{
    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::NewHUDElementDelegate), EventData_New_HUD_Element::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::ScoreUpdatedDelegate), EventData_Updated_Score::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::LivesUpdatedDelegate), EventData_Updated_Lives::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::HealthUpdatedDelegate), EventData_Updated_Health::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::AmmoUpdatedDelegate), EventData_Updated_Ammo::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::AmmoTypeUpdatedDelegate), EventData_Updated_Ammo_Type::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::PowerupUpdatedTimeDelegate), EventData_Updated_Powerup_Time::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::PowerupUpdatedStatusDelegate), EventData_Updated_Powerup_Status::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::RequestPlaySoundDelegate), EventData_Request_Play_Sound::sk_EventType);
    pEventMgr->VAddListener(MakeDelegate(this, &HumanView::RequestResetLevelDelegate), EventData_Request_Reset_Level::sk_EventType);
}

void HumanView::RemoveAllDelegates()
{
    IEventMgr* pEventMgr = IEventMgr::Get();
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::NewHUDElementDelegate), EventData_New_HUD_Element::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::ScoreUpdatedDelegate), EventData_Updated_Score::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::LivesUpdatedDelegate), EventData_Updated_Lives::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::HealthUpdatedDelegate), EventData_Updated_Health::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::AmmoUpdatedDelegate), EventData_Updated_Ammo::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::AmmoTypeUpdatedDelegate), EventData_Updated_Ammo_Type::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::PowerupUpdatedTimeDelegate), EventData_Updated_Powerup_Time::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::PowerupUpdatedStatusDelegate), EventData_Updated_Powerup_Status::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::RequestPlaySoundDelegate), EventData_Request_Play_Sound::sk_EventType);
    pEventMgr->VRemoveListener(MakeDelegate(this, &HumanView::RequestResetLevelDelegate), EventData_Request_Reset_Level::sk_EventType);
}

//=====================================================================================================================
// Delegates
//=====================================================================================================================

void HumanView::NewHUDElementDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_New_HUD_Element> pCastEventData = static_pointer_cast<EventData_New_HUD_Element>(pEventData);
    if (m_pHUD)
    {
        m_pHUD->AddHUDElement(pCastEventData->GetHUDKey(), pCastEventData->GetHUDElement());
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::ScoreUpdatedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Score> pCastEventData = static_pointer_cast<EventData_Updated_Score>(pEventData);
    if (m_pHUD)
    {
        m_pHUD->UpdateScore(pCastEventData->GetNewScore());
        if (!pCastEventData->IsInitialScore())
        {
            if ((pCastEventData->GetOldScore() / 1000000) != (pCastEventData->GetNewScore() / 1000000))
            {
                shared_ptr<EventData_New_Life> pEvent(new EventData_New_Life(pCastEventData->GetActorId(), 1));
                IEventMgr::Get()->VQueueEvent(pEvent);
            }
        }
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::LivesUpdatedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Lives> pCastEventData = static_pointer_cast<EventData_Updated_Lives>(pEventData);
    if (m_pHUD)
    {
        m_pHUD->UpdateLives(pCastEventData->GetNewLivesCount());
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::HealthUpdatedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Health> pCastEventData = static_pointer_cast<EventData_Updated_Health>(pEventData);
    if (m_pHUD)
    {
        m_pHUD->UpdateHealth(max(0, pCastEventData->GetNewHealth()));
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::AmmoUpdatedDelegate(IEventDataPtr pEventData)
{
    if (m_pHUD)
    {
        shared_ptr<EventData_Updated_Ammo> pCastEventData = static_pointer_cast<EventData_Updated_Ammo>(pEventData);

        // TODO: This should be more generic
        if ((pCastEventData->GetAmmoType() == AmmoType_Pistol && m_pHUD->IsElementVisible("pistol")) ||
            (pCastEventData->GetAmmoType() == AmmoType_Magic && m_pHUD->IsElementVisible("magic")) ||
            (pCastEventData->GetAmmoType() == AmmoType_Dynamite && m_pHUD->IsElementVisible("dynamite")))
        { 
            m_pHUD->UpdateAmmo(pCastEventData->GetAmmoCount());
        }
        else
        {
            LOG_ERROR("Unknown ammo type: " + ToStr(pCastEventData->GetAmmoType()));
        }
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::AmmoTypeUpdatedDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Ammo_Type> pCastEventData = static_pointer_cast<EventData_Updated_Ammo_Type>(pEventData);
    if (pCastEventData->GetAmmoType() >= AmmoType_Max)
    {
        return;
    }

    if (m_pHUD)
    {
        // TODO: Redo this to be more general
        m_pHUD->SetElementVisible("pistol", false);
        m_pHUD->SetElementVisible("magic", false);
        m_pHUD->SetElementVisible("dynamite", false);

        if (pCastEventData->GetAmmoType() == AmmoType_Pistol) { m_pHUD->SetElementVisible("pistol", true); }
        else if (pCastEventData->GetAmmoType() == AmmoType_Magic) { m_pHUD->SetElementVisible("magic", true); }
        else if (pCastEventData->GetAmmoType() == AmmoType_Dynamite) { m_pHUD->SetElementVisible("dynamite", true); }
        else
        {
            LOG_ERROR("Unknown ammo type: " + ToStr(pCastEventData->GetAmmoType()));
        }
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

void HumanView::PowerupUpdatedTimeDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Powerup_Time> pCastEventData = static_pointer_cast<EventData_Updated_Powerup_Time>(pEventData);
    if (m_pHUD)
    {
        m_pHUD->UpdateStopwatchTime(pCastEventData->GetSecondsRemaining());
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}


void HumanView::PowerupUpdatedStatusDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Updated_Powerup_Status> pCastEventData = static_pointer_cast<EventData_Updated_Powerup_Status>(pEventData);
    if (m_pHUD)
    {
        if (!m_pHUD->SetElementVisible("stopwatch", !pCastEventData->IsPowerupFinished()))
        {
            LOG_ERROR("Could not set visibility to HUD element \"stopwatch\"");
        }
    }
    else
    {
        LOG_ERROR("HUD is unitialized");
    }
}

// TODO: Handle somehow volume of specific track
// Mix_VolumeChunk for sound
// Music has only 1 channel as far as I know so setting volume for music globally should be fine
void HumanView::RequestPlaySoundDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EventData_Request_Play_Sound> pCastEventData = static_pointer_cast<EventData_Request_Play_Sound>(pEventData);
    if (pCastEventData)
    {
        if (pCastEventData->IsMusic()) // Background music - instrumental
        {
            shared_ptr<MidiFile> pMidiFile = MidiResourceLoader::LoadAndReturnMidiFile(pCastEventData->GetSoundPath().c_str());
            assert(pMidiFile != nullptr);

            g_pApp->GetAudio()->PlayMusic(pMidiFile->data, pMidiFile->size, true);
            g_pApp->GetAudio()->SetMusicVolume(pCastEventData->GetVolume());
        }
        else // Effect / Speech etc. - WAV
        {
            shared_ptr<Mix_Chunk> pSound = WavResourceLoader::LoadAndReturnSound(pCastEventData->GetSoundPath().c_str());
            assert(pSound != nullptr);

            Mix_VolumeChunk(pSound.get(), pCastEventData->GetVolume());
            g_pApp->GetAudio()->PlaySound(pSound.get(), false);
        }
    }
}

void HumanView::RequestResetLevelDelegate(IEventDataPtr pEventData)
{
    // This event actually has nothing but the information that someone requested level reset
    shared_ptr<EventData_Request_Reset_Level> pCastEventData = 
        static_pointer_cast<EventData_Request_Reset_Level>(pEventData);

    LOG_ERROR("");
}
