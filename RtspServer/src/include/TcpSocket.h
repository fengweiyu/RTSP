/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	TcpSocket.h
* Description		: 	TcpSocket operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H


#include <map>
#include <stdio.h>
#include <string>

using std::map;
using std::string;

/*****************************************************************************
-Class			: TcpSocket
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class TcpSocket
{
public:
	virtual int Init(string i_strIP,unsigned short i_wPort)=0;//由于string是深拷贝,不是默认的只拷贝值的那种浅拷贝,所以可以不用指针,可以直接用或用引用均可
	virtual int Send(char * i_acSendBuf,int i_iSendLen,int i_iSocketFd)=0;
	virtual int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,int i_iSocketFd,timeval *i_ptTime=NULL)=0;
	virtual void Close(int i_iSocketFd)=0;	
};


/*****************************************************************************
-Class			: TcpServer
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class TcpServer : public TcpSocket
{
public:
	TcpServer();
	~TcpServer();
	int Init(string i_strIP,unsigned short i_wPort);	
    int Accept();
	int Send(char * i_acSendBuf,int i_iSendLen,int i_iClientSocketFd);
	int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,int i_iClientSocketFd,timeval *i_ptTime=NULL);
	void Close(int i_iClientSocketFd);
	
private:
	int  m_iServerSocketFd;
};


/*****************************************************************************
-Class			: TcpClient
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class TcpClient : public TcpSocket
{
public:
	TcpClient();
	~TcpClient();
	int Init(string i_strIP,unsigned short i_wPort);
	int Send(char * i_acSendBuf,int i_iSendLen,int i_iClientSocketFd=0);
	int Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,int i_iClientSocketFd=0,timeval *i_ptTime=NULL);
	void Close(int i_iClientSocketFd=0);
    int GetClientSocket();
private:
	int  m_iClientSocketFd;
};


#endif
