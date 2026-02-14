#include <complex.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

typedef enum {
   PROTO_HELLO,
} proto_type_e;

typedef struct {
   proto_type_e type;
   unsigned short len;
} proto_hdr_t;

void handle_server(int fd) {
    char buf[4096] = {0};
    read(fd, buf, sizeof(proto_hdr_t) + sizeof(int));

    proto_hdr_t *hdr = (proto_hdr_t*) buf;
    hdr->type = ntohl(hdr->type);
    hdr->len = ntohs(hdr->len);

    int *data = (int*)&hdr[1];
    *data = ntohl(*data);

    if (hdr->type != PROTO_HELLO) {
        printf("Protocol mismatch, failing.\n");
        return;
    }

    if (*data != 1) {
       printf("Protocol version mismatch, failing\n");
       return;
    }

    printf("Server connected, protocol v1\n");
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serverInfo = {0};
    struct sockaddr_in clientInfo = {0};
    socklen_t clientSize = 0;

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = 0;
    serverInfo.sin_port = htons(8080);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
       perror("socket");
       return -1;
    }
    printf("%d\n", fd);

    if (connect(fd, (struct sockaddr*)&serverInfo, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        close(fd);
        return -1;
    }

    handle_server(fd);
    close(fd);
    return 0;
}
