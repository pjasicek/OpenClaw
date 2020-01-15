#include <algorithm>
#include <cassert>
#include "TouchManager.h"

void TouchManager::Update() {
    for (auto &recognizer : m_Recognizers) {
        recognizer->VOnUpdate();
    }

    for (auto attach = m_AttachedRecognizers.begin(); attach != m_AttachedRecognizers.end();) {
        const SDL_FingerID &fingerId = attach->first;
        std::vector<AbstractRecognizer*> &recognizers = attach->second;

        if (recognizers.empty()) {
            attach = m_AttachedRecognizers.erase(attach);
            continue;
        }

        ProcessAttachedRecognizers(fingerId, recognizers);
        ++attach;
    }
}

bool TouchManager::PollEvent(Touch_Event *evt) {
    if (m_EventQueue.empty() || !evt) {
        return false;
    }
    *evt = m_EventQueue.front();
    m_EventQueue.pop_front();
    return true;
}

void TouchManager::DetachAllExcept(SDL_FingerID fingerId, AbstractRecognizer *except) {
    auto it = m_AttachedRecognizers.find(fingerId);
    if (it != m_AttachedRecognizers.end()) {
        std::vector<AbstractRecognizer*> &recognizers = it->second;

        for (AbstractRecognizer *recognizer : recognizers) {
            if (recognizer && recognizer != except) {
                recognizer->VFingerDetached(fingerId);
            }
        }

        recognizers.clear();
        if (except) {
            recognizers.push_back(except);
        }
    }
}

void TouchManager::QueueEvent(const Touch_Event &evt) {
    m_EventQueue.push_back(evt);
}

void TouchManager::OnFingerDown(const SDL_TouchFingerEvent &evt) {
    std::vector<AbstractRecognizer*> attached;
    bool stop = false;
    for (auto &recognizer : m_Recognizers) {
        auto state = recognizer->OnFingerDown(evt);
        switch (state) {
            case RecognizerState::DoNothing:
            case RecognizerState::Failed:
                break;
            case RecognizerState::Recognizing:
                attached.push_back(recognizer.get());
                break;
            case RecognizerState::EventReady:
            case RecognizerState::Hold:
            case RecognizerState::Done:
                attached.push_back(recognizer.get());
                stop = true;
                break;
            default:
                assert(false && "Unknown state !");
                break;
        }
        if (stop)
            break;
    }
    if (!attached.empty()) {
        m_AttachedRecognizers.insert(std::make_pair(evt.fingerId, std::move(attached)));
    }
}

void TouchManager::OnFingerUp(const SDL_TouchFingerEvent &evt) {
    OnFingerUpOrMotion(evt, true);
}

void TouchManager::OnFingerMotion(const SDL_TouchFingerEvent &evt) {
    OnFingerUpOrMotion(evt, false);
}


void TouchManager::OnFingerUpOrMotion(const SDL_TouchFingerEvent &evt, bool isUp) {
    auto it = m_AttachedRecognizers.find(evt.fingerId);
    if (it != m_AttachedRecognizers.end()) {
        std::vector<AbstractRecognizer*> &attached = it->second;

        bool isFirst = true;
        bool stop = false;
        for (auto recognizerIt = attached.begin(); !stop && recognizerIt != attached.end();) {
            AbstractRecognizer *recognizer = *recognizerIt;
            assert(recognizer);

            auto state = isUp ? recognizer->OnFingerUp(evt) : recognizer->OnFingerMotion(evt);
            switch (state) {
                case RecognizerState::DoNothing:
                case RecognizerState::Failed:
                    recognizer->VFingerDetached(evt.fingerId);
                    recognizerIt = attached.erase(recognizerIt);
                    break;
                case RecognizerState::Recognizing:
                    isFirst = false;
                    ++recognizerIt;
                    break;
                case RecognizerState::EventReady:
                case RecognizerState::Hold:
                case RecognizerState::Done:
                    if (isFirst) {
                        if (attached.size() != 1) {
                            DetachAllExcept(evt.fingerId, recognizer);
                        }
                        stop = true;
                        break;
                    }
                    isFirst = false;
                    ++recognizerIt;
                    break;
                default:
                    assert(false && "Unknown state !");
                    break;
            }
        }
        // Little hack.
        // I didn't test Android but Emscripten use finger index as finger id.
        // Fast double tap may erase events in recognizers because fingers will have the same finger id.
        // So we has to process attached recognizers before FINGERDOWN event with the same finger id.
        if (isUp) {
            ProcessAttachedRecognizers(it->first, it->second);
        }
    }
}

