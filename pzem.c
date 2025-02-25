#include "pzem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define PZEM_DEFAULT_ADDR 0xf8

static int uart_fd = -1;

// Инициализация UART
bool pzem_init(char *uart_path) {
    uart_fd = open(uart_path, O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart_fd == -1) {
        perror("Ошибка открытия UART!");
        return false;
    }

    struct termios options;
    tcgetattr(uart_fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag = CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    tcflush(uart_fd, TCIFLUSH);
    tcsetattr(uart_fd, TCSANOW, &options);

    return true;
}

// Отправка команды
bool pzem_send_command(uint8_t *cmd, size_t cmd_size) {
    if (uart_fd == -1) return false;
    if (write(uart_fd, cmd, cmd_size) != cmd_size) {
        perror("Ошибка отправки команды!");
        return false;
    }
    return true;
}

// Чтение данных
bool pzem_read(PZEMData *data) {
    if (uart_fd == -1) return false;

    uint8_t request[] = { PZEM_DEFAULT_ADDR, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x64, 0x64 };
    if (!pzem_send_command(request, sizeof(request))) return false;

    sleep(1);

    uint8_t response[25];
    int n = read(uart_fd, response, sizeof(response));
    if (n < 23) {
        perror("Ошибка чтения данных!");
        return false;
    }

    data->voltage = (response[3] << 8 | response[4]) / 10.0;
    data->current = (response[5] << 8 | response[6] | response[7] << 24 | response[8] << 16 ) / 1000.0;
    data->power = (response[9] << 8 | response[10] | response[11] << 24 | response[12] << 16) / 10.0;
    data->energy = (response[13] << 8 | response[14] | response[15] << 24 | response[16] << 16);
    data->frequency = (response[17] << 8 | response[18]) / 10.0;
    data->powerFactor = (response[19] << 8 | response[20]) / 100.0;
    return true;
}

uint16_t crc16_update(uint16_t crc, uint8_t a) {
int i;

crc ^= a;
for (i = 0; i < 8; ++i)
{
    if (crc & 1)
    crc = (crc >> 1) ^ 0xA001;
    else
    crc = (crc >> 1);
}
return crc;
}

// Сброса счетчика
bool pzem_reset_energy(void) {
    uint16_t crc = 0xffff;
    uint8_t reset_cmd[] = { PZEM_DEFAULT_ADDR, 0x42, 0x00, 0x00};
    crc = crc16_update(crc, PZEM_DEFAULT_ADDR);
    crc = crc16_update(crc, 0x42);
    reset_cmd[2] = crc & 0xFF;
    reset_cmd[3] = crc >> 8 & 0xFF;
    pzem_send_command(reset_cmd, sizeof(reset_cmd));
    sleep(1);
    uint8_t response[25];
    read(uart_fd, response, sizeof(response));
    return true;
}

// Закрытие UART
void pzem_close(void) {
    if (uart_fd != -1) {
        close(uart_fd);
        uart_fd = -1;
    }
}
