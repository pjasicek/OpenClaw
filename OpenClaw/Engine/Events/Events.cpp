#include "Events.h"

// To define a new event - you need a 32-bit GUID. 
// In Visual Studio, go to Tools->Create GUID and grab the first bit.

const EventType EventData_Environment_Loaded::sk_EventType(0xa3814acd);
const EventType EventData_Remote_Environment_Loaded::sk_EventType(0x8E2AD6E6);
const EventType EventData_New_Actor::sk_EventType(0xe86c7c31);
const EventType EventData_Move_Actor::sk_EventType(0xeeaa0a40);
const EventType EventData_Destroy_Actor::sk_EventType(0x77dd2b3a);
const EventType EventData_New_Render_Component::sk_EventType(0xaf4aff75);
const EventType EventData_Modified_Render_Component::sk_EventType(0x80fe9766);
const EventType EventData_Request_Start_Game::sk_EventType(0x11f2b19d);
const EventType EventData_Remote_Client::sk_EventType(0x301693d5);
const EventType EventData_Network_Player_Actor_Assignment::sk_EventType(0xa7c92f11);
const EventType EventData_Update_Tick::sk_EventType(0xf0f5d183);
const EventType EventData_Decompress_Request::sk_EventType(0xc128a129);
const EventType EventData_Decompression_Progress::sk_EventType(0x68de1f28);
const EventType EventData_Request_New_Actor::sk_EventType(0x40378c64);
//const EventType EventData_Request_Destroy_Actor::sk_EventType(0xf5395770);
const EventType EventData_PlaySound::sk_EventType(0x3d8118ee);
const EventType EventData_Attach_Actor::sk_EventType(0x3dac18ee);
const EventType EventData_Collideable_Tile_Created::sk_EventType(0x8cac18ee);
const EventType EventData_Start_Climb::sk_EventType(0x8bab18ee);
const EventType EventData_Actor_Fire::sk_EventType(0x123418ee);
const EventType EventData_Actor_Attack::sk_EventType(0x567818ee);
const EventType EventData_New_HUD_Element::sk_EventType(0x5678abce);
const EventType EventData_New_Life::sk_EventType(0x4b83b7fb);
const EventType EventData_Updated_Score::sk_EventType(0x5a1f78ce);
const EventType EventData_Updated_Lives::sk_EventType(0xf671afe8);
const EventType EventData_Updated_Health::sk_EventType(0xe7eb8dba);
const EventType EventData_Updated_Ammo::sk_EventType(0xa2ba516);
const EventType EventData_Updated_Ammo_Type::sk_EventType(0x60169d12);
const EventType EventData_Request_Change_Ammo_Type::sk_EventType(0xe4c6edd7);
const EventType EventData_Teleport_Actor::sk_EventType(0x1f3bc45);
const EventType EventData_Updated_Powerup_Time::sk_EventType(0xadb6bd74);
const EventType EventData_Updated_Powerup_Status::sk_EventType(0xe0b1d14d);
const EventType EventData_Modify_Player_Stat::sk_EventType(0xbcf3d9f5);
const EventType EventData_Checkpoint_Reached::sk_EventType(0xcf204982);
const EventType EventData_Claw_Died::sk_EventType(0x6b32b1af);
const EventType EventData_Request_Play_Sound::sk_EventType(0x6abcb1af);
const EventType EventData_Request_Reset_Level::sk_EventType(0x6a5168af);
const EventType EventData_Menu_SwitchPage::sk_EventType(0x6a5842af);
const EventType EventData_Menu_LoadGame::sk_EventType(0x12abcdaf);
const EventType EventData_Quit_Game::sk_EventType(0xbf29c1f5);
const EventType EventData_Menu_Modifiy_Item_Visibility::sk_EventType(0x955a579c);
const EventType EventData_Set_Volume::sk_EventType(0xc486b92);
const EventType EventData_Sound_Enabled_Changed::sk_EventType(0x1236b92);
const EventType EventData_Menu_Modify_Item_State::sk_EventType(0x4566b92);
const EventType EventData_Add_Static_Geometry::sk_EventType(0x456abc2);
const EventType EventData_Enter_Menu::sk_EventType(0xdc4606a5);
const EventType EventData_Item_Picked_Up::sk_EventType(0xdc5706a5);
const EventType EventData_Finished_Level::sk_EventType(0xdc5706f5);
const EventType EventData_Entered_Boss_Area::sk_EventType(0x6a51ffff);
const EventType EventData_Boss_Fight_Started::sk_EventType(0x62ccc8af);
const EventType EventData_Boss_Fight_Ended::sk_EventType(0x62cdd8af);
const EventType EventData_Boss_Health_Changed::sk_EventType(0x2222d8af);
const EventType EventData_Claw_Health_Below_Zero::sk_EventType(0xfffff8af);
const EventType EventData_Claw_Respawned::sk_EventType(0xffabc8af);
const EventType EventData_IngameMenu_Resume_Game::sk_EventType(0x44abc8af);
const EventType EventData_IngameMenu_End_Life::sk_EventType(0x455bc8af);
const EventType EventData_IngameMenu_End_Game::sk_EventType(0x455bcfff);
const EventType EventData_Actor_Fire_Ended::sk_EventType(0x666bcfff);
const EventType EventData_World_Finished_Loading::sk_EventType(0x776bcfff);

/*bool EventData_PlaySound::VBuildEventFromScript(void)
{
    if (m_EventData.IsString())
    {
        m_SoundResource = m_EventData.GetString();
        return true;
    }

    return false;
}

void RegisterEngineScriptEvents(void)
{
    REGISTER_SCRIPT_EVENT(EventData_Request_Destroy_Actor, EventData_Request_Destroy_Actor::sk_EventType);
    REGISTER_SCRIPT_EVENT(EventData_PhysCollision, EventData_PhysCollision::sk_EventType);
    REGISTER_SCRIPT_EVENT(EventData_PlaySound, EventData_PlaySound::sk_EventType);
}

*/