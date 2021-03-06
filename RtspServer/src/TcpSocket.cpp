/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	TcpSocket.cpp
* Description		: 	TcpSocket  server operation center
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
#include <unistd.h>
#include <netinet/tcp.h> 

#include "Definition.h"
#include "TcpSocket.h"

using std::cout;
using std::endl;
using std::string;


/*****************************************************************************
-Fuction		: TcpServer
-Description	: TcpServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
TcpServer::TcpServer()
{
    m_iServerSocketFd = -1;
}

/*****************************************************************************
-Fuction		: ~TcpServer
-Description	: ~TcpServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
TcpServer::~TcpServer()
{
}

/*****************************************************************************
-Fuction		: Init
-Description	: Init
-Input			: 
-Output 		: 
-Return 		: 失败返回-1，成功返回0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpServer::Init(string i_strIP,unsigned short i_wPort)
{
	int iRet=FALSE;
	int iSocketFd=-1;
	unsigned short wPort=i_wPort;
	string IP(i_strIP.c_str());
	struct sockaddr_in tServerAddr;

	if(m_iServerSocketFd !=-1)
	{
	    iRet=TRUE;
	}
	else
	{
        iSocketFd=socket(AF_INET,SOCK_STREAM,0);
        if(iSocketFd<0)
        {
            perror(NULL);
            cout<<"TcpSocketInit err"<<endl;
        }
        else
        {
            // Set Sockfd NONBLOCK //暂时使用阻塞形式的
            //iSocketStatus=fcntl(iSocketFd, F_GETFL, 0);
            //fcntl(iSocketFd, F_SETFL, iSocketStatus | O_NONBLOCK);    
            int tmp = 1;
            if (setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp)) < 0) 
            {
                cout<<"TcpSocket setsockopt err"<<endl;
                close(iSocketFd);
                iSocketFd=-1;
                return iRet;
            }
            int chOpt = 1;
            if (setsockopt(iSocketFd, IPPROTO_TCP, TCP_NODELAY, (char *)&chOpt, sizeof(chOpt)) < 0) 
            {
                perror(NULL);
                cout<<"TcpSocket setsockopt TCP_NODELAY err"<<endl;
                close(iSocketFd);
                iSocketFd=-1;
                return iRet;
            }
            // Connect to server
            //this->GetIpAndPort(i_URL,&IP,&wPort);
            bzero(&tServerAddr, sizeof(tServerAddr));
            tServerAddr.sin_family = AF_INET;
            tServerAddr.sin_port = htons(wPort);//一般是554
            tServerAddr.sin_addr.s_addr = inet_addr(IP.c_str());//也可以使用htonl(INADDR_ANY),表示使用本机的所有IP
            if(bind(iSocketFd,(struct sockaddr*)&tServerAddr,sizeof(tServerAddr))<0)
            {
                perror(NULL);
                cout<<"TcpSocket bind err"<<endl;
                close(iSocketFd);
                iSocketFd=-1;
            }
            else
            {
                //当前服务器ip和端口号最大允许连接的客户端个数为100
                if(listen(iSocketFd,100)<0) //等待连接个数,也就是允许连接的客户端个数100
                {
                    perror(NULL);
                    cout<<"TcpSocket listen err"<<endl;
                    close(iSocketFd);
                    iSocketFd=-1;
                }
                else
                {
                    m_iServerSocketFd=iSocketFd;
                    iRet=TRUE;
                }
            }
	    }
    }
	return iRet;
}

/*****************************************************************************
-Fuction		: Accept
-Description	: 阻塞的操作形式
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpServer::Accept()
{
    int iClientSocketFd=-1;
    struct sockaddr_un tClientAddr; 
    socklen_t iLen=0;
    //have connect request use accept  
    iLen=sizeof(tClientAddr);  
    iClientSocketFd=accept(m_iServerSocketFd,(struct sockaddr*)&tClientAddr,&iLen);  //这里会等待客户端连接
    if(iClientSocketFd<0)  
    {  
        perror("cannot accept client connect request");  
        close(m_iServerSocketFd);  
        //unlink(UNIX_DOMAIN);  //错误： ‘UNIX_DOMAIN’在此作用域中尚未声明
    } 
	else
	{
	}
	return iClientSocketFd;
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
int TcpServer::Send(char * i_acSendBuf,int i_iSendLen,int i_iClientSocketFd)
{
	int iRet=FALSE;
	if(i_acSendBuf==NULL ||i_iSendLen<=0)
	{
        cout<<"Send err"<<endl;
	}
	else
	{
        iRet=send(i_iClientSocketFd,i_acSendBuf,i_iSendLen,0);
        if(iRet<0)
        {
            close(i_iClientSocketFd);
        }
        else
        {
            iRet=TRUE;
        }
        string strSend(i_acSendBuf);
        cout<<"Send :\r\n"<<strSend<<endl;
	}

	return iRet;
}

/*****************************************************************************
-Fuction		: Recv
-Description	: 阻塞的操作形式
-Input			: 
-Output 		: o_piRecvLen 返回小于要读的数据，则表示读完了，默认超时1s
-Return 		: 
iRet=FALSE;表示套接字出错，
iRet = TRUE; 表示没有出错，包括超时
o_piRecvLen 表示接收到的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpServer::Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,int i_iClientSocketFd,timeval *i_ptTime)
{
    int iRecvLen=FALSE;
    int iRet=FALSE;
    fd_set tReadFds;
    timeval tTimeValue;
    char *pcRecvBuf=o_acRecvBuf;
    int iLeftRecvLen=i_iRecvBufMaxLen;

    if(NULL == o_acRecvBuf ||NULL == o_piRecvLen ||i_iRecvBufMaxLen <= 0)
    {
        perror("TcpServer::Recv NULL");
        return iRet;
    }   
    memset(o_acRecvBuf,0,i_iRecvBufMaxLen);;
    while(iLeftRecvLen > 0)
    {
        FD_ZERO(&tReadFds); //清空描述符集合    
        FD_SET(i_iClientSocketFd, &tReadFds); //设置描述符集合
        tTimeValue.tv_sec      = 1;//超时时间，超时返回错误
        tTimeValue.tv_usec     = 0;
        if(NULL != i_ptTime)
            memcpy(&tTimeValue,i_ptTime,sizeof(timeval));
        iRet = select(i_iClientSocketFd + 1, &tReadFds, NULL, NULL, &tTimeValue);//调用select（）监控函数//NULL 一直等到有变化
        if(iRet<0)  
        {
            perror("select Recv err\n");  
            close(i_iClientSocketFd);
            iRet=FALSE;
            break;
        }
        else if(0 == iRet)
        {
            //perror("select Recv timeout\r\n");
            iRet = TRUE;
            break;
        }
        else
        {
        }
        if (FD_ISSET(i_iClientSocketFd, &tReadFds))   //测试fd1是否可读  
        {
            iRecvLen=recv(i_iClientSocketFd,pcRecvBuf,iLeftRecvLen,0);  
            if(iRecvLen<=0)
            {
                if(errno != EINTR)
                {
                    printf("errno Recv err%d\r\n",iRecvLen); 
                    perror("errno"); 
                    iRet=FALSE;
                    break;
                }
            }
            else
            {
                iLeftRecvLen = iLeftRecvLen-iRecvLen;
                pcRecvBuf += iRecvLen;
                iRet = TRUE;
            }
        }
        else
        {
            perror("errno FD_ISSET err"); 
            iRet=FALSE;
        	break;
        }
    }
    if(iLeftRecvLen < i_iRecvBufMaxLen)
    {
        string strRecv(o_acRecvBuf);
        *o_piRecvLen = i_iRecvBufMaxLen - iLeftRecvLen;
        cout<<"SvcRecv :\r\n"<<strRecv<<endl;
    }
    else
    {
        //cout<<"Recv err:"<<iRecvAllLen<<endl;
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
void TcpServer::Close(int i_iClientSocketFd)
{
	if(i_iClientSocketFd!=-1)
	{
		close(i_iClientSocketFd);
	}
	else
	{
		cout<<"Close err:"<<i_iClientSocketFd<<endl;
	}
}

/*****************************************************************************
-Fuction		: TcpClient
-Description	: TcpClient
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
TcpClient ::TcpClient()
{
    m_iClientSocketFd = -1;
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
TcpClient ::~TcpClient()
{

}

/*****************************************************************************
-Fuction		: Init
-Description	: Init
-Input			: 
-Output 		: 
-Return 		: 失败返回-1，成功返回0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpClient::Init(string i_strIP,unsigned short i_wPort)
{
	int iRet=FALSE;
	int iSocketFd=-1;
	struct sockaddr_in tServerAddr;
	iSocketFd=socket(AF_INET,SOCK_STREAM,0);
	if(iSocketFd<0)
	{
		perror(NULL);
		cout<<"TcpSocketInit err"<<endl;
	}
	else
	{
		// Set Sockfd NONBLOCK //暂时使用阻塞形式的
		//iSocketStatus=fcntl(iSocketFd, F_GETFL, 0);
		//fcntl(iSocketFd, F_SETFL, iSocketStatus | O_NONBLOCK);	
        int tmp = 1;
        if (setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp)) < 0) 
        {
            cout<<"TcpClient setsockopt err"<<endl;
            close(iSocketFd);
            iSocketFd=-1;
            return iRet;
        }
		// Connect to server
		bzero(&tServerAddr, sizeof(tServerAddr));
		tServerAddr.sin_family = AF_INET;
		tServerAddr.sin_port = htons(i_wPort);
		tServerAddr.sin_addr.s_addr = inet_addr(i_strIP.c_str());
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
			m_iClientSocketFd=iSocketFd;
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
int TcpClient::Send(char * i_acSendBuf,int i_iSendLen,int i_iClientSocketFd)
{
	int iRet=FALSE;
	if(i_acSendBuf==NULL ||i_iSendLen<=0)
	{
        cout<<"Send err"<<endl;
	}
	else
	{
        iRet=send(m_iClientSocketFd,i_acSendBuf,i_iSendLen,0);
        if(iRet<0)
        {
            close(m_iClientSocketFd);
        }
        else
        {
            iRet=TRUE;
        }
        string strSend(i_acSendBuf);
        cout<<"Send :\r\n"<<strSend<<endl;
	}
	
	return iRet;
}

/*****************************************************************************
-Fuction		: Recv
-Description	: 阻塞的操作形式
-Input			: 
-Output 		: o_piRecvLen 返回小于要读的数据，则表示读完了，默认超时1s
-Return 		: 
iRet=FALSE;表示套接字出错，
iRet = TRUE; 表示没有出错，包括超时
o_piRecvLen 表示接收到的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int TcpClient::Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,int i_iClientSocketFd,timeval *i_ptTime)
{
    int iRecvLen=FALSE;
    int iRet=FALSE;
    fd_set tReadFds;
    timeval tTimeValue;
    char *pcRecvBuf=o_acRecvBuf;
    int iLeftRecvLen=i_iRecvBufMaxLen;

    if(NULL == o_acRecvBuf ||NULL == o_piRecvLen ||i_iRecvBufMaxLen <= 0)
    {
        perror("TcpClient::Recv NULL");
        return iRet;
    }   
    while(iLeftRecvLen > 0)
    {
        FD_ZERO(&tReadFds); //清空描述符集合    
        FD_SET(m_iClientSocketFd, &tReadFds); //设置描述符集合
        tTimeValue.tv_sec  =1;//超时时间，超时返回错误
        tTimeValue.tv_usec = 0;
        if(NULL != i_ptTime)
            memcpy(&tTimeValue,i_ptTime,sizeof(timeval));
        iRet = select(m_iClientSocketFd + 1, &tReadFds, NULL, NULL, &tTimeValue);//调用select（）监控函数//NULL 一直等到有变化
        if(iRet<0)  
        {
            perror("select Recv err\n");  
            close(m_iClientSocketFd);
            iRet=FALSE;
            break;
        }
        else if(0 == iRet)
        {
            //perror("select Recv timeout\r\n");
            iRet = TRUE;
            break;
        }
        else
        {
        }
        if (FD_ISSET(m_iClientSocketFd, &tReadFds))   //测试fd1是否可读  
        {
            iRecvLen=recv(i_iClientSocketFd,pcRecvBuf,iLeftRecvLen,0);  
            if(iRecvLen<=0)
            {
                if(errno != EINTR)
                {
                    iRet=FALSE;
                    break;
                }
            }
            else
            {
                iLeftRecvLen = iLeftRecvLen-iRecvLen;
                pcRecvBuf += iRecvLen;
                iRet = TRUE;
            }
        }
        else
        {
            iRet=FALSE;
        	break;
        }
    }
    if(iLeftRecvLen < i_iRecvBufMaxLen)
    {
        string strRecv(o_acRecvBuf);
        *o_piRecvLen = i_iRecvBufMaxLen - iLeftRecvLen;
        cout<<"Recv :\r\n"<<strRecv<<endl;
    }
    else
    {
        //cout<<"Recv err:"<<iRecvAllLen<<endl;
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
void TcpClient::Close(int i_iClientSocketFd)
{
	if(m_iClientSocketFd!=-1)
	{
		close(m_iClientSocketFd);
		m_iClientSocketFd=-1;
	}
	else
	{
		cout<<"Close err:"<<m_iClientSocketFd<<endl;
	}
}

/*****************************************************************************
-Fuction        : GetClientSocket
-Description    : GetClientSocket
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int TcpClient::GetClientSocket()
{
    return m_iClientSocketFd;
}

