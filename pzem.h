#ifndef PZEM_H
#define PZEM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float powerFactor;
} PZEMData;

bool pzem_init(char *uart_path);

bool pzem_read(PZEMData *data);

uint16_t crc16_update(uint16_t crc, uint8_t a);

bool pzem_reset_energy(void);

void pzem_close(void);

#endif // PZEM_H
