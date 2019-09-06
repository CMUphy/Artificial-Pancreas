//
//  main.cpp
//  TCPClient
//
//  Created by shenyuanluo on 2018/2/12.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include <iostream>
#include "TCPClient.hpp"
#include <stdio.h>
#include<string.h>
#include <cstdlib>
#define datalen 30
#define DEMO_PORT 5578
std::string DEMO_IP ="127.0.0.1";
using namespace std;
//
//  TCPClient.cpp
//  TCPClient
//
//  Created by shenyuanluo on 2018/2/12.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#include "TCPClient.hpp"
#include <pthread.h>




int main()
{	
    struct timespec outtime;	
    double recvdata[BUFF_SIZE];
		char recvchar[BUFF_SIZE];
		ssize_t recvLen;
        string sendMsg;
	struct sockaddr_in serverAddr;      
    socklen_t serAddrLen;               
    int serverSockfd;                   
   int clientSockfd = socket(PF_INET, SOCK_STREAM, 0);
if (0 > clientSockfd)
    {
        std::cout << "create socket failed！" << std::endl;
        return 1;
    }
    std::cout << "create socket succeed！" << std::endl;
    
    
    serAddrLen = sizeof(serverAddr);
    memset(&serverAddr, 0, serAddrLen);                     
    serverAddr.sin_family      = AF_INET;                   
    serverAddr.sin_port        = htons(DEMO_PORT);               
    serverAddr.sin_addr.s_addr = inet_addr(DEMO_IP.c_str());  


int ret = connect(clientSockfd, (struct sockaddr*)&serverAddr, serAddrLen);
	if (0 != ret)
	{
		std::cout << "Connection failed！" << std::endl;
		return Ret_failed;
	}
std::cout << "Connection Succeed!" << std::endl;


while(1)
    { 

		std::cin >> sendMsg;
        ssize_t sendLen = send(clientSockfd, sendMsg.c_str(), sendMsg.length(), 0);
        if (0 > sendLen)    
        {
            std::cout << "Send Error！" << std::endl;
            break;
        }
        else if (0 == sendLen) 
        {
            std::cout << "Send failed,disconnection！" << std::endl;
            break;
        }
        else   
        {
            if (0 == strcmp(sendMsg.c_str(), "quit"))
            {
                break;
            }
//            std::cout << "发送消息成功！" << std::endl;
        }
  
        memset(recvdata, 0, BUFF_SIZE);
        memset(recvchar, 0, BUFF_SIZE);
		
        if(0 == strcmp(sendMsg.c_str(), "Get_Data"))
		{ recvLen = recv(clientSockfd, recvdata, BUFF_SIZE, 0);
        if(recvLen>0)
        {  
        std::cout<<"Name:XXX       "<<"Date: 2019-9-3   "<<std::endl;
        std::cout<<"Glucose:"<<std::endl;
           for (int j=0;j<datalen-1;j++)
					 {
						std::cout<< recvdata[j]<<std::endl;
					 }
                     continue;
        }
        }
		if(0 == strcmp(sendMsg.c_str(), "Change_PID"))
		{
			recvLen = recv(clientSockfd, recvchar, BUFF_SIZE, 0);
			cout<<recvchar<<endl;
			double parament[3];
			for(int i=0;i<3;i++){cin>>parament[i];}
			ssize_t sendLen = send(clientSockfd, parament, BUFF_SIZE, 0);
           recv(clientSockfd, recvchar, BUFF_SIZE, 0);
           cout<<recvchar<<endl;

		}
		if(0 == strcmp(sendMsg.c_str(), "Change_setpoint"))
		{
			recvLen = recv(clientSockfd, recvchar, BUFF_SIZE, 0);
			cout<<recvchar<<endl;
			double parament[1];
			for(int i=0;i<1;i++){cin>>parament[i];}
			ssize_t sendLen = send(clientSockfd, parament, BUFF_SIZE, 0);
			if(parament[0]==1)
			{
			recvLen = recv(clientSockfd, recvchar, BUFF_SIZE, 0);
			cout<<recvchar<<endl;
			double parament1[1];
			for(int i=0;i<1;i++){cin>>parament1[i];}
			send(clientSockfd, parament1, BUFF_SIZE, 0);
			 recv(clientSockfd, recvchar, BUFF_SIZE, 0);
           cout<<recvchar<<endl;
			}
           if(parament[0]==2)
			{
			recvLen = recv(clientSockfd, recvchar, BUFF_SIZE, 0);
			cout<<recvchar<<endl;
			double parament1[2];
			for(int i=0;i<2;i++){cin>>parament1[i];}
			send(clientSockfd, parament1, BUFF_SIZE, 0);
			 recv(clientSockfd, recvchar, BUFF_SIZE, 0);
           cout<<recvchar<<endl;
			}

		}
            if (0 == strcmp(sendMsg.c_str(), "quit"))   // 遇到 “quit” 则退出
            {
                break;
            }
        
    }


    return 0;
}
