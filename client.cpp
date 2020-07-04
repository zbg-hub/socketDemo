#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h>
#include <thread>

using namespace std;

void Send(int client_fd);
void Receive(int client_fd);

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        printf("invalid socket !");
        return 0;
    }

    //设置服务端信息
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(10004);
    serAddr.sin_addr.s_addr = inet_addr("192.168.1.104");

    if (connect(client_fd, (sockaddr*)&serAddr, sizeof(serAddr)) < 0)
    {
        printf("connect error !");
        close(client_fd);
        return 0;
    }
    else {
        cout << "连接成功"<<endl;
    }

    char sendData[200] = "hello server";
    send(client_fd, sendData, strlen(sendData), 0);

    //接收服务端发来的第一条消息
    char recData[255];
    while (1) {
        int ret = recv(client_fd, recData, 255, 0);
        if (ret > 0)
        {
            recData[ret] = 0x00;
            cout<<recData<<endl;
            break;
        }
    }
    
    //开启两个线程，一个用于接收消息并显示，另一个用于发送消息
    thread thread1(Send, client_fd);
    thread thread2(Receive, client_fd);

    //阻塞主线程
    while (1) {

    }
    close(client_fd);
    return 0;
}

//发送消息
void Send(int client_fd) {
    char sendData[200];
    while (1) {
        int index = 0;
        while (1) {
            char c = getchar();
            if (c == '\n')
                break;
            sendData[index] = c;
            index++;
        }
        if (index > 0) {
            sendData[index] = '\0';
            send(client_fd, sendData, strlen(sendData), 0);
        }
    }
}

//接收消息并显示
void Receive(int client_fd) {
    char recData[255];
    while (1) {
        memset(recData,'\0',sizeof(recData));
        int ret = recv(client_fd, recData, 255, 0);
        if (ret > 0)
        {
            recData[ret] = '\0';
            cout<<"receive message:"<<recData<<endl;
        }
    }
}

