#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "erproc.h"

char* execute_ps() {
    char* result = NULL;
    FILE* pipe = popen("ps -a", "r");
    if (pipe != NULL) {
        char buffer[128];
        size_t size = 0;
        char* tmp = NULL;

        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            size_t len = strlen(buffer);
            tmp = realloc(result, size + len);
            if (tmp == NULL) {
                free(result);
                pclose(pipe);
                return NULL;
            }
            result = tmp;
            memcpy(result + size, buffer, len);
            size += len;
        }

        pclose(pipe);
    }
    return result;
}

int main() {
    int fd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(40001);
    Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
    Connect(fd, (struct sockaddr *) &adr, sizeof adr);
    char* ps_result =  NULL;
    char buf[1024];
    ssize_t nread;
    while(true)
    {
        ps_result = execute_ps();
        write(fd, ps_result, strlen(ps_result));
        nread = read(fd, buf, 1024);
        if (nread == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, buf, nread); 
        if (strcmp(buf,"end\n") == 0)
            break;
        if (strcmp(buf,"upd\n") == 0)
            continue;
        else {
            pid_t pid = atoi(buf); 
            int result = kill(pid, SIGKILL);
            if (result == 0) {
                printf("Процесс успешно убит!\n");
                write(fd, "Процесс успешно убит!\n", strlen("Процесс успешно убит!\n"));
            } else {
                write(fd, "Не удалось убить процесс!\n", strlen("Не удалось убить процесс!\n"));
                printf("Не удалось убить процесс!\n");
            }
            write(STDOUT_FILENO, buf, nread); 
        }  
    }
    sleep(15);
    close(fd);
    return 0;
}
