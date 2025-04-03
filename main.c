#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TEMP_FILE "/sys/class/hwmon/hwmon5/temp2_input"
#define CPU_FREQ_PATH "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq"
#define GPU_FREQ_PATH "/sys/class/drm/card0/gt_max_freq_mhz"
#define CPU_MIN_FREQ 400000
#define CPU_MAX_FREQ 4700000
#define GPU_MIN_FREQ 300
#define GPU_MAX_FREQ 1450
#define CPU_STEP 100000
#define GPU_STEP 50
#define NUM_CPUS 20
#define TEMP_LOWER_LIMIT 78000
#define TEMP_UPPER_LIMIT 80000
#define INTERVAL_SEC 0
#define INTERVAL_NSEC 500000000 // 0.5 secondi

int read_temperature() {
    FILE *file = fopen(TEMP_FILE, "r");
    if (!file) return -1;
    int temp;
    fscanf(file, "%d", &temp);
    fclose(file);
    return temp;
}

void set_cpu_frequency(int freq) {
    char path[128];
    for (int i = 0; i < NUM_CPUS; i++) {
        snprintf(path, sizeof(path), CPU_FREQ_PATH, i);
        FILE *file = fopen(path, "w");
        if (file) {
            fprintf(file, "%d", freq);
            fclose(file);
        }
    }
}

void set_gpu_frequency(int freq) {
    FILE *file = fopen(GPU_FREQ_PATH, "w");
    if (file) {
        fprintf(file, "%d", freq);
        fclose(file);
    }
}

int main() {
    int cpu_freq = CPU_MAX_FREQ;
    int gpu_freq = GPU_MAX_FREQ;
    struct timespec req = {INTERVAL_SEC, INTERVAL_NSEC};

    while (1) {
        int temp = read_temperature();
        if (temp == -1) {
            perror("Error reading temperature");
            break;
        }

        printf("%d m°C - ", temp);

        if (temp >= TEMP_UPPER_LIMIT) {
            cpu_freq = (cpu_freq - CPU_STEP >= CPU_MIN_FREQ) ? cpu_freq - CPU_STEP : CPU_MIN_FREQ;
            gpu_freq = (gpu_freq - GPU_STEP >= GPU_MIN_FREQ) ? gpu_freq - GPU_STEP : GPU_MIN_FREQ;
            printf("CPU %d kHz - GPU %d MHz\n", cpu_freq, gpu_freq);
        } else if (temp <= TEMP_LOWER_LIMIT) {
            cpu_freq = (cpu_freq + CPU_STEP <= CPU_MAX_FREQ) ? cpu_freq + CPU_STEP : CPU_MAX_FREQ;
            gpu_freq = (gpu_freq + GPU_STEP <= GPU_MAX_FREQ) ? gpu_freq + GPU_STEP : GPU_MAX_FREQ;
            printf("CPU %d kHz - GPU %d MHz\n", cpu_freq, gpu_freq);
        } else {
            printf("-\n");
        }

        set_cpu_frequency(cpu_freq);
        set_gpu_frequency(gpu_freq);

        nanosleep(&req, NULL);
    }
    return 0;
}
