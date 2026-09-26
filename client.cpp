#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

bool read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) return false;
        buf += rv;
        n -= rv;
    }
    return true;
}

bool write_full(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) return false;
        buf += rv;
        n -= rv;
    }
    return true;
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0x7f000001);  // 127.0.0.1 in network byte order

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect failed");
        return 1;
    }

    const char *msg = "hello aman";
    uint32_t len = strlen(msg);

    write_full(fd, (char*)&len, 4);   // send length header
    write_full(fd, msg, len);         // send actual message

    uint32_t reply_len;
    read_full(fd, (char*)&reply_len, 4);
    char buf[4096] = {};
    read_full(fd, buf, reply_len);

    printf("Server replied: %.*s\n", reply_len, buf);

    close(fd);
    return 0;
}