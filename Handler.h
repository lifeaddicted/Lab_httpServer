#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "Sock.h"

/**
 * @brief IO处理器
 */
class Handler
{
    public:
        virtual int handleInput() = 0;
        virtual int handleOutput() = 0;
        virtual void handleClose() = 0;
};

#endif