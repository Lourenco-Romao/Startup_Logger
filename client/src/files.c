#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "files.h"
#include "api.h"

int write_event(const char *path, const LogEvent *event, EventFormat format) {
    if (path == NULL || event == NULL) {
        return 1;
    }

    if (format != EVENT_FORMAT_LOG && format != EVENT_FORMAT_PENDING) {
        return 1;
    }

    FILE *file = fopen(path, "a");

    if (file == NULL) {
        perror("Failed to open the file");
        return 1;
    }

    int written = -1;

    if (format == EVENT_FORMAT_PENDING) {

        written = fprintf(
            file,
            "{"
                "\"hostname\":\"%s\","
                "\"eventType\":\"%s\","
                "\"os\":\"%s\","
                "\"timestamp\":%" PRId64
            "}\n",
            event->hostname,
            event->eventType,
            event->os,
            event->timestamp
        );

    } else {

        struct tm utc_time;

        if (unix_to_local(event->timestamp, &utc_time) != 0) {
            fprintf(stderr, "Erro ao converter timestamp\n");
            fclose(file);
            return 1;
        }

        written = fprintf(
            file,
            "[%04d-%02d-%02d] [%02d:%02d:%02d] [%s] %s\n",
            utc_time.tm_year + 1900,
            utc_time.tm_mon + 1,
            utc_time.tm_mday,
            utc_time.tm_hour,
            utc_time.tm_min,
            utc_time.tm_sec,
            event->hostname,
            event->eventType
        );
    }

    if (written < 0) {
        fclose(file);
        return 1;
    }

    if (fclose(file) != 0) {
        return 1;
    }

    return 0;
}

int process_pending_events(const char *path)
{
    if (path == NULL) {
        return 1;
    }

    FILE *file = fopen(path, "r");

    if (file == NULL) {
        // Se ainda não existe pending.log, não há nada para enviar
        return 0;
    }

    char line[512];

    while (fgets(line, sizeof(line), file) != NULL) {

        if (send_json(line) != 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);

    // Todos os eventos foram enviados com sucesso.
    // Esvaziar o pending.log.
    file = fopen(path, "w");

    if (file == NULL) {
        return 1;
    }

    fclose(file);

    return 0;
}