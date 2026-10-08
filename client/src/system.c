#include <stdint.h>

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