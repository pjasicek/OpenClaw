#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <assert.h>

template <typename T> class Singleton
{
public:
    static T* Instance()
    {
        static T _singleton;
        return &_singleton;
    }

protected:
    Singleton() { }
    ~Singleton() { }
};

#endif