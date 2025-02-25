#include "logger.h"
#include <string.h>
#include <unistd.h>

void log_message(LogConfig *logconfig, const char *format, ...) {
    char buffer[1024];
    time_t now = time(NULL);
    FILE *log_file = fopen(logconfig->log_name, "a");
    if (!log_file) return;

    struct tm *tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Запись в файл
    fprintf(log_file, "[%s] %s\n", time_str, buffer);
    fclose(log_file);

    // Отправка в syslog-сервер
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) return;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(logconfig->log_port);
    inet_pton(AF_INET, logconfig->log_ip, &server_addr.sin_addr);

    char syslog_msg[1060];  // Префикс + время + сообщение
    snprintf(syslog_msg, sizeof(syslog_msg), "<13> %s Point 1: %s", time_str, buffer);

    sendto(sockfd, syslog_msg, strlen(syslog_msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    close(sockfd);
}
