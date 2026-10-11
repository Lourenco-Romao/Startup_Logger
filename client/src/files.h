#ifndef QUEUE_H
#define QUEUE_H

#include "system.h"

typedef enum {
    EVENT_FORMAT_LOG,
    EVENT_FORMAT_PENDING
} EventFormat;

int write_event(const char *path, const LogEvent *event, EventFormat format);

#endif