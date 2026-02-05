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

void handle_client(int fd) {
    char buf[4096] = {0};
    proto_hdr_t *hdr = (proto_hdr_t*) buf;

    hdr->type = htonl(PROTO_HELLO);
    hdr->len = sizeof(int);

    int reallen = hdr->len;
    hdr->len = htons(hdr->len);

    int *data = (int*)&hdr[1];
    *data = htonl(1);

    write(fd, hdr, sizeof(proto_hdr_t) + reallen);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serverInfo = {0};
    struct sockaddr_in clientInfo = {0};
    socklen_t clientSize = 0;

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = 0;
    serverInfo.sin_port = htons(5555);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
       perror("socket");
       return -1;
    }
    printf("%d\n", fd);

    //bind ... asign the address
    //
    // 0: success / -1: error
    //
    //struct sockaddr_in {
    //    sa_family_t     sin_family;     /* AF_INET */
    //    in_port_t       sin_port;       /* Port number */
    //    struct in_addr  sin_addr;       /* IPv4 address */
    //};
    //
    // sys/socket.h
    // int bind(int sckfd, const struct sockaddr *addr, socklen_t addrlen);
    if (bind(fd, (struct sockaddr*) &serverInfo, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        close(fd);
        return -1;
    }

    //listen
    if (listen(fd, 0) == -1) {
        perror("listen");
        close(fd);
        return -1;
    }

    while(1) {
        //accept ... receive connections
        //int accept(int sockfd, struct sockaddr *_Nullable restrict addr, socklen_t *_Nullable restrict addrlen);
        int cfd = accept(fd, (struct sockaddr*) &clientInfo, &clientSize);
        if (cfd == -1) {
            perror("accept");
            close(fd);
            return -1;
        }

        handle_client(cfd);
        close(cfd);
    }
    close(fd);
    return 0;
}
