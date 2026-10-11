#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "files.h"
#include "api.h"

#ifdef _WIN32
#include <windows.h>
#endif

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

static int replace_file(const char *source, const char *destination)
{
#ifdef _WIN32

    if (!MoveFileExA(
        source,
        destination,
        MOVEFILE_REPLACE_EXISTING
    )) {
        fprintf(
            stderr,
            "Failed to replace file. Windows error: %lu\n",
            GetLastError()
        );

        return 1;
    }

#else

    if (rename(source, destination) != 0) {
        perror("Failed to replace file");
        return 1;
    }

#endif

    return 0;
}

int process_pending_events(const char *path) {
    if (path == NULL) {
        return 1;
    }

    FILE *pending = fopen(path, "r");

    if (pending == NULL) {
        // If pending.log doesn't exist, there is nothing to send
        return 0;
    }

    char temp_path[512];

    int written = snprintf(
        temp_path,
        sizeof(temp_path),
        "%s.tmp",
        path
    );

    if (written < 0 || (size_t)written >= sizeof(temp_path)) {
        fclose(pending);
        return 1;
    }

    FILE *temp = fopen(temp_path, "w");

    if (temp == NULL) {
        fclose(pending);
        return 1;
    }

    char line[512];
    int send_failed = 0;

    while (fgets(line, sizeof(line), pending) != NULL) {
        /*
         * If a temporary error already occurred,
         * keep the remaining events without trying to send them.
         */
        if (send_failed) {
            if (fputs(line, temp) == EOF) {
                fclose(pending);
                fclose(temp);
                return 1;
            }
            continue;
        }
        SendResult result = send_json(line);

        if (result == SEND_OK) {
            /*
             * Event was successfully sent.
             * Do not copy it to the temporary file.
             */
            continue;
        }
        if (result == SEND_REJECTED) {
            /*
             * The server rejected the event (HTTP 400).
             * Do not keep it because retrying would fail again.
             */
            fprintf(
                stderr,
                "Discarding invalid pending event: %s",
                line
            );
            continue;
        }
        if (result == SEND_RETRY) {
            /*
             * A temporary error occurred.
             * Keep this event and stop trying the remaining ones.
             */
            send_failed = 1;

            if (fputs(line, temp) == EOF) {
                fclose(pending);
                fclose(temp);
                return 1;
            }
        }
    }

    if (ferror(pending)) {
        fclose(pending);
        fclose(temp);
        return 1;
    }

    if (fclose(pending) != 0) {
        fclose(temp);
        return 1;
    }

    if (fclose(temp) != 0) {
        return 1;
    }

    if (replace_file(temp_path, path) != 0) {
        return 1;
    }

    return send_failed ? 1 : 0;
}