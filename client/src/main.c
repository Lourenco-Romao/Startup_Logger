#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "system.h"
#include "api.h"
#include "files.h"

#define LOG_FILE "./events-logger.log"
#define PENDING_LOG_FILE "./pending.log"

int main(void)
{
    // Create Startup log event structure and get the hstname and timestamp
    LogEvent event = {
        .hostname = {0},
        .eventType = "startup",
        .timestamp = (int64_t)time(NULL),
        .os = get_operating_system()
    };

    if (get_machine_name(event.hostname, sizeof(event.hostname)) != 0) {
        fprintf(stderr, "Erro ao obter o nome da maquina\n");
        return 1;
    }

    // Store the log event in a file

    if (write_event(LOG_FILE, &event, EVENT_FORMAT_LOG) != 0) {
        return 1;
    }

    // Send the log event to the server
    if (send_startup_event(&event) != 0) {
        fprintf(stderr, "Erro ao enviar evento para API\n");
        if (write_event(PENDING_LOG_FILE, &event, EVENT_FORMAT_PENDING) != 0) {
            return 1;
        }
    }

    return 0;
}