#ifndef API_H
#define API_H

#include "system.h"

int send_startup_event(const LogEvent *event);
int send_json(const char *json);

#endif