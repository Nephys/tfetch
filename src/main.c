#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <inttypes.h>

// Quick and dirty but gets the work done

#ifdef __linux__
    #include <sys/sysinfo.h>
    #define FETCH_LINUX
#elif __FreeBSD__
    #include <time.h>
    #define FETCH_BSD
#endif

char* get_os(void) {
    FILE* os;
    char buff[70];
    char* name = NULL;
    char* version = NULL;

    static char result[255];

    if(!(os = fopen("/etc/os-release", "r"))) { return (char*) "Unknown UNIX System"; }

    while (fgets(buff, sizeof(buff), os)) {
        // Check if the line contains NAME or VERSION
        if (strstr(buff, "NAME=")) {
            name = strdup(strchr(buff, '=') + 1);
            // Remove quotes if present
            memmove(&name[strcspn(name, "\"")], &name[strcspn(name, "\"") + 1], strlen(name) - strcspn(name, "\""));
            memmove(&name[strcspn(name, "\"")], &name[strcspn(name, "\"") + 1], strlen(name) - strcspn(name, "\""));
            // Remove trailing newline character if present
            name[strcspn(name, "\n")] = 0;
        } else if (strstr(buff, "VERSION=")) {
            version = strdup(strchr(buff, '=') + 1);
            // Remove quotes if present
            memmove(&version[strcspn(version, "\"")], &version[strcspn(version, "\"") + 1], strlen(version) - strcspn(version, "\""));
            memmove(&version[strcspn(version, "\"")], &version[strcspn(version, "\"") + 1], strlen(version) - strcspn(version, "\""));
            // Remove trailing newline character if present
            version[strcspn(version, "\n")] = 0;
        }

        if (name && version){ break; }
    }
    fclose(os);

    if (!name && !version) { return (char*) "Unknown UNIX System"; }

    sprintf(result, "%s %s", name, version);
    free(name);
    free(version);
    return result;
}

char* get_cpu(void) {
    FILE* cpu_info;
    char buff[1024];
    char* cpu_name = NULL;
    double cpu_speed = -1;

    static char result[2048];

    if(!(cpu_info = fopen("/proc/cpuinfo", "r"))) { return (char*) "Unknown CPU"; };

    while (fgets(buff, sizeof(buff), cpu_info)) {
        if (strncmp(buff, "model name", 10) == 0) {
            cpu_name = strdup(buff + 13);
            cpu_name[strcspn(cpu_name, "\n")] = 0;
            break;
        }
    }
    fclose(cpu_info);
    if(!(cpu_info = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r"))) { return (char*) "Unknown CPU"; };
    fgets(buff, sizeof(buff), cpu_info);
    cpu_speed = atof(buff) / 1000000;
    fclose(cpu_info);

    int num_threads = get_nprocs();
    sprintf(result, "%s (x%d) @ %.2fGHz", cpu_name, num_threads, cpu_speed);
    free(cpu_name);
    return result;
}

uint64_t get_uptime(void) {
    #ifdef FETCH_LINUX
        struct sysinfo sys;
        sysinfo(&sys);
        return sys.uptime;
    #elif FETCH_BSD
        struct timespec time_spec;
        if(clock_gettime(CLOCK_UPTIME_PRECISE, &time_spec) != 0) { return 0; }
        return (uint64_t) time_spec.tv_sec;
    #else
        #error "Unsupported OS"
    #endif
}

char* get_ram(void) {
    FILE* mem_info;
    char buff_total[255];
    char buff_free[255];

    static char result[30];

    if(!(mem_info = fopen("/proc/meminfo", "r"))) { return (char*)"ERROR"; }

    fgets(buff_total, 255, (FILE*)mem_info);
    fgets(buff_free, 255, (FILE*)mem_info);
    fgets(buff_free, 255, (FILE*)mem_info);
    fclose(mem_info);

    strtok(buff_total, " ");
	int total = atoi(strtok(NULL, " ")) / 1024;
    strtok(buff_free, " ");
	int free = atoi(strtok(NULL, " ")) / 1024;

    sprintf(result, "%dM / %dM", total - free, total);

    return result;
}

char* ansi_color(int id) {
    static char result[47];
    sprintf(result, "\x1b[48;5;%dm  \x1b[48;5;%dm  \x1b[0m", id, id + 8);
    return result;
}

int main(int argc, char** argv) {
    struct utsname name;
    uname(&name);

    printf("%s\t\x1b[1;34m%s@\x1b[1;37m%s\n", ansi_color(0), getenv("USER"),name.nodename);
    printf("%s\t\x1b[1;34mos\t\x1b[0;37m%s\n", ansi_color(1), get_os());
    printf("%s\t\x1b[1;34mkernel\t\x1b[0;37m%s\n", ansi_color(2), name.release);
    printf("%s\t\x1b[1;34muptime\t\x1b[0;37m%"PRIu64"h %"PRIu64"m\n", ansi_color(3), get_uptime() / 60 / 60, get_uptime() / 60 % 60);
    printf("%s\t\x1b[1;34mshell\t\x1b[0;37m%s\n", ansi_color(4), getenv("SHELL"));
    printf("%s\t\x1b[1;34mhome\t\x1b[0;37m%s\n", ansi_color(5), getenv("HOME"));
    printf("%s\t\x1b[1;34mcpu\t\x1b[0;37m%s\033[0m\n", ansi_color(6), get_cpu());
    printf("%s\t\x1b[1;34mmemory\t\x1b[0;37m%s\033[0m\n", ansi_color(7), get_ram());

    return 0;
}
