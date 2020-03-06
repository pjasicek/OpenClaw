#include "Console.h"
#include <algorithm>
#include <assert.h>

#include <iostream>
#include <sstream>

//#####################################################################
//################### GLOBAL CONSTANTS ################################
//#####################################################################

//#####################################################################
//################### HELPER FUNCTIONS ################################
//#####################################################################

static SDL_Texture* GetTextureFromTtfText(TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer, std::string text)
{
    //cout << "GetTextureFromTtfText" << endl;
    SDL_Surface* surfaceText = TTF_RenderText_Blended(font, text.c_str(), textColor);
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);
    SDL_FreeSurface(surfaceText);

    return textureText;
}

SDL_Rect GetRenderRectFromTexture(SDL_Texture* texture, int16_t x, int16_t y)
{
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    return{ x, y, w, h };
}

void RenderText(SDL_Renderer* renderer, std::string text, TTF_Font* font, SDL_Color color, int16_t x, int16_t y)
{
    SDL_Texture* texture = GetTextureFromTtfText(font, color, renderer, text);
    SDL_Rect rect = GetRenderRectFromTexture(texture, x, y);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

void RenderRectangle(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color)
{
    // Save defaults
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // Restore defaults
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void SplitStringIntoVector(std::string str, std::vector<std::string>& vec)
{
    std::stringstream ss(str);
    ss >> std::noskipws;
    std::string field;
    char ws_delim;
    while (1) 
    {
        if (ss >> field)
        {
            vec.push_back(field);
        }
        else if (ss.eof())
        {
            break;
        }
        else
        {
            vec.push_back(std::string());
        }
        ss.clear();
        ss >> ws_delim;
    }

    if (vec.empty())
    {
        vec.push_back(str);
    }

    if (str.back() == ' ')
    {
        vec.push_back(std::string(""));
    }
}

//#####################################################################
//################## IMPLEMENTATION - ConsoleText #####################
//#####################################################################

class ConsoleText
{
public:
    ConsoleText(TTF_Font* font, std::string text, SDL_Color color, int16_t x, int16_t y);
    ~ConsoleText();

    std::string GetText() { return _text; }
    SDL_Color GetColor() { return _color; }

    void Render(SDL_Renderer* renderer, int16_t startX, int16_t startY);

private:
    void Init(SDL_Renderer* renderer);

    TTF_Font* _font;
    std::string _text;
    SDL_Color _color;
    SDL_Texture* _texture;
    SDL_Rect _renderRect;

    int16_t _x;
    int16_t _y;
};

ConsoleText::ConsoleText(TTF_Font* font, std::string text, SDL_Color color, int16_t x, int16_t y)
{
    assert(font != NULL);

    _text = text;
    _color = color;
    _font = font;
    _texture = NULL;
    _x = x;
    _y = y;
}

ConsoleText::~ConsoleText()
{
    if (_texture != NULL)
    {
        SDL_DestroyTexture(_texture);
        _texture = NULL;
    }
}

void ConsoleText::Init(SDL_Renderer* renderer)
{
    _texture = GetTextureFromTtfText(_font, _color, renderer, _text.c_str());
    _renderRect = GetRenderRectFromTexture(_texture, _x, _y);
}

void ConsoleText::Render(SDL_Renderer* renderer, int16_t startX, int16_t startY)
{
    if (_texture == NULL)
    {
        Init(renderer);
    }

    //cout << "ConsoleText::Render" << endl;

    SDL_Rect screenRect = { _renderRect.x - startX, _renderRect.y - startY, _renderRect.w, _renderRect.h };
    //PrintRect(screenRect, "ScreenRect");
    SDL_RenderCopy(renderer, _texture, NULL, &screenRect);
}

//#####################################################################
//################## IMPLEMENTATION - ConsoleLine #####################
//#####################################################################

class ConsoleLine
{
public:

    ConsoleLine(TTF_Font* font, uint16_t lineNumber, int16_t leftOffset);
    ~ConsoleLine();

    std::string GetLineText();
    uint16_t GetLinePixelWidth();
    uint16_t GetLineNumber() { return _lineNumber; }
    SDL_Rect& GetRenderRect() { return _renderRect; }
    void AddText(std::string text, SDL_Color textColor);
    void Render(SDL_Renderer* renderer, uint16_t startX, uint16_t startY);
    void Commit();

private:
    std::vector<ConsoleText> _texts;
    int16_t _leftOffset;
    uint16_t _lineNumber;
    SDL_Rect _renderRect;
    TTF_Font* _font;
    bool _committed;
};


ConsoleLine::ConsoleLine(TTF_Font* font, uint16_t lineNumber, int16_t leftOffset)
{
    assert(font != NULL);

    _font = font;
    _lineNumber = lineNumber;

    _leftOffset = leftOffset;

    int lineHeight = TTF_FontHeight(_font);
    int totalWidth = 0;

    _renderRect = { 0, _lineNumber * lineHeight, totalWidth, lineHeight };

    _committed = false;
}

ConsoleLine::~ConsoleLine()
{
    _texts.clear();
}

uint16_t ConsoleLine::GetLinePixelWidth()
{
    std::string lineText = GetLineText();

    int w, h;
    TTF_SizeText(_font, lineText.c_str(), &w, &h);

    return w;
}

void ConsoleLine::AddText(std::string text, SDL_Color color)
{
    if (_committed)
    {
        return;
    }

    int16_t y = _renderRect.y;
    int16_t x = GetLinePixelWidth() + _leftOffset;

    //cout << "AddText: x = " << x << ", y = " << y << endl;

    _texts.push_back(ConsoleText(_font, text, color, x, y));
}

std::string ConsoleLine::GetLineText()
{
    std::string lineText;
    for (auto linePart : _texts)
    {
        lineText += linePart.GetText();
    }

    return lineText;
}

void ConsoleLine::Render(SDL_Renderer* renderer, uint16_t startX, uint16_t startY)
{
    //cout << "ConsoleLine::Render" << endl;
    if (!_committed)
    {
        return;
    }

    for (ConsoleText text : _texts)
    {
        //cout << "for (ConsoleText text : _texts)" << endl;
        text.Render(renderer, startX, startY);
    }
}

void ConsoleLine::Commit()
{
    _renderRect.w = GetLinePixelWidth();
    //cout << "Pixel width = " << GetLinePixelWidth();

    _committed = true;
}

//#####################################################################
//#################### IMPLEMENTATION - Console #######################
//#####################################################################

Console::Console(uint16_t width, uint16_t height, TTF_Font* font, SDL_Renderer* renderer, const char* backgroundResource)
{
    assert(font != NULL);

    _width = width;
    _height = height;
    _font = font;
    _isActive = false;

    m_LineSeparatorHeight = 3;
    m_CommandPromptOffsetY = 10;
    m_ConsosleToggleSpeed = 0.65;

    _x = 0;
    _y = 0;
    _totalHeight = _height + m_LineSeparatorHeight + m_CommandPromptOffsetY;
    _animationOffsetY = _totalHeight;

    _leftOffset = 5;
    _commandPrompt = "> ";

    int w, h;
    TTF_SizeText(font, _commandPrompt.c_str(), &w, &h);
    _commandLeftOffset = w + _leftOffset;
    _lineHeight = h;

    _backgroundTexture = IMG_LoadTexture(renderer, backgroundResource);

    _handler = NULL;
    _handlerUserData = NULL;

    _autocompleted = false;

    AddLine("This is a cat:", COLOR_WHITE);
    AddLine("   ____", COLOR_WHITE);
    AddLine("  (.   \\", COLOR_WHITE);
    AddLine("    \\  |   ", COLOR_WHITE);
    AddLine("     \\ |___(\\--/)", COLOR_WHITE);
    AddLine("   __/    (  . . )", COLOR_WHITE);
    AddLine("  \"'._.    '-.O.'", COLOR_WHITE);
    AddLine("       '-.  \\ \"|\\", COLOR_WHITE);
    AddLine("          '.,,/'.,,", COLOR_WHITE);
}

Console::Console(const ConsoleConfig* const pConsoleConfig, SDL_Renderer* pRenderer, SDL_Window* pWindow)
{
    _x = 0;
    _y = 0;
    _isActive = false;
    _autocompleted = false;
    _handler = NULL;
    _handlerUserData = NULL;

    m_pRenderer = pRenderer;
    m_pWindow = pWindow;

    assert(m_pRenderer && m_pWindow);

    int windowWidth, windowHeight;
    float scaleX, scaleY;
    SDL_GetWindowSize(m_pWindow, &windowWidth, &windowHeight);
    SDL_RenderGetScale(pRenderer, &scaleX, &scaleY);

    windowWidth = (int)(windowWidth / scaleX);
    windowHeight = (int)(windowHeight / scaleY);

    _width = pConsoleConfig->widthRatio * windowWidth;
    _height = pConsoleConfig->heightRatio * windowHeight;
    _leftOffset = pConsoleConfig->leftOffset;
    _commandPrompt = pConsoleConfig->commandPrompt + " ";
    m_LineSeparatorHeight = pConsoleConfig->lineSeparatorHeight;
    m_CommandPromptOffsetY = pConsoleConfig->commandPromptOffsetY;
    m_ConsosleToggleSpeed = pConsoleConfig->consoleAnimationSpeed;

    _font = TTF_OpenFont(pConsoleConfig->fontPath.c_str(), pConsoleConfig->fontHeight);
    assert(_font != NULL);

    _backgroundTexture = IMG_LoadTexture(pRenderer, pConsoleConfig->backgroundImagePath.c_str());

    _totalHeight = _height + m_LineSeparatorHeight + m_CommandPromptOffsetY;
    _animationOffsetY = _totalHeight;

    int w, h;
    TTF_SizeText(_font, _commandPrompt.c_str(), &w, &h);
    _commandLeftOffset = w + _leftOffset;
    _lineHeight = h;
}

Console::~Console()
{
    _consoleTextLines.clear();
    if (_backgroundTexture != NULL)
    {
        SDL_DestroyTexture(_backgroundTexture);
        _backgroundTexture = NULL;
    }
    if (_font) {
        TTF_CloseFont(_font);
        _font = nullptr;
    }
}

//################# INTERFACE #####################

void Console::OnUpdate(uint32_t msDiff)
{
    if (!_isActive && (_animationOffsetY >= _totalHeight))
    {
        return;
    }

    if (_isActive && (_animationOffsetY > 0))
    {
        _animationOffsetY -= (double)msDiff * m_ConsosleToggleSpeed;
        if (_animationOffsetY < 0)
        {
            _animationOffsetY = 0.0;
        }
    }
    else if (!_isActive && (_animationOffsetY < _totalHeight))
    {
        _animationOffsetY += (double)msDiff * m_ConsosleToggleSpeed;
        if (_animationOffsetY > _totalHeight)
        {
            _animationOffsetY = _totalHeight;
        }
    }
}

void Console::OnRender(SDL_Renderer* renderer)
{
    if (!_isActive && _animationOffsetY >= _totalHeight)
    {
        return;
    }

    RenderBackground(renderer);
    RenderCommandHistory(renderer);
    RenderCurrentCommand(renderer);
}

void Console::AddLine(std::string text, SDL_Color color)
{
    int lineNumber = _consoleTextLines.size();
    ConsoleLine newLine = ConsoleLine(_font, lineNumber, _leftOffset);
    newLine.AddText(text, color);
    newLine.Commit();
    _consoleTextLines.push_back(newLine);
    //cout << "size = " << _consoleTextLines.size() << endl;
}

bool Console::OnEvent(SDL_Event& event)
{
    bool eventEaten = false;

    // If console is inactive then only check tilde for activation
    if (!_isActive)
    {
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            SDL_GetScancodeFromKey(event.key.keysym.sym) == SDL_SCANCODE_GRAVE)
        {
            eventEaten = true;
            _isActive = true;
        }
        return eventEaten;
    }

    eventEaten = true;

    std::string originalCommand = _currentCommandText;
    bool doneAutocomplete = false;

    if (event.type == SDL_KEYDOWN)
    {
        // Backspace - delete 1 character if possible
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && _currentCommandText.length() > 0)
        {
            _currentCommandText = _currentCommandText.substr(0, _currentCommandText.length() - 1);
        }
        // Enter - execute current command and save it to history
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER))
        {
            CommitCurrentCommand();
        }
        // Toggle console
        else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            SDL_GetScancodeFromKey(event.key.keysym.sym) == SDL_SCANCODE_GRAVE)
        {
            _isActive = false;
            return true;
        }
        // UP - move console up
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_UP))
        {
            ScrollUp(_lineHeight);
        }
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_PAGEUP))
        {
            ScrollUp(3 * _lineHeight);
        }
        // DOWN - move console down
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_DOWN))
        {
            ScrollDown(_lineHeight);
        }
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_PAGEDOWN))
        {
            ScrollDown(3 * _lineHeight);
        }
        // TAB - try to autocomplete current command
        else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_TAB))
        {
            AutocompleteCommand();
            doneAutocomplete = true;

            // In case we cycled through all commands, reset history 
            if (_autocompleteCommands.size() == _autcompletedCommandsIdxs.size())
            {
                _autcompletedCommandsIdxs.clear();
            }
        }
    }
    else if (event.type == SDL_TEXTINPUT)
    {
        _currentCommandText += event.text.text;
        _currentCommandText.erase(std::remove(_currentCommandText.begin(), _currentCommandText.end(), ';'), _currentCommandText.end());
    }
    else if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y > 0)
        {
            ScrollUp(_lineHeight);
        }
        else if (event.wheel.y < 0)
        {
            ScrollDown(_lineHeight);
        }
    }

    // If string by keyboard input (delete or textinput and so on) clear autocomplete history
    if (!doneAutocomplete && (originalCommand.compare(_currentCommandText) != 0))
    {
        _autcompletedCommandsIdxs.clear();
    }

    return eventEaten;
}

