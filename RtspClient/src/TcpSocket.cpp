/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	TcpSocket.cpp
* Description		: 	TcpSocket operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "Definition.h"
#include "TcpSocket.h"

using std::cout;
using std::endl;
using std::string;


/*****************************************************************************
-Fuction		: TcpSocket
-Description	: TcpSocket
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
TcpSocket::TcpSocket()
{
	m_iSocketFd=-1;
}

/*****************************************************************************
-Fuction		: ~TcpSocket
-Description	: ~TcpSocket
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
TcpSocket::~TcpSocket()
{
}
/*****************************************************************************
-Fuction		: GetIpAndPort
-Description	: GetIpAndPort
-Input			: " rtsp://192.168.1.88:554/1"
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpSocket::GetIpAndPort(string i_URL,string *o_Ip,unsigned short *o_wPort)
{
	int iRet=FALSE;
	string IP("");
	unsigned int iIpStartPos;
	unsigned int iIpEndPos;
	unsigned int iPortEndPos;
	iIpStartPos=i_URL.find("//",0);
	if(iIpStartPos!=5)
	{	
		cout<<"GetIp err:"<<iIpStartPos<<endl;
	}
	else
	{
		iIpEndPos=i_URL.find(":",iIpStartPos+2);
		if(iIpEndPos==string::npos)
		{
			cout<<"GetIp1 err:"<<iIpEndPos<<endl;
		}
		else
		{
			iPortEndPos=i_URL.find("/",iIpEndPos+1);
			if(iPortEndPos==string::npos)
			{
				cout<<"GetPort err:"<<iPortEndPos<<endl;
			}
			else
			{
				//IP.assign(i_URL,iIpStartPos+2,iIpEndPos-iIpStartPos-2);//对象之间的拷贝是浅拷贝，容易造成释放同一个内存，
				//memcpy(o_Ip,&IP,sizeof(string));//这里就有double free or corruption (top)的错误
				o_Ip->assign(i_URL,iIpStartPos+2,iIpEndPos-iIpStartPos-2);				
				*o_wPort=atoi(i_URL.substr(iIpEndPos+1,iPortEndPos-iIpEndPos-1).c_str());
				cout<<"IP:"<<o_Ip->c_str()<<" Port:"<<i_URL.substr(iIpEndPos+1,iPortEndPos-iIpEndPos-1).c_str()<<endl;
				iRet=TRUE;
			}
		}
	}
	return iRet;
}

/*****************************************************************************
-Fuction		: Create
-Description	: Create
-Input			: 
-Output 		: 
-Return 		: 失败返回-1，成功返回0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpSocket::Create(string i_URL)
{
	int iRet=FALSE;
	int iSocketFd=-1;
	unsigned short wPort;
	string IP="";
	struct sockaddr_in tServerAddr;
	iSocketFd=socket(AF_INET,SOCK_STREAM,0);
	if(iSocketFd<0)
	{
		perror(NULL);
		cout<<"TcpSocketCreate err"<<endl;
	}
	else
	{
		// Set Sockfd NONBLOCK //暂时使用阻塞形式的
		//iSocketStatus=fcntl(iSocketFd, F_GETFL, 0);
		//fcntl(iSocketFd, F_SETFL, iSocketStatus | O_NONBLOCK);	
		
		// Connect to server
		this->GetIpAndPort(i_URL,&IP,&wPort);
		bzero(&tServerAddr, sizeof(tServerAddr));
		tServerAddr.sin_family = AF_INET;
		tServerAddr.sin_port = htons(wPort);
		tServerAddr.sin_addr.s_addr = inet_addr(IP.c_str());
		if(connect(iSocketFd, (struct sockaddr *)&tServerAddr, sizeof(tServerAddr)) < 0 && errno != EINPROGRESS) 
		{
			perror(NULL);
			cout<<"TcpSocket connect err"<<endl;
			close(iSocketFd);
			iSocketFd=-1;
		}
		else
		{
			//test
			m_iSocketFd=iSocketFd;
			iRet=TRUE;
		}
	}
	return iRet;
}

/*****************************************************************************
-Fuction		: Send
-Description	: 阻塞的操作形式
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpSocket::Send(string *i_pMsg)
{
	int iRet=FALSE;
	iRet=send(m_iSocketFd,i_pMsg->c_str(),i_pMsg->length(),0);
	if(iRet<0)
	{
		close(m_iSocketFd);
	}
	else
	{
		iRet=TRUE;
	}
	cout<<"Send :\r\n"<<i_pMsg->c_str()<<endl;
	return iRet;
}

/*****************************************************************************
-Fuction		: Recv
-Description	: 阻塞的操作形式
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpSocket::Recv(string *o_pMsg)
{
	int iRecvLen=FALSE;
	int iRet=FALSE;
	fd_set tReadFds;
	timeval tTimeValue;
	char acRecvBuf[1024];
	o_pMsg->assign("");//i_pMsg->clear();
	
	 FD_ZERO(&tReadFds); //清空描述符集合	
	 FD_SET(m_iSocketFd, &tReadFds); //设置描述符集合
	 tTimeValue.tv_sec  =1;//超时时间，超时返回错误
	 tTimeValue.tv_usec = 0;
	 while(1)
	 {
		 iRet = select(m_iSocketFd + 1, &tReadFds, NULL, NULL, &tTimeValue);//调用select（）监控函数//NULL 一直等到有变化
		 if(iRet<0)  
		 {
			 perror("select Recv err\n");  
			 close(m_iSocketFd);	
			 break;
		 }
		 else
		 {
		 }
		 if (FD_ISSET(m_iSocketFd, &tReadFds))   //测试fd1是否可读  
		 {
			 memset(acRecvBuf,0,1024);	
			iRecvLen=recv(m_iSocketFd,acRecvBuf,sizeof(acRecvBuf),0);  
			if(iRecvLen<=0)
			{
				break;
			}
			else
			{
				o_pMsg->append(acRecvBuf,iRecvLen);
				iRet=TRUE;
			}
		 }
		 else
		 {
		 	break;
		 }
	 }
	if(o_pMsg->length()>0)
	{
		cout<<"Recv :\r\n"<<o_pMsg->c_str()<<endl;
		iRet=TRUE;
	}
	else
	{
		cout<<"Recv err:"<<iRet<<endl;
		iRet=FALSE;
	}
	return iRet;
}


/*****************************************************************************
-Fuction		: Close
-Description	: Close
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void TcpSocket::Close()
{
	if(m_iSocketFd!=-1)
	{
		close(m_iSocketFd);
		m_iSocketFd=-1;
	}
	else
	{
		cout<<"Close err:"<<m_iSocketFd<<endl;
	}
}

