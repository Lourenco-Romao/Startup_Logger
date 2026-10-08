#ifndef SYSTEM_H
#define SYSTEM_H

int get_machine_name(char *buffer, int64_t size);

typedef struct {
    char *machine;
    char *eventType;
    int64_t timestamp;
} LogEvent;

#endif // SYSTEM_H