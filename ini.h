#ifndef INI_H
#define INI_H

#include <stdint.h>

#define CONFIG_FILE "working.ini"

typedef struct {
    char name[16];
    char server_ip[64];
    uint16_t server_port;
    char uart_path[16];
    uint8_t delay;
} Config;

typedef struct {
    char log_ip[64];
    uint16_t log_port;
    char log_name[128];
} LogConfig;


int load_config(Config *config, LogConfig *logconfig);

#endif /* INI_H */

