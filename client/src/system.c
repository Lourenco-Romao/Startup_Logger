#include <stdint.h>
#include <time.h>

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
