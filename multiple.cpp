#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

bool read_full(int fd , char *buf , size_t n){
    while(n>0){
        ssize_t rv=read(fd , buf , n);
        if(rv<=0)
        return false;
        buf+=rv;
        n-=rv;
    }
return true;

}

bool write_full(int fd , char *buf , size_t n){
    while(n>0){
        ssize_t rv=write(fd , buf , n);
        if(rv<=0)
        return false;
        buf+=rv;
        n-=rv;
    }
return true;

}





int main() {
int fd=socket(AF_INET , SOCK_STREAM , 0);
if(fd<0)
perror("failed");

struct sockaddr_in addr=
{};
addr.sin_addr.s_addr=htonl(0);
addr.sin_port=htons(1234);
addr.sin_family=AF_INET;
int rv=bind(fd , (struct sockaddr*)&addr , sizeof(addr) );
if(rv<0)
{perror("failed");
return 1;
}
listen(fd, 5);


int client_fds[10]={0};

while(true){
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(fd, &read_fds);
int max_fd=fd;

for(int i=0;i<10;i++){
    if(client_fds[i]>0)
    {
        FD_SET(client_fds[i] , &read_fds);
        if(client_fds[i]>max_fd)
        max_fd=client_fds[i];
    }
}

select(max_fd +1 , &read_fds , NULL,NULL,NULL);

if(FD_ISSET(fd , &read_fds)){
    int conn=accept(fd , NULL ,NULL);
    for(int i=0;i<10;i++){
        if(client_fds[i]==0){
            client_fds[i]=conn;
            printf("new client in slot %d\n" , i);
            break;
        }
    }
}

//  for (int i = 0; i < 10; i++) {
//             if (client_fds[i] > 0 && FD_ISSET(client_fds[i], &read_fds)) {
//                 // char buf[64] = {};
//                 // ssize_t n = read(client_fds[i], buf, sizeof(buf) - 1);
//                 // if (n <= 0) {
//                 //     close(client_fds[i]);
//                 //     client_fds[i] = 0;
//                 //     printf("Client in slot %d disconnected\n", i);
//                 // } else {
//                 //     printf("Slot %d: %s\n", i, buf);
//                 //     write(client_fds[i], buf, n);
//                 // }
//             }
//         }






for (int i = 0; i < 10; i++) {
    if (client_fds[i] > 0 && FD_ISSET(client_fds[i], &read_fds)) {
        int conn_fd = client_fds[i];

        // Step 1: read the 4-byte length header.
        uint32_t len = 0;
        if (!read_full(conn_fd, (char*)&len, 4)) {
            close(conn_fd);
            client_fds[i] = 0;
            printf("Client in slot %d disconnected\n", i);
            continue;
        }

        // Step 2: read exactly `len` bytes of payload.
        char buf[4096] = {};
        if (len > sizeof(buf) || !read_full(conn_fd, buf, len)) {
            close(conn_fd);
            client_fds[i] = 0;
            continue;
        }

        printf("Slot %d received: %.*s\n", i, len, buf);

        // Step 3: echo back, same framing.
        write_full(conn_fd, (char*)&len, 4);
        write_full(conn_fd, buf, len);
    }
}
    }
}





