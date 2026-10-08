#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "system.h"

#define LOG_FILE "./bomdia.log"

int main(void)
{
    // Create Startup log event structure and get the hstname and timestamp
    LogEvent event = {
        .hostname = {0},
        .eventType = "startup",
        .timestamp = (int64_t)time(NULL)
    };

    if (get_machine_name(event.hostname, sizeof(event.hostname)) != 0) {
        fprintf(stderr, "Erro ao obter o nome da maquina\n");
        return 1;
    }

    // Convert the timestamp to UTC to 
    struct tm utc_time;

    if (unix_to_utc(event.timestamp, &utc_time) != 0) {
        fprintf(stderr, "Erro ao converter timestamp\n");
        return 1;
    }
    // Store the log event in a file

    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL) {
        return 1;
    }

    fprintf(
        file,
        "[%04d-%02d-%02d] [%02d:%02d:%02d] [%s] %s\n",
        utc_time.tm_year + 1900,
        utc_time.tm_mon + 1,
        utc_time.tm_mday,
        utc_time.tm_hour,
        utc_time.tm_min,
        utc_time.tm_sec,
        event.hostname,
        event.eventType
    );

    fclose(file);
    return 0;
}