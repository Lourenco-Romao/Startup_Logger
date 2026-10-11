#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "system.h"
#include "api.h"
#include "files.h"

int main(void)
{
    char log_path[512];
    char pending_path[512]; //the path of the file that holds the jsons pending to be sent to the api

        // Get the local event log path
    if (get_app_data_path("events-logger.log", log_path, sizeof(log_path)) != 0) {
        fprintf(stderr, "Failed to get event log path\n");
        return 1;
    }

    // Get the pending events log path
    if (get_app_data_path("pending.log", pending_path, sizeof(pending_path)) != 0) {
        fprintf(stderr, "Failed to get pending log path\n");
        return 1;
    }

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

    if (write_event(log_path, &event, EVENT_FORMAT_LOG) != 0) {
        return 1;
    }

    process_pending_events(pending_path);

    // Send the log event to the server
    if (send_startup_event(&event) != 0) {
        fprintf(stderr, "Erro ao enviar evento para API\n");
        if (write_event(pending_path, &event, EVENT_FORMAT_PENDING) != 0) {
            return 1;
        }
    }

    return 0;
}