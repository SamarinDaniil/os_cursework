#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "erproc.h"

int main() {
    int server = Socket(AF_INET, SOCK_STREAM, 0);
    if (server > 0)
        printf(" => Удалось создать серверный сокет.\n");
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(40001);
    int result = Bind(server, (struct sockaddr *) &adr, sizeof adr);
    if (result == 0)
        printf(" => Привязка адреса и порта удалась.\n");
    Listen(server, 1);
    socklen_t adrlen = sizeof adr;
    int fd = Accept(server, (struct sockaddr *) &adr, &adrlen);
    if (fd > 0)
        printf(" => Клиент подключен.\n");
    printf(" => Ожидание процессов клиента!.\n");
    ssize_t nread;
    char buf[1024];
    char *line = NULL;
    size_t lineSize = 0;
    while(true)
    {
        nread = read(fd, buf, 1024);
        if (nread == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, buf, nread);
        printf("\n => Udpate proc: upd\n => End prog: end\n => Id proc for kill:\n");
        size_t charCount = getline(&line,&lineSize,stdin);
        if (charCount > 0)
        {
            write(fd, line, charCount);
            if (strcmp(line,"end\n") == 0)
                break;
            if (strcmp(line,"upd\n") == 0)
                continue;
        }
        nread = read(fd, buf, 1024);
        if (nread == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, buf, nread);
    }
    sleep(5);

    close(fd);
    close(server);
    return 0;
}
