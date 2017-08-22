#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include <vector>
#include <algorithm>

template <typename T>
class Subject
{
public:
    virtual ~Subject() { }

    void AddObserver(T* pObserver) { m_Observers.push_back(pObserver); }
    void RemoveObserver(T* pObserver) { m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), pObserver), m_Observers.end()); }

protected:
    std::vector<T*> m_Observers;
};

#endif