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
#else
    #error "Unsupported OS"
#endif

// Edit these if you dont like the default color scheme
#define ANSI_COLOR      "\x1b[1;"
#define ACCENT_COLOR    "\x1b[1;34m"
#define BOLD_COLOR      "\x1b[1;37m"
#define TEXT_COLOR      "\x1b[0;37m"
#define ERROR_COLOR     "\x1b[0;91m"

// Was getting some weird behaviour when using a function to create the palette
const char* palette[] = {
    "\x1b[48;5;0m  ",
    "\x1b[48;5;1m  ",
    "\x1b[48;5;2m  ",
    "\x1b[48;5;3m  ",
    "\x1b[48;5;4m  ",
    "\x1b[48;5;5m  ",
    "\x1b[48;5;6m  ",
    "\x1b[48;5;7m  ",
    "\x1b[48;5;8m  ",
    "\x1b[48;5;9m  ",
    "\x1b[48;5;10m  ",
    "\x1b[48;5;11m  ",
    "\x1b[48;5;12m  ",
    "\x1b[48;5;13m  ",
    "\x1b[48;5;14m  ",
    "\x1b[48;5;15m  "
};


char* get_os(void) {
    FILE* os;
    char buff[70];
    char* name = NULL;
    char* version = NULL;

    static char result[255];

    if(!(os = fopen("/etc/os-release", "r"))) {
        sprintf(result, ACCENT_COLOR "os\t" ERROR_COLOR "Unknown OS");
        return result;
    }

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

    if (!name && !version) {
        sprintf(result, ACCENT_COLOR "os\t" ERROR_COLOR "Unknown OS");
        return result;
    }

    sprintf(result, ACCENT_COLOR "os\t" TEXT_COLOR "%s %s", name, version);
    free(name);
    free(version);
    return result;
}

char* get_cpu(void) {
    FILE* cpu_info;
    char buff[512];
    char* cpu_name = NULL;
    double cpu_speed = -1;

    static char result[255];

    if(!(cpu_info = fopen("/proc/cpuinfo", "r"))) {
        sprintf(result, ACCENT_COLOR "cpu\t" ERROR_COLOR "Unknown CPU");
        return result;
    };

    while (fgets(buff, sizeof(buff), cpu_info)) {
        if (strncmp(buff, "model name", 10) == 0) {
            cpu_name = strdup(buff + 13);
            cpu_name[strcspn(cpu_name, "\n")] = 0;
            break;
        }
    }
    fclose(cpu_info);

    if(!(cpu_info = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r"))) {
        sprintf(result, ACCENT_COLOR "cpu\t" ERROR_COLOR "Unknown CPU");
        return result;
    };

    fgets(buff, sizeof(buff), cpu_info);
    cpu_speed = atof(buff) / 1000000;
    fclose(cpu_info);

    int num_threads = get_nprocs();
    sprintf(result, ACCENT_COLOR "cpu\t" TEXT_COLOR "%s (x%d) @ %.2fGHz", cpu_name, num_threads, cpu_speed);
    free(cpu_name);

    return result;
}

char* get_uptime(void) {
    uint64_t uptime = 0;
    static char result[255];

    #ifdef FETCH_LINUX
        struct sysinfo sys;
        sysinfo(&sys);
        uptime = sys.uptime;
    #elif FETCH_BSD
        struct timespec time_spec;
        if(clock_gettime(CLOCK_UPTIME_PRECISE, &time_spec) != 0) { return 0; }
        uptime = (uint64_t) time_spec.tv_sec;
    #else
        #error "Unsupported OS"
    #endif
    
    sprintf(result, ACCENT_COLOR "uptime\t" TEXT_COLOR "%"PRIu64"h %"PRIu64"m", uptime / 60 / 60, uptime / 60 % 60);
    return result;
}

char* get_mem(void) {
    FILE* mem_info;
    char buff_total[255];
    char buff_free[255];

    static char result[512];

    if(!(mem_info = fopen("/proc/meminfo", "r"))) {
        sprintf(result, ACCENT_COLOR "memory\t" ERROR_COLOR "ERROR");
        return result;
    }

    fgets(buff_total, 255, (FILE*)mem_info);
    fgets(buff_free, 255, (FILE*)mem_info);
    fgets(buff_free, 255, (FILE*)mem_info);
    fclose(mem_info);

    strtok(buff_total, " ");
	int total = atoi(strtok(NULL, " ")) / 1024;
    strtok(buff_free, " ");
	int free = atoi(strtok(NULL, " ")) / 1024;

    sprintf(result, ACCENT_COLOR "memory\t" TEXT_COLOR "%dM / %dM", total - free, total);
    return result;
}

char* get_shell(void) {
    static char result[255];

    char* shell_path = getenv("SHELL");
    char* shell = strrchr(shell_path, '/');
    
    if(shell != NULL) {
        sprintf(result, ACCENT_COLOR "shell\t" TEXT_COLOR "%s", shell + 1);
    } else {
        sprintf(result, ACCENT_COLOR "shell\t" TEXT_COLOR "%s", shell_path);
    }
    return result;
}

int main(int argc, char** argv) {
    struct utsname name;
    uname(&name);

    // Stinkyyyyyy :((((((((((
    printf("%s%s\x1b[0m\t" ACCENT_COLOR "%s@" BOLD_COLOR "%s\n", palette[0], palette[8], getenv("USER"), name.nodename);
    printf("%s%s\x1b[0m\t%s\n", palette[1], palette[9], get_os());
    printf("%s%s\x1b[0m\t" ACCENT_COLOR "kernel\t" TEXT_COLOR "%s\n", palette[2], palette[10] , name.release);
    printf("%s%s\x1b[0m\t%s\n", palette[3], palette[11], get_uptime());
    printf("%s%s\x1b[0m\t%s\n", palette[4], palette[12], get_shell());
    printf("%s%s\x1b[0m\t" ACCENT_COLOR "home\t" TEXT_COLOR "%s\n", palette[5], palette[13], getenv("HOME"));
    printf("%s%s\x1b[0m\t%s\n", palette[6], palette[14], get_cpu());
    printf("%s%s\x1b[0m\t%s\n", palette[7], palette[15], get_mem());

    return 0;
}
