#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <sys/signal.h>

using namespace std;

#define MAX_READ_LINE 1024

void* recAndSend(void* arg);

//线程的参数结构
struct parameter
{
        int rec;
        int send;
};

int main(void) {
	int client_fd[2];
	pthread_t thread1;
	pthread_t thread2;
	char buff[200];
	int recv_len = -1;
	int conn_fd = -1;
	int ret = -1;
	int server_ip_port = 10004;
	
	//设置服务端ip端口信息
	struct sockaddr_in clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = htons(server_ip_port);

	//创建一个socket，用于绑定和监听
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		fprintf(stderr, "socket error %s errno: %d\n", strerror(errno), errno);
		return 0;
	}
	
	//绑定socket连接和服务端信息
	ret = bind(listen_fd,(struct sockaddr *) &clientAddr,sizeof(clientAddr));
	if (ret < 0) {
	printf("bind socket error %s errno: %d\n", strerror(errno), errno);
	}

	//监听
	ret = listen(listen_fd, 1024);
	if (ret < 0) {
	fprintf(stderr, "listen error %s errno: %d\n", strerror(errno), errno);
	}

	//监听到两个连接，创建对两个连接的处理线程
	int conn_index = 0;
	while(1) {
		conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
		if(conn_fd < 0) {
		    printf("accpet socket error: %s errno :%d\n", strerror(errno), errno);
		    continue;
		}
		else {
			char sendData[200] = "开始聊天";
			while(1) {
				int recv_len = recv(conn_fd, buff, strlen(sendData), 0);
				if (recv_len < 0) {
					printf("recv error %s errno: %d\n", strerror(errno), errno);
					continue;
				}
				else {
					buff[recv_len] = '\0';
					printf("recv message from client: %s\n", buff);
					send(conn_fd, sendData, strlen(sendData), 0);
					break;
				}
			}

			client_fd[conn_index] = conn_fd;
			conn_index++;
			if(conn_index == 2) {
				parameter* p1 = new parameter;
				p1->rec = client_fd[0];
				p1->send = client_fd[1];
				pthread_create(&thread1, NULL, recAndSend, (void*)p1);
				parameter* p2 = new parameter;
				p2->rec = client_fd[1];
				p2->send = client_fd[0];
				pthread_create(&thread2, NULL, recAndSend, (void*)p2);
				conn_index = 0;
			} 
			  
		}
	}
	close(listen_fd);
	listen_fd = -1;
	return 0;
}

//处理线程，收到一个客户端发送的信息后转发给另一个客户端
void* recAndSend(void* arg) {
 	struct parameter *para =(parameter*)arg; 
	int rec_fd = para->rec;
	int send_fd = para->send;
	char sendData[200];
	char buff[200];
	while(1) {
		int recv_len = recv(send_fd, buff, MAX_READ_LINE, 0);
		if (recv_len < 0) {
			printf("recv error %s errno: %d\n", strerror(errno), errno);
			continue;
		}
		else if (recv_len > 0) {
			send(rec_fd, buff, strlen(buff), 0);
		}
	}
	close(send_fd);
	send_fd = -1;
	return ((void*)0);
}