void Console::Toggle() 
{ 
    _isActive = !_isActive; 
}

bool Console::IsActive()
{ 
    return _isActive; 
}

void Console::SetIsActive(bool val)
{ 
    _isActive = val; 
}

void Console::SetCommandHandler(void(*handler)(const char*, void*), void* userdata)
{ 
    _handler = handler;
    _handlerUserData = userdata;
}

//################# PRIVATE IMPLEMENTATION #####################

void Console::RenderBackground(SDL_Renderer* renderer)
{
    int imgWidth, imgHeight;
    int consoleWidth = _width;
    int consoleHeight = _height + m_CommandPromptOffsetY;

    // No background texture is set - render default as black color
    if (_backgroundTexture == NULL)
    {
        SDL_Rect backgroundRect = { 0, 0, consoleWidth, consoleHeight - (int16_t)_animationOffsetY };
        RenderRectangle(renderer, backgroundRect, COLOR_BLACK_LIGHTER);
    }
    // Render background with given texture
    else
    {
        SDL_QueryTexture(_backgroundTexture, NULL, NULL, &imgWidth, &imgHeight);

        int repeatX = consoleWidth / imgWidth;
        if ((consoleWidth % imgWidth) > 0)
        {
            repeatX++;
        }

        int repeatY = consoleHeight / imgHeight;
        if ((consoleHeight % imgHeight) > 0)
        {
            repeatY++;
        }

        for (int i = 1; i <= repeatX; i++)
        {
            for (int j = 1; j <= repeatY; j++)
            {
                int w = imgWidth;
                int h = imgHeight;
                int coordX = (i - 1) * imgWidth;
                int coordY = (j - 1) * imgHeight;
                // Check if last part on X is beyond width dimensions, if so, clip it
                if ((i * imgWidth) > consoleWidth)
                {
                    w = imgWidth - ((i * imgWidth) - consoleWidth);
                }
                // Same with Y
                if ((j * imgHeight) > consoleHeight)
                {
                    h = imgHeight - ((j * imgHeight) - consoleHeight);
                }

                SDL_Rect srcTextureRect = { 0, 0, w, h };
                SDL_Rect dstRect = { coordX, coordY - (int16_t)_animationOffsetY, w, h };

                SDL_RenderCopy(renderer, _backgroundTexture, &srcTextureRect, &dstRect);
            }
        }
    }

    // Render RED line below console
    SDL_Rect lineRect = { 0, _height + m_CommandPromptOffsetY - (int16_t)_animationOffsetY, _width, m_LineSeparatorHeight };
    RenderRectangle(renderer, lineRect, COLOR_RED);
}

