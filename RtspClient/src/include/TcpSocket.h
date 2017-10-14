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
	TcpSocket();
	~TcpSocket();
	int Create(string i_URL);
	int Send(string *i_pMsg);
	int Recv(string *o_pMsg);
	void Close();
	
private:
	int GetIpAndPort(string i_URL,string *o_Ip,unsigned short *o_wPort);
	int m_iSocketFd;
};




#endif
