#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "system.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

int get_machine_name(char *buffer, size_t size)
{
#ifdef _WIN32

    DWORD buffer_size = (DWORD)size;

    if (!GetComputerNameA(buffer, &buffer_size)) {
        return -1;
    }

#else

    if (gethostname(buffer, size) != 0) {
        return -1;
    }

    buffer[size - 1] = '\0';

#endif

    return 0;
}

int unix_to_local(int64_t timestamp, struct tm *local_time)
{
    if (local_time == NULL) {
        return -1;
    }

    time_t time = (time_t)timestamp;

#ifdef _WIN32

    if (localtime_s(local_time, &time) != 0) {
        return -1;
    }

#else

    if (localtime_r(&time, local_time) == NULL) {
        return -1;
    }

#endif

    return 0;
}

const char *get_operating_system(void)
{
#if defined(_WIN32)
    return "windows";
#elif defined(__ANDROID__)
    return "android";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}

int get_app_data_path(const char *filename, char *buffer, size_t buffer_size) {
    if (filename == NULL || buffer == NULL || buffer_size == 0) {
        return -1;
    }

#ifdef _WIN32

    // Get the Windows ProgramData directory
    const char *program_data = getenv("ProgramData");

    if (program_data == NULL) {
        return -1;
    }

    char directory[MAX_PATH];

    // Build the StartupLogger directory path
    int written = snprintf(directory, sizeof(directory), "%s\\StartupLogger", program_data);

    if (written < 0 || (size_t)written >= sizeof(directory)) {
        return -1;
    }

    // Create the StartupLogger directory if it does not exist
    if (!CreateDirectoryA(directory, NULL)) {

        DWORD error = GetLastError();

        // ERROR_ALREADY_EXISTS is not considered an error
        if (error != ERROR_ALREADY_EXISTS) {
            return -1;
        }
    }

    // Build the full file path
    written = snprintf(buffer, buffer_size, "%s\\%s", directory, filename);

    if (written < 0 || (size_t)written >= buffer_size) {
        return -1;
    }

#else

    // On Linux, store the file in the current directory
    int written = snprintf(buffer, buffer_size, "./%s", filename);

    if (written < 0 || (size_t)written >= buffer_size) {
        return -1;
    }

#endif

    return 0;
}