void Console::RenderCommandHistory(SDL_Renderer* renderer)
{
    SDL_Rect consoleRect = GetRenderRect();
    // Render all visible console lines
    for (auto consoleLine : _consoleTextLines)
    {

        SDL_Rect lineRect = consoleLine.GetRenderRect();
        //PrintRect(consoleRect, "ConsoleRect");
        //PrintRect(lineRect, "LineRect");
        if (SDL_HasIntersection(&consoleRect, &lineRect))
        {
            //cout << "Rendering.." << endl;
            consoleLine.Render(renderer, _x, _y +(int16_t)_animationOffsetY);
        }
    }
}

void Console::RenderCurrentCommand(SDL_Renderer* renderer)
{
    int16_t promptStartX = _leftOffset;
    int16_t promptStartY = _height - _lineHeight + 4;

    RenderText(renderer, _commandPrompt, _font, COLOR_WHITE, promptStartX, promptStartY - (int16_t)_animationOffsetY);

    _currentCommandText += '_';
    RenderText(renderer, _currentCommandText, _font, COLOR_WHITE, _commandLeftOffset, promptStartY - (int16_t)_animationOffsetY);
    _currentCommandText = _currentCommandText.substr(0, _currentCommandText.length() - 1);
}

SDL_Rect Console::GetRenderRect()
{
    return{ _x, _y, _width, _height - _lineHeight - m_LineSeparatorHeight };
}

