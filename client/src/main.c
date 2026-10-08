#include <stdio.h>
#include <time.h>
#include <stdint.h>

#include "system.h"

#define LOG_FILE "./bomdia.log"

int main(void)
{
    LogEvent event = {
        .hostname = {0},
        .eventType = "Startup",
        .timestamp = (int64_t)time(NULL)
    };

    if (get_machine_name(event.hostname, sizeof(event.hostname)) != 0) {
        fprintf(stderr, "Erro ao obter o nome da maquina\n");
        return 1;
    }

    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL) {
        return 1;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    fprintf(
        file,
        "[%04d-%02d-%02d] [%02d:%02d:%02d] Windows Iniciado\n",
        local->tm_year + 1900,
        local->tm_mon + 1,
        local->tm_mday,
        local->tm_hour,
        local->tm_min,
        local->tm_sec
    );

    fclose(file);
    return 0;
}