void TouchManager::AddRecognizers(const std::vector<std::shared_ptr<AbstractRecognizer>> &recognizer) {
    if (!recognizer.empty()) {
        m_Recognizers.reserve(m_Recognizers.size() + recognizer.size());
        m_Recognizers.insert(m_Recognizers.end(), recognizer.begin(), recognizer.end());
        std::sort(m_Recognizers.rbegin(), m_Recognizers.rend(),
                [](const std::shared_ptr<AbstractRecognizer> &l, const std::shared_ptr<AbstractRecognizer> &r) {
                    return *l < *r;
        });
    }
}

void TouchManager::RemoveRecognizer(int id) {
    auto item = std::find_if(m_Recognizers.begin(), m_Recognizers.end(),
                 [id](const std::shared_ptr<AbstractRecognizer> &recognizer) {
                     return recognizer->GetId() == id;
                 });
    if (item != m_Recognizers.end()) {
        // Remove recognizer pointers in m_AttachedRecognizers
        for (auto attachedRecognizersIt = m_AttachedRecognizers.begin(); attachedRecognizersIt != m_AttachedRecognizers.end();) {
            std::vector<AbstractRecognizer*> &recognizers = attachedRecognizersIt->second;
            for (auto pRecognizerIt = recognizers.begin(); pRecognizerIt != recognizers.end();) {
                if (item->get() == *pRecognizerIt) {
                    pRecognizerIt = recognizers.erase(pRecognizerIt);
                } else {
                    ++pRecognizerIt;
                }
            }
            if (recognizers.empty()) {
                attachedRecognizersIt = m_AttachedRecognizers.erase(attachedRecognizersIt);
            } else {
                ++attachedRecognizersIt;
            }
        }

        m_Recognizers.erase(item);
    }
}

void TouchManager::RemoveAllRecognizers() {
    m_Recognizers.clear();
    m_AttachedRecognizers.clear();
}

void TouchManager::ProcessAttachedRecognizers(SDL_FingerID fingerId,
                                              std::vector<AbstractRecognizer *> &attachedRecognizers) {
    bool isFirst = true;

    // recognizers are ordered by zIndex. If first is ready then take it
    for (auto it = attachedRecognizers.begin(); it != attachedRecognizers.end();) {
        AbstractRecognizer *recognizer = *it;
        if (recognizer) {
            RecognizerState state = recognizer->VGetState(fingerId);
            if (state == RecognizerState::EventReady && isFirst) {
                // The first recognizer is ready. Take events, detach other recognizers.
                while (state == RecognizerState::EventReady) {
                    QueueEvent(recognizer->VGetEvent(fingerId));
                    state = recognizer->VGetState(fingerId);
                }
                if (state == RecognizerState::Failed || state == RecognizerState::Done || state == RecognizerState::DoNothing) {
                    DetachAllExcept(fingerId, nullptr);
                } else if (attachedRecognizers.size() != 1) {
                    DetachAllExcept(fingerId, recognizer);
                }
                return;
            }

            switch (state) {
                case RecognizerState::EventReady:
                    assert(!isFirst && "Unknown error !");
                    // It is not the first recognizer.
                    // Waiting recognizers with greater zIndex
                    ++it;
                    break;
                case RecognizerState::DoNothing:
                case RecognizerState::Failed:
                case RecognizerState::Done:
                    if (state == RecognizerState::Done && isFirst) {
                        // Event is end. Detach all
                        DetachAllExcept(fingerId, nullptr);
                        return;
                    }
                    // Detach only this recognizer
                    recognizer->VFingerDetached(fingerId);
                    it = attachedRecognizers.erase(it);
                    break;
                case RecognizerState::Hold:
                    // Detach another recognizers
                    if (attachedRecognizers.size() != 1) {
                        DetachAllExcept(fingerId, recognizer);
                    }
                    return;
                case RecognizerState::Recognizing:
                    // Wait for a recognizer.
                    // Next will not be the first.
                    isFirst = false;
                    ++it;
                    break;
                default:
                    assert(false && "Unknown state !");
                    break;
            }
        }
    }
}
