#ifndef SYSTEM_H
#define SYSTEM_H

#include <time.h>
#include <stddef.h>

#define MACHINE_NAME_SIZE 256

typedef struct {
    char hostname[MACHINE_NAME_SIZE];
    const char *eventType;
    int64_t timestamp;
    const char *os;
} LogEvent;

int get_machine_name(char *buffer, size_t size);

const char *get_operating_system(void);

int unix_to_local(int64_t timestamp, struct tm *utc_time);

#endif // SYSTEM_H