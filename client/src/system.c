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

int unix_to_utc(int64_t timestamp, struct tm *utc_time)
{
    if (utc_time == NULL) {
        return -1;
    }

    time_t time = (time_t)timestamp;

#ifdef _WIN32

    if (gmtime_s(utc_time, &time) != 0) {
        return -1;
    }

#else

    if (gmtime_r(&time, utc_time) == NULL) {
        return -1;
    }

#endif

    return 0;
}