void Console::CommitCurrentCommand()
{
    uint16_t lineNumber = _consoleTextLines.size();
    ConsoleLine newConsoleLine = ConsoleLine(_font, lineNumber, _leftOffset);
    newConsoleLine.AddText(_commandPrompt, COLOR_WHITE);
    newConsoleLine.AddText(_currentCommandText, COLOR_WHITE);

    //cout << "line width = " << newConsoleLine.GetLinePixelWidth() << endl;

    newConsoleLine.Commit();
    _consoleTextLines.push_back(newConsoleLine);

    // Notify command handler if any
    if (_handler)
    {
        _handler(_currentCommandText.c_str(), _handlerUserData);
    }

    _currentCommandText.clear();

    // Move command line if necessary
    if ((_consoleTextLines.size() * _lineHeight) > (uint16_t)(_height - m_CommandPromptOffsetY))
    {
        _y += _lineHeight;
    }

    //_y += _lineHeight;
}

void Console:: ScrollUp(int16_t distanceY)
{
    if ((_consoleTextLines.size() > 0) && (_consoleTextLines[0].GetRenderRect().y < _y))
    {
        if (distanceY > _y)
        {
            _y = 0;
        }
        else
        {
            _y -= distanceY;
        }
    }
    else
    {
        _y = 0;
    }
}

