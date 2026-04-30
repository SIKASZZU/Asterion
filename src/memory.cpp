#include <iostream>

#if defined(_WIN32)
    #include <windows.h>
    #include <psapi.h>
#elif defined(__linux__)
    #include <sys/resource.h>
    #include <unistd.h>
#endif

int printMemoryUsage() {
    #if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            return static_cast<int>(pmc.WorkingSetSize / 1024 / 1024);
        } else {
            std::cerr << "Failed to get process memory info\n";
            return -1;
        }
    #elif defined(__linux__)
        struct rusage usage{};
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            // On Linux: ru_maxrss is in kilobytes
        #if defined(__APPLE__)
            return static_cast<int>(usage.ru_maxrss / 1024 / 1024);
        #else
            return static_cast<int>(usage.ru_maxrss / 1024);
        #endif
        } else {
            std::cerr << "Failed to get resource usage\n";
            return -1;
        }
    #else
        std::cout << "Memory usage not supported on this platform\n";
        return -1;
    #endif
}