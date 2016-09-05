//#define SDL_MAIN_HANDLED
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <libwap.h>
#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_ttf.h>

#include <Windows.h>

#include "Console.h"
#include "Audio\AudioMgr.h"

#include "Game.h"

void CommandHandler(const char* command, void* userdata)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received command: %s", command);
    Console* console = static_cast<Console*>(userdata);
    console->AddLine("YOLO LINE", { 255, 0, 0, 255 });
}

int main(int argc, char* argv[])
{
    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to set high priority class to this process");
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Runtime Error",
            "Failed to initialize SDL2 library.",
            NULL);

        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Captain Claw", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1024, 768, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL2 Renderer");
        exit(1);
    }

    if (TTF_Init() < 0) exit(1);

    TTF_Font* font = TTF_OpenFont("clacon.ttf", 20);
    if (font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load TTF font");
        exit(1);
    } 

    SDL_Texture* backgroundTexture = NULL;
    backgroundTexture = IMG_LoadTexture(renderer, "console02.tga");

    Console* console = new Console(1024, 768 / 2, font, backgroundTexture);
    console->SetCommandHandler(CommandHandler, console);

    if (!sAudioMgr->Initialize())
    {
        return -1;
    }

    RezArchive* clawRezArchive = WAP_LoadRezArchive("CLAW.REZ");
    if (clawRezArchive == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load rez archive CLAW.REZ");
        return -1;
    }

    RezFile* midiRezFile = WAP_GetRezFileFromRezArchive(clawRezArchive, "LEVEL7/MUSIC/PLAY.XMI");
    MidiFile* midiFile = WAP_XmiToMidiFromRezFile(midiRezFile);

    sAudioMgr->PlayMusic(midiFile->data, midiFile->size, true);
    sAudioMgr->SetMusicVolume(20);

    Mix_Chunk* chunk = Mix_LoadWAV("GUNSHOT.WAV");
    sAudioMgr->SetSoundVolume(40);
    sAudioMgr->PlaySound(chunk, true);

    sAudioMgr->PauseAllSounds();

    SDL_StartTextInput();

    SDL_Event event;
    const uint8_t* key_state;
    int count;
    uint32_t ticks;
    uint32_t timeSinceLastFrame = 0;
    bool quit = false;
    while (!quit)
    {
        ticks = SDL_GetTicks();
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
            {
                switch (SDL_GetScancodeFromKey(event.key.keysym.sym))
                {
                case SDL_SCANCODE_KP_0: sAudioMgr->SetMusicVolume(0); break;
                case SDL_SCANCODE_KP_1: sAudioMgr->SetMusicVolume(20); break;
                case SDL_SCANCODE_KP_2: sAudioMgr->SetMusicVolume(40); break;
                case SDL_SCANCODE_KP_3: sAudioMgr->SetMusicVolume(60); break;
                case SDL_SCANCODE_KP_4: sAudioMgr->SetSoundVolume(40); break;
                case SDL_SCANCODE_KP_5: sAudioMgr->SetSoundVolume(80); break;
                case SDL_SCANCODE_KP_6: sAudioMgr->SetSoundVolume(120); break;
                case SDL_SCANCODE_KP_7: sAudioMgr->PauseAllSounds(); break;
                case SDL_SCANCODE_KP_8: sAudioMgr->ResumeAllSounds(); break;
                case SDL_SCANCODE_GRAVE:
                    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Grave");
                    console->Toggle();
                    event.text.text[0] = '\0';
                    break;
                default: break;
                }
            }

            console->ProcessEvents(event);
        }
        key_state = SDL_GetKeyboardState(&count);
        if (key_state[SDL_SCANCODE_ESCAPE])
            quit = true;

        SDL_Delay(2);

        console->Update(timeSinceLastFrame, renderer);

        SDL_RenderPresent(renderer);

        timeSinceLastFrame = SDL_GetTicks() - ticks;
    }

    SDL_StopTextInput();

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "-------- Terminating managers --------");
    // Cleanup
    sAudioMgr->Terminate();

    return 0;
}