#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>
#include <vector>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

const SDL_Color COLOR_RED = { 255, 0, 0, 255 };
const SDL_Color COLOR_GREEN = { 0, 255, 0, 255 };
const SDL_Color COLOR_BLUE = { 0, 0, 255, 255 };
const SDL_Color COLOR_BLACK = { 0, 0, 0, 255 };
const SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };
const SDL_Color COLOR_BLACK_LIGHTER = { 30, 30, 30, 255 };

struct ConsoleConfig
{
    ConsoleConfig()
    {
        backgroundImagePath = "";
        stretchBackgroundImage = true;
        widthRatio = 1.0;
        heightRatio = 0.5;
        lineSeparatorHeight = 3;
        commandPromptOffsetY = 10;
        consoleAnimationSpeed = 0.65;
        fontColor = COLOR_WHITE;
        fontPath = "";
        fontHeight = 14;
        leftOffset = 5;
        commandPrompt = "> ";
    }

    std::string backgroundImagePath;
    bool stretchBackgroundImage;
    double widthRatio;
    double heightRatio;
    unsigned lineSeparatorHeight;
    unsigned commandPromptOffsetY;
    double consoleAnimationSpeed;
    SDL_Color fontColor;
    std::string fontPath;
    unsigned fontHeight;
    unsigned leftOffset;
    std::string commandPrompt;
};

class ConsoleLine;

class Console
{
public:
    Console(uint16_t width, uint16_t height, TTF_Font* font, SDL_Renderer* renderer, const char* backgroundResource = NULL);
    Console(const ConsoleConfig* const pConsoleConfig, SDL_Renderer* pRenderer, SDL_Window* pWindow);
    ~Console();

    void OnUpdate(uint32_t msDiff);
    void OnRender(SDL_Renderer* renderer);
    bool OnEvent(SDL_Event& event);

    void AddLine(std::string text, SDL_Color color);

    void Toggle();

    bool IsActive();

    void Resize(uint16_t width, uint16_t height) { _width = width; _height = height; }

    void SetIsActive(bool val);

    //* Registers callback for registering commited commands
    //* Usage:
    //  void CommandHandler(const char* command);
    //  ...
    //  console->SetCommandHandler(CommandHandler);
    void SetCommandHandler(void(*handler)(const char*, void*), void* userdata);

    //* Higly experimental, use at your own risk
    //* Not working as bash autocomplete, this just loops through available
    // commands endlessly
    //* To register command use it like this:
    //* console->AddAutocompleteCommand("list");
    //* console->AddAutocompleteCommand("list all");
    //* console->AddAutocompleteCommand("list all objects");
    //* Duplicates are automatically removed
    void AddAutocompleteCommand(const char* command);

private:
    void RenderBackground(SDL_Renderer* renderer);
    void RenderCommandHistory(SDL_Renderer* renderer);
    void RenderCurrentCommand(SDL_Renderer* renderer);

    void CommitCurrentCommand();
    void ScrollUp(int16_t distanceY);
    void ScrollDown(int16_t distanceY);
    void AutocompleteCommand();

    void InvalidateSameCommands(std::string subCommand, int16_t subCommandPos);

    SDL_Rect GetRenderRect();

    int _width;
    int _height;
    int _totalHeight;

    int _x;
    int _y;

    double _animationOffsetY;

    bool _isActive;
    uint16_t _commandLeftOffset;
    int16_t _leftOffset;
    uint16_t _lineHeight;
    TTF_Font* _font;

    uint16_t m_LineSeparatorHeight;
    uint16_t m_CommandPromptOffsetY;
    double m_ConsosleToggleSpeed;

    SDL_Renderer* m_pRenderer;
    SDL_Window* m_pWindow;

    std::string _commandPrompt;
    std::string _currentCommandText;

    std::vector<ConsoleLine> _consoleTextLines;
    std::vector<std::string> _autocompleteCommands;
    std::vector<uint16_t> _autcompletedCommandsIdxs;
    bool _autocompleted;

    SDL_Texture* _backgroundTexture;

    void(*_handler)(const char*, void*);
    void* _handlerUserData;
};

#endif
