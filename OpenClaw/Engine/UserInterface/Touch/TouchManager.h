#ifndef __TOUCHMANAGER_H__
#define __TOUCHMANAGER_H__

#include <map>
#include <vector>
#include <list>
#include <memory>

#include "TouchEvents.h"
#include "TouchRecognizers/AbstractRecognizer.h"

class TouchManager {
public:

    void OnFingerDown(const SDL_TouchFingerEvent &evt);
    void OnFingerUp(const SDL_TouchFingerEvent &evt);
    void OnFingerMotion(const SDL_TouchFingerEvent &evt);

    void Update();

    bool PollEvent(Touch_Event *evt);

    void AddRecognizers(const std::vector<std::shared_ptr<AbstractRecognizer>> &recognizer);
    void RemoveRecognizer(int id);
    void RemoveAllRecognizers();

private:
    void OnFingerUpOrMotion(const SDL_TouchFingerEvent &evt, bool isUp);
    void QueueEvent(const Touch_Event &evt);
    void DetachAllExcept(SDL_FingerID fingerId, AbstractRecognizer *except);
    void ProcessAttachedRecognizers(SDL_FingerID fingerId, std::vector<AbstractRecognizer*> &attachedRecognizers);

    std::list<Touch_Event> m_EventQueue;

    std::vector<std::shared_ptr<AbstractRecognizer>> m_Recognizers;
    std::map<SDL_FingerID, std::vector<AbstractRecognizer*>> m_AttachedRecognizers;
};

#endif