//
//  TCPClient.hpp
//  TCPClient
//
//  Created by shenyuanluo on 2018/2/12.
//  Copyright © 2018年 http://blog.shenyuanluo.com/ All rights reserved.
//

#ifndef TCPClient_hpp
#define TCPClient_hpp

#include <iostream>     // cout
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // close


#define BUFF_SIZE 1024


/** 返回值 枚举 */
typedef enum __retValue {
    Ret_error               = -2,       // 出错
    Ret_failed              = -1,       // 失败
    Ret_success             = 0,        // 成功
}RetValue;


void* SendMsg(void *arg);               /** 发送消息 */
void* RecvMsg(void *arg);               /** 接收消息 */


class TCPClient
{
private:
    struct sockaddr_in serverAddr;      // 服务端地址
    socklen_t serAddrLen;               // 地址长度
    int clientSockfd;                   // 客户端 socket
    
    /**
     发送消息
     
     @param arg 线程参数（TCPClient实例指针）
     */
    friend void* SendMsg(void* arg);
    
    /**
     接收消息
     
     @param arg 线程参数（TCPClient实例指针）
     */
    friend void* RecvMsg(void* arg);
    
    
public:
    TCPClient(std::string ipStr, unsigned int port);
    ~TCPClient();
    
    /**
     连接 服务端

     @return 参见‘RetValue’
     */
    RetValue ConnServer();
    
    /**
     运行 客户端

     @return 参见‘RetValue’
     */
    RetValue Run();
    
};

#endif /* TCPClient_hpp */
