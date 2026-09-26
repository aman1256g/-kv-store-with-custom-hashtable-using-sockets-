#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
using namespace std;


bool readfull(int conn , char *buf , size_t n){
while(n>0)
{
    ssize_t rv=read(conn , buf , n);
    if(rv <=0)
    return false;
    buf+=rv;
    n-=rv;
}
return true;
}


bool writefull(int conn , char* buf , int n){
    while(n>0){
        ssize_t rv=write(conn , buf , n);
        if(rv <= 0)
        return false;
        buf+=rv;
        n-=rv;
    }
    return true;
}


int main(){

int fd=socket(AF_INET , SOCK_STREAM , 0);
if(fd <0)
{   perror("error");
    return 1;
}

int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        perror("bind() failed");
        return 1;
    }

    rv = listen(fd, SOMAXCONN);
    if (rv) {
        perror("listen() failed");
        return 1;
    }

    printf("Server listening on port 1234...\n");

    int client_fds[10] = {0};   // 0 = empty slot

    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        int max_fd = fd;

        for (int i = 0; i < 10; i++) {
            if (client_fds[i] > 0) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) max_fd = client_fds[i];
            }
        }

        select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        // Case 1: a new client is trying to connect.
        if (FD_ISSET(fd, &read_fds)) {
            int conn = accept(fd, NULL, NULL);
            if (conn >= 0) {
                bool placed = false;
                for (int i = 0; i < 10; i++) {
                    if (client_fds[i] == 0) {
                        client_fds[i] = conn;
                        printf("New client in slot %d\n", i);
                        placed = true;
                        break;
                    }
                }
                if (!placed) {
                    printf("No free slot, rejecting connection\n");
                    close(conn);
                }
            }
        }

        // Case 2: an existing client sent a message.
        for (int i = 0; i < 10; i++) {
            if (client_fds[i] <= 0 || !FD_ISSET(client_fds[i], &read_fds)) continue;

            int conn_fd = client_fds[i];

            uint32_t len = 0;
            if (!readfull(conn_fd, (char *)&len, 4)) {
                close(conn_fd);
                client_fds[i] = 0;
                printf("Client in slot %d disconnected\n", i);
                continue;
            }

            char buf[4096] = {};
            if (len > sizeof(buf) || !readfull(conn_fd, buf, len)) {
                close(conn_fd);
                client_fds[i] = 0;
                printf("Client in slot %d disconnected (bad message)\n", i);
                continue;
            }

            printf("Slot %d received: %.*s\n", i, len, buf);

            // --- Command handling goes here in Chapter 7 ---
            writefull(conn_fd, (char *)&len, 4);
            writefull(conn_fd, buf, len);
        }
    }

    close(fd);
    return 0;



}