void Console::ScrollDown(int16_t distanceY)
{
    if ((_consoleTextLines.size() > 0) && (_consoleTextLines.back().GetRenderRect().y >= _y))
    {
        int16_t maxScrollDistance = _consoleTextLines.back().GetRenderRect().y - _y;
        if (distanceY > maxScrollDistance)
        {
            _y += maxScrollDistance;
        }
        else
        {
            _y += distanceY;
        }
    }
}

void Console::AddAutocompleteCommand(const char* command)
{
    _autocompleteCommands.push_back(std::string(command));

    // Sort and remove any duplicates
    std::sort(_autocompleteCommands.begin(), _autocompleteCommands.end());
    _autocompleteCommands.erase(std::unique(_autocompleteCommands.begin(), _autocompleteCommands.end()), _autocompleteCommands.end());
}

void Console::InvalidateSameCommands(std::string subCommand, int16_t subCommandPos)
{
    for (auto autocompleteCommand : _autocompleteCommands)
    {
        int pos = std::find(_autocompleteCommands.begin(), _autocompleteCommands.end(), autocompleteCommand) - _autocompleteCommands.begin();

        std::vector <std::string> autocompleteCommandVector;
        SplitStringIntoVector(autocompleteCommand, autocompleteCommandVector);

        if (((int16_t)autocompleteCommandVector.size()) >= (subCommandPos + 1))
        {
            if (autocompleteCommandVector[subCommandPos].compare(subCommand) == 0)
            {
                // Add if not exists in vector already
                if (std::find(_autcompletedCommandsIdxs.begin(), _autcompletedCommandsIdxs.end(), pos) == _autcompletedCommandsIdxs.end())
                {
                    _autcompletedCommandsIdxs.push_back(pos);
                }
            }
        }
    }
}

