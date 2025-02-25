#include "ini.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int load_config(Config *config, LogConfig *logconfig) {
    char logfile_name[64];
    time_t now = time(NULL);
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) {
        perror("Ошибка открытия .ini файла!\n");
        return -1;
    }
    while (!feof(file)) {
        char key[64], value[128];
        if (fscanf(file, "%63[^=]=%127s\n", key, value) == 2) {
            if (strcmp(key, "server_ip") == 0) {
                strncpy(config->server_ip, value, sizeof(config->server_ip) - 1);
            } else if(strcmp(key, "name") == 0) {
                strncpy(config->name, value, sizeof(config->name) - 1);
            } else if (strcmp(key, "server_port") == 0) {
                config->server_port = (uint16_t)atoi(value);
            } else if (strcmp(key, "uart_path") == 0) {
                strncpy(config->uart_path, value, sizeof(config->uart_path) - 1);
            } else if (strcmp(key, "delay") == 0) {
                config->delay = (uint8_t)atoi(value);
            } else if(strcmp(key, "log_ip") == 0) {
                strncpy(logconfig->log_ip, value, sizeof(logconfig->log_ip) - 1);
            } else if (strcmp(key, "log_port") == 0) {
                logconfig->log_port = (uint16_t)atoi(value);
            } 
        }
    }
    strftime(logfile_name, sizeof(logfile_name), "logs/%H:%M:%S_%d-%m-%Y.log", localtime(&now));
    strncpy(logconfig->log_name, logfile_name, sizeof(logconfig->log_name) - 1);
    fclose(file);
    return 0;
}
