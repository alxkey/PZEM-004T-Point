#include "app.h"
#include <sys/ioctl.h>
#include <linux/if.h>

int send_packet(int sockfd, struct sockaddr_in *server_addr, const char *data, int len) {
    return sendto(sockfd, data, len, 0, (struct sockaddr *)server_addr, sizeof(*server_addr));
}

int receive_response(int sockfd, char *buffer, int bufsize, int timeout) {
    struct timeval tv = { .tv_sec = timeout, .tv_usec = 0 };
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    int len = recvfrom(sockfd, buffer, bufsize, 0, (struct sockaddr *)&from, &fromlen);
    if (len > 0) buffer[len] = '\0';

    return len;
}

int main() {
    Config config;
    LogConfig logconfig;
    uint8_t crc;
    char buffer[64];
    char msg[70];
    time_t now = time(NULL);

    if (load_config(&config, &logconfig) != 0) {
        printf("Ошибка чтения файла конфигурации.\n");
        return EXIT_FAILURE;
    }

    printf("point name: %s\nip addr: %s\nport: %04d\nuart: %s\nre-query: %02d\nip addr logsever: %s\n",
            config.name, config.server_ip, config.server_port, config.uart_path, config.delay, logconfig.log_ip);

    if (!pzem_init(config.uart_path)) {
        fprintf(stderr, "Ошибка инициализации PZEM\n");
        return EXIT_FAILURE;
    }
    if (!pzem_reset_energy()) {
        fprintf(stderr, "Ошибка сброса счетчика\n");
    }

    log_message(&logconfig, "point name: %s\nip addr: %s\nport: %04d\nuart: %s\nre-query: %02d\nip addr logsever: %s\n",
            config.name, config.server_ip, config.server_port, config.uart_path, config.delay, logconfig.log_ip);

    FILE *file;
    char filename[64];
    strftime(filename, sizeof(filename), "files/%H:%M:%S_%d-%m-%Y.wm", localtime(&now));
    file = fopen(filename, "a");
    if (!file) {
        perror("Ошибка открытия файла");
        pzem_close();
        return EXIT_FAILURE;
    }

    while (1) {
        PZEMData data;
        if (pzem_read(&data)) {
            now = time(NULL);
            struct tm *t = localtime(&now);

            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
              log_message(&logconfig, "Ошибка создания сокета.");
              return EXIT_FAILURE;
            }

            struct sockaddr_in server_addr = { 0 };
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(config.server_port);
            inet_pton(AF_INET, config.server_ip, &server_addr.sin_addr);

            snprintf(msg, sizeof(msg), "D:%s,T:%02d-%02d-%02d,V:%.1f,C:%.3f,P:%.1f,E:%.0f,F:%.1f,PF:%.2f",config.name, 
                     t->tm_hour, t->tm_min, t->tm_sec, data.voltage, data.current, data.power, data.energy, data.frequency, data.powerFactor);
            log_message(&logconfig, "Размер данных : %ld.", strlen(msg));
            crc = checksum8(msg, strlen(msg));
            char scrc5[5];
            char scrc8[8];
            sprintf(scrc5, "0x%2X", crc);
            sprintf(scrc8, ",S:0x%2X", crc);
            printf("CRC-8 = 0x%2X\n", crc);
            log_message(&logconfig, "Контрольная сумма : 0X%02X.", crc);
            strcat(msg,scrc8);
            send_packet(sockfd, &server_addr, msg, strlen(msg));
            log_message(&logconfig, "Отправленная на сервер информация:  %s", msg);
            if (receive_response(sockfd, buffer, sizeof(buffer), 10) > 0) {
                 if (strcmp(buffer,scrc5) == 0)  {
                     log_message(&logconfig, "Принятая контрольная сумма проверена.");
                     log_message(&logconfig, "#######################################\n");
                    }
                 else {
                     log_message(&logconfig, " Ошибка в принятой контрольноьй сумме!");
                    }
                }
            close(sockfd);
            fprintf(file, "T: %02d:%02d:%02d, V: %.1f, C: %.3f, P: %.1f, E: %.0f, F: %.1f, PF: %.2f\n",
                    t->tm_hour, t->tm_min, t->tm_sec,
                    data.voltage, data.current, data.power, data.energy, data.frequency, data.powerFactor);

            printf("Time: %02d:%02d:%02d, Voltage: %.1fV, Current: %.3fA, Power: %.1fW, Energy: %.0fWh, Freq: %.1fHz, PF: %.2f\n",
                   t->tm_hour, t->tm_min, t->tm_sec,
                   data.voltage, data.current, data.power, data.energy, data.frequency, data.powerFactor);
            fflush(file);
        } else {
            fprintf(stderr, "Ожидаем\n");
        }
        sleep(config.delay);
    }
    fclose(file);
    pzem_close();
    return EXIT_SUCCESS;
}

