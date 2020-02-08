#include "EventMgr.h"
#include "../SharedDefines.h"

static IEventMgr* g_pEventMgr = NULL;
GenericObjectFactory<IEventData, EventType> g_EventFactory;

IEventMgr* IEventMgr::Get()
{
    assert(g_pEventMgr);
    return g_pEventMgr;
}
IEventMgr::IEventMgr(const char* pName, bool setAsGlobal)
{
    if (setAsGlobal)
    {
        if (g_pEventMgr)
        {
            LOG_ERROR("Attempting to create two global event managers! The old one will be destroyed an doverwritten with this one");
            SAFE_DELETE(g_pEventMgr);
        }

        g_pEventMgr = this;
    }
}

IEventMgr::~IEventMgr()
{
    if (g_pEventMgr == this) {
        g_pEventMgr = nullptr;
    }
//    if (g_pEventMgr)
//    {
//        SAFE_DELETE(g_pEventMgr);
//    }
}