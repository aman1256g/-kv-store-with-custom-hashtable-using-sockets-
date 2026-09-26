#include<arpa/inet.h>
#include<cstdio>
#include<cstring>
#include<sys/socket.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<bits/stdc++.h>
using namespace std;
#include<thread>
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




void handle(int conn){
cout<<"client connected \n"<<endl;
while(true){
    
    uint32_t len=0;
    if(!read_full(conn , (char*)&len , 4))
    break;
    char buf[4096]={};
    if(len > sizeof(buf) || !read_full(conn ,buf , len))
        break;
        printf("received %.*s" , len ,buf);

        write_full(conn , (char*)&len , 4);
         write_full(conn , buf , len);

}

close(conn);
cout<<"client disconnected\n"<<endl;
}






int main(){

    int fd=socket(AF_INET , SOCK_STREAM , 0 );
    if(fd<0){
        perror("socket failed");
    return 1;
    }

    struct sockaddr_in addr ={};
    addr.sin_addr.s_addr=htonl(0);
    addr.sin_family=AF_INET;
    addr.sin_port=htons(1234); 

   int rv=bind(fd , (const struct sockaddr*)&addr , sizeof(addr));
   if(rv)
   {
    perror("bind() , failed");
    return 1;
   }


   rv=listen(fd , 4);
   if(rv){
    perror("socket listing");
    return 1;
   }

   cout<<endl<<"working listening on port 1234" ;
 

// fcntl(fd , F_SETFL , O_NONBLOCK);

//    while(true)
//    {

    //------------------------------------------for blocking client single accept------------------------------------------------------------------------------------------
    // struct sockaddr_in client={};
    // socklen_t addrlen=sizeof(client);

    // int conn_fd=accept(fd,(struct sockaddr*)&client , & addrlen);
    // if(conn_fd < 0)
    // continue;



// int conn_fd=accept(fd , NULL ,NULL);
// if(conn_fd < 0){
//     if(errno==EAGAIN || errno==EWOULDBLOCK )
//     printf("no one still \n");
//     continue;
// }
//     // perror("accept");
//     // continue;

    // cout<<"client connected\n"<<endl;
   
    while(true) 
    {
        


int conn=accept(fd , NULL ,NULL);
if(conn < 0)
continue;
thread t(handle  , conn);
t.detach();
  

//     if(!read_full(conn_fd , (char *)&len , 4)){
//         close(conn_fd);
//         continue;
//     }

// char buf[4096]={};
// if(len>sizeof(buf)){
//     close(conn_fd);
//     continue;
// }
// if(!read_full(conn_fd ,buf ,len)){
//     close(conn_fd);
//     continue;
// }

// printf("received %.*s\n" , len , buf ); //  %.*s ->  says print exactly len characters from buf  i am taking bytes from the byte maybe it doesnt contain /0 at the end of string so where it would stop if print directly 
// // %.*s -> write till precision
// write_full(conn_fd, (char*)&len, 4);
// write_full(conn_fd, buf, len);
//    }
// }

   }
}