void Console::AutocompleteCommand()
{
    // Makes no sense to check if there are no entries
    if (_autocompleteCommands.empty())
    {
        return;
    }

    // Split current command into vector of strings separated by space
    std::vector<std::string> currentStringsVector;
    SplitStringIntoVector(_currentCommandText, currentStringsVector);

    // Iterate over all saved autocomplete commands
    for (auto autocompleteCommand : _autocompleteCommands)
    {
        // Check if we already checked tried this command in previous "TAB" click, if we did, skip it
        int pos = std::find(_autocompleteCommands.begin(), _autocompleteCommands.end(), autocompleteCommand) - _autocompleteCommands.begin();

        if (std::find(_autcompletedCommandsIdxs.begin(), _autcompletedCommandsIdxs.end(), pos) != _autcompletedCommandsIdxs.end())
        {
            continue;
        }

        _autcompletedCommandsIdxs.push_back(pos);

        // Split current autocomplete command into vector of strings separated by sapce
        std::vector <std::string> autocompleteCommandVector;
        SplitStringIntoVector(autocompleteCommand, autocompleteCommandVector);

        // If current command has already more strings, then no match is possible, skip
        if (currentStringsVector.size() > autocompleteCommandVector.size())
        {
            continue;
        }

        // Given "n = number of words in current command" compare strings with autocomplete command n - 1 times -
        // - meaning check that all words up until last one are same
        int16_t n = currentStringsVector.size() - 1;
        // If we only have single string in current command, we go check only "last" string
        bool foundMatch = true;
        for (uint16_t i = 0; i < n; i++)
        {
            if (currentStringsVector[i].compare(autocompleteCommandVector[i]) != 0)
            {
                foundMatch = false;
                break;
            }
        }

        size_t replaceLength = std::max(autocompleteCommandVector[n].length(), currentStringsVector[n].length());
        currentStringsVector[n].clear();

        InvalidateSameCommands(autocompleteCommandVector[n], n);
        

        // If yes, check if last current command word is substring of last autocomplete word
        if (foundMatch)
        {
            // If it is, autocomplete command and save index of given autocompleted command
            if (autocompleteCommandVector[n].find(currentStringsVector[n]) == 0 ||
                currentStringsVector[n].empty()) //<<<< !!! problem
            {
                size_t lastWordIdx;
                if (std::count(_currentCommandText.begin(), _currentCommandText.end(), ' ') == 0)
                {
                    lastWordIdx = 0;
                }
                else
                {
                    lastWordIdx = _currentCommandText.find_last_of(' ') + 1;

                }
                _currentCommandText.replace(lastWordIdx, replaceLength, autocompleteCommandVector[n]);
                break;
            }
        }
    }    
}