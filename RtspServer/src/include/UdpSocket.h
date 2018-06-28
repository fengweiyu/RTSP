/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	UdpSocket.h
* Description		: 	UdpSocket operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H


#include <map>
#include <stdio.h>
#include <string>

using std::map;
using std::string;

#define UDP_MTU     1500
/*****************************************************************************
-Class			: UdpSocket
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class UdpSocket
{
public:
    virtual int Init(string *i_pstrDstIP,unsigned short i_wDstPort,string *i_pstrIP,unsigned short i_wPort)=0;
    virtual int Send(char * i_acSendBuf,int i_iSendLen)=0;//一旦某个参数开始指定默认值,它右边的所有参数都必须指定默认值.
    virtual int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,timeval *i_ptTime=NULL)=0;//在调用具有默认参数的函数时, 若某个实参默认,其右边的所有实参都应该默认
    virtual void Close()=0;//即无论是定义还是调用的时候默认的都得放到后面，https://www.cnblogs.com/LubinLew/p/DefaultParameters.html  
};


/*****************************************************************************
-Class          : UdpServer
-Description    : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
class UdpServer : public UdpSocket
{
public:
    UdpServer();
    ~UdpServer();
    int Init(string *i_pstrClientIP,unsigned short i_wClientPort,string *i_pstrIP,unsigned short i_wPort);    
    int Send(char * i_acSendBuf,int i_iSendLen);
    int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,timeval *i_ptTime=NULL);
    void Close();
    
private:
    int             m_iServerSocketFd;
    string          m_strClientIP;//目的客户端IP，端口不会变，所以放在成员变量里
    unsigned short  m_wClientPort;
};


/*****************************************************************************
-Class          : UdpClient
-Description    : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
class UdpClient : public UdpSocket
{
public:
    UdpClient();
    ~UdpClient();
    int Init(string *i_pstrServerIP,unsigned short i_wServerPort,string *i_pstrIP=NULL,unsigned short i_wPort=0);
    int Send(char * i_acSendBuf,int i_iSendLen);
    int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,timeval *i_ptTime=NULL);
    void Close();
    int GetClientSocket();
private:
    int             m_iClientSocketFd;
    string          m_strServerIP;//目的服务器IP，端口不会变，所以放在成员变量里
    unsigned short  m_wServerPort;
};




#endif
