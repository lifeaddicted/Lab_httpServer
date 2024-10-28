#ifndef __LOGGERMGR_H__
#define __LOGGERMGR_H__

#include "easylogging++.h"

class LoggerMgr {
    public:
        void init();
};

void LoggerMgr::init()
{
    el::Loggers::getLogger("network");
}


#define NW_DBG(__VA__ARGS__...) CLOG(DEBUG, __VA__ARGS__)

#endif