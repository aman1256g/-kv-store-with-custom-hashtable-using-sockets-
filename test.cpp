#include <iostream>
#include<bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
#include "parser.cpp"
#include "commandhandler.cpp"
#include "hashtable.h"

Table table;


bool readfull(int fd , char *buf , int len){
if(len == 0)
return true;
while(len >  0){
    ssize_t rv=read(fd , buf , len);
        if(rv <= 0)
        {
            return false;
            
        }
        len-=(size_t)rv;
            buf+=rv;
}
return true;
}

bool writefull(int fd , const char *buf , int len){

    while(len > 0){
        ssize_t wv=write(fd , buf , len);
        if(wv <0)
        return false;

        len -=(ssize_t)wv;
        buf+=wv;
    }
    return true;
}






int main(){
    table_init(&table , 4);

int serverfd=socket(AF_INET , SOCK_STREAM , 0);
if(serverfd<0)
return 1;


int val=1;
setsockopt(serverfd , SOL_SOCKET , SO_REUSEADDR , &val , sizeof(val));

cout<< "socket created successfull" << serverfd << "this is the socket" ;


struct sockaddr_in addr{};
addr.sin_family=AF_INET;
addr.sin_port=htons(1234);
addr.sin_addr.s_addr=htonl(INADDR_ANY);

if(bind(serverfd , (const struct sockaddr*)&addr , sizeof(addr)) < 0)
{
    cout<<"bind failed";
    close(serverfd);
return 1;
}

if(listen(serverfd , SOMAXCONN) < 0)
{

    cout<<" listen failed";
    close(serverfd);
    return 1;
}

cout<<"server socket made listen successfull";

vector<int> client(0);
client.push_back(serverfd);

while(true){
    fd_set read_fds;
    FD_ZERO(&read_fds);

int max=serverfd;
for(int fd:client){
    FD_SET(fd , &read_fds);
    if(fd>max)
     max=fd;
}


int activity=select(max+1 , &read_fds , nullptr,nullptr , nullptr);
if(activity < 0)
    break;

    struct sockaddr_in client_addr={};
socklen_t addrlen = sizeof(client_addr);


if(FD_ISSET(serverfd  , &read_fds)){
   int conn=accept(serverfd , (struct sockaddr *)&client_addr , &addrlen);
if(conn<0)
{cout<<"connection failed";
return 1;}
client.push_back(conn);
cout<<"new client "<< conn;
}


for(auto it = client.begin();it!=client.end();)
{
int fd=*it;

if(fd == serverfd)
{++it;
continue;}


if(FD_ISSET(fd , &read_fds)){
    uint32_t len = 0;
    if(!readfull(fd , (char *)&len , 4))
   { cout<<"client disconnected";
    it=client.erase(it);
    continue;
}
    
    vector<char> rbuf(len+1 , 0);
    bool t = readfull(fd , rbuf.data() , len);
if(t == false){
    cout<<"client disconnected";
    it=client.erase(it);
}
   

string request(rbuf.data());
vector<string> args = parse(request);
string response=  process_request(table , args);

uint32_t responselen= response.size();
int header = writefull(fd , (char*)&responselen , 4);
if(!writefull(fd ,  response.data() , responselen))
{
    close(fd);
    cout<<"writeback failed";
    client.erase(it);
    continue;
}
cout<<"write successfull"<<request << "--->" << response << endl;
++it;
}
else
++it;

}





}



close(serverfd);
return 0;


}