#ifndef __LOGGERMGR_H__
#define __LOGGERMGR_H__

#include "easylogging++.h"

class LoggerMgr {
    public:
        static void init()
        {
            el::Loggers::getLogger("network");
        }
};

// void LoggerMgr::init()
// {
//     el::Loggers::getLogger("network");
// }


#define NW_DBG() CLOG(DEBUG, "network")

#endif