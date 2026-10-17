#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "api.h"

#ifndef API_BASE_URL
#define API_BASE_URL "http://localhost"
#endif

#define EVENTS_ENDPOINT API_BASE_URL "/api/v1/events"

#ifndef API_KEY
#define API_KEY NULL
#endif

SendResult send_startup_event(const LogEvent *event)
{

    if (event == NULL) {
        fprintf(stderr, "Invalid event: event is NULL\n");
        return SEND_REJECTED;
    }


    char json[512];

    int written = snprintf(
        json,
        sizeof(json),
        "{"
            "\"hostname\":\"%s\","
            "\"eventType\":\"%s\","
            "\"os\":\"%s\","
            "\"timestamp\":%" PRId64
        "}",
        event->hostname,
        event->eventType,
        event->os,
        event->timestamp
    );

    if (written < 0 || (size_t)written >= sizeof(json)) {
        return SEND_REJECTED;
    }

    return send_json(json);
}

SendResult send_json(const char *json)
{
    if (json == NULL) {
        fprintf(stderr, "Cannot send NULL JSON\n");
        return SEND_REJECTED;
    }
    //----------- get api key --------------------------
    const char *api_key = API_KEY;

    if (api_key == NULL || api_key[0] == '\0') {
        fprintf(stderr, "API key is not configured\n");
        return SEND_RETRY;
    }
    //--------------------------------------------------

    CURL *curl = NULL;
    struct curl_slist *headers = NULL;

    CURLcode result;
    long status_code;

    curl = curl_easy_init();

    if (curl == NULL) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return SEND_RETRY;
    }

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json"
    );

    if (headers != NULL) {
        size_t header_size = strlen(api_key) + sizeof("X-API-Key: ");
        char *auth_header = malloc(header_size);

        if (auth_header != NULL) {
            snprintf(auth_header, header_size, "X-API-Key: %s", api_key);

            struct curl_slist *updated = curl_slist_append(
                headers,
                auth_header
            );

            free(auth_header);

            if (updated != NULL) {
                headers = updated;
            } else {
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
                return SEND_RETRY;
            }
        } else {
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return SEND_RETRY;
        }
    } else {
        curl_easy_cleanup(curl);
        return SEND_RETRY;
    }

    curl_easy_setopt(curl, CURLOPT_URL, EVENTS_ENDPOINT);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

    // Maximum time allowed to establish the connection
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

    // Maximum time allowed for the entire request
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        fprintf(
            stderr,
            "Erro CURL: %s\n",
            curl_easy_strerror(result)
        );

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        return SEND_RETRY;
    }

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &status_code
    );

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (status_code == 400 || status_code == 422) {
        fprintf(
            stderr,
            "Event rejected by API: HTTP %ld\n",
            status_code
        );
        return SEND_REJECTED;
    }

    if (status_code == 201) {
        return SEND_OK;
    }

    fprintf(
        stderr,
        "Unexpected API response: HTTP %ld\n",
        status_code
    );
    return SEND_RETRY;
}