#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "ini.h"
#include "logger.h"
#include "checksum.h"
#include "pzem.h"

int send_packet(int sockfd, struct sockaddr_in *server_addr, const char *data, int len);
int receive_response(int sockfd, char *buffer, int bufsize, int timeout);

#endif /* APP_H */
