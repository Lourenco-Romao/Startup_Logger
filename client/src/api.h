#ifndef API_H
#define API_H

#include "system.h"

typedef enum {
    SEND_OK,
    SEND_RETRY,
    SEND_REJECTED
} SendResult;

int send_startup_event(const LogEvent *event);
SendResult send_json(const char *json);

#endif