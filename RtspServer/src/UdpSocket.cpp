/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	UdpSocket.cpp
* Description		: 	UdpSocket operation center
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
#include "UdpSocket.h"

using std::cout;
using std::endl;
using std::string;


/*****************************************************************************
-Fuction		: UdpServer
-Description	: UdpServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
UdpServer::UdpServer()
{
    m_iServerSocketFd = -1;
    m_strClientIP.clear();
    m_wClientPort=0;
}

/*****************************************************************************
-Fuction        : ~UdpServer
-Description    : ~UdpServer
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
UdpServer::~UdpServer()
{
}

/*****************************************************************************
-Fuction        : Init
-Description    : Init
-Input          : 
-Output         : 
-Return         : 失败返回-1，成功返回0
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpServer::Init(string *i_pstrClientIP,unsigned short i_wClientPort,string *i_pstrIP,unsigned short i_wPort)
{
    int iRet=FALSE;
    int iSocketFd=-1;
    unsigned short wPort=i_wPort;
    struct sockaddr_in tServerAddr;

    if(m_iServerSocketFd !=-1)
    {
        iRet=TRUE;
    }
    else
    {
        iSocketFd=socket(AF_INET,SOCK_DGRAM,0);
        if(iSocketFd<0)
        {
            perror(NULL);
            cout<<"UdpSocketInit err"<<endl;
        }
        else
        {
            // Set Sockfd NONBLOCK //暂时使用阻塞形式的
            //iSocketStatus=fcntl(iSocketFd, F_GETFL, 0);
            //fcntl(iSocketFd, F_SETFL, iSocketStatus | O_NONBLOCK);    
            
            // Connect to server
            //this->GetIpAndPort(i_URL,&IP,&wPort);
            bzero(&tServerAddr, sizeof(tServerAddr));
            tServerAddr.sin_family = AF_INET;
            tServerAddr.sin_port = htons(wPort);//
            if(NULL == i_pstrIP)
                tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);//也可以使用htonl(INADDR_ANY),表示使用本机的所有IP
            else
                tServerAddr.sin_addr.s_addr = inet_addr(i_pstrIP->c_str());//也可以使用htonl(INADDR_ANY),表示使用本机的所有IP
            if(bind(iSocketFd,(struct sockaddr*)&tServerAddr,sizeof(tServerAddr))<0)
            {
                perror(NULL);
                cout<<"UdpSocket bind err"<<endl;
                close(iSocketFd);
                iSocketFd=-1;
            }
            else
            {
                m_iServerSocketFd=iSocketFd;
                if(i_pstrClientIP!=NULL && i_wClientPort!=0)
                {
                    m_strClientIP.assign(i_pstrClientIP->c_str());
                    m_wClientPort=i_wClientPort;
                }
                iRet=TRUE;
            }
        }
    }
    return iRet;
}

/*****************************************************************************
-Fuction        : Send
-Description    : 阻塞的操作形式
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpServer::Send(char * i_acSendBuf,int i_iSendLen)
{
    int iRet=FALSE;
    struct sockaddr_in tClientAddr;
    char * acSendBuf=i_acSendBuf;
    int iSendLen=i_iSendLen;
    if(i_acSendBuf==NULL ||i_iSendLen<=0||m_wClientPort==0||m_strClientIP.length()<=0)
    {
        cout<<"Send err"<<m_wClientPort<<endl;
    }
    else
    {
        bzero(&tClientAddr, sizeof(tClientAddr));
        tClientAddr.sin_family = AF_INET;
        tClientAddr.sin_port = htons(m_wClientPort);//
        tClientAddr.sin_addr.s_addr = inet_addr(m_strClientIP.c_str());
        while(1)
        {
            iRet=sendto(m_iServerSocketFd,acSendBuf,iSendLen,0,(struct sockaddr*)&tClientAddr,sizeof(tClientAddr));
            if(iRet<0)
            {
                close(m_iServerSocketFd);
                cout<<"sendto err"<<iRet<<endl;
                break;
            }
            else
            {
                if(iRet<iSendLen)
                {
                    acSendBuf+=iRet;
                    iSendLen-=iRet;
                }
                else
                {
                    iRet=TRUE;
                    break;
                }
            }
        }
        string strSend(i_acSendBuf);
        cout<<"Send :\r\n"<<strSend<<"iRet:"<<iRet<<endl;
    }

    return iRet;
}

/*****************************************************************************
-Fuction        : Recv
-Description    : 阻塞的操作形式
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpServer::Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,timeval *i_ptTime)
{
    int iRet=FALSE;
    fd_set tReadFds;
    timeval tTimeValue;
    char acRecvBuf[UDP_MTU];
    char *pcRecvBuf=o_acRecvBuf;
    //int iRecvAllLen=0;
    struct sockaddr_in tClientAddr;
    struct sockaddr_in tRecvClientAddr;
    socklen_t iSockaddrLen=sizeof(struct sockaddr_in);
    int iRecvLen=0;
    
    memset(o_acRecvBuf,0,i_iRecvBufMaxLen);;
    FD_ZERO(&tReadFds); //清空描述符集合    
    FD_SET(m_iServerSocketFd, &tReadFds); //设置描述符集合
    tTimeValue.tv_sec      = 1;//超时时间，超时返回错误
    tTimeValue.tv_usec     = 0;
    if(NULL != i_ptTime)
        memcpy(&tTimeValue,i_ptTime,sizeof(timeval));

        
    //while(1)//udp是面向数据报的，没有粘包,不需要一直读
    {
        iRet = select(m_iServerSocketFd + 1, &tReadFds, NULL, NULL, &tTimeValue);//调用select（）监控函数//NULL 一直等到有变化
        if(iRet<0)  
        {
            perror("select Recv err\n");  
            //close(m_iServerSocketFd);   
            //break;
        }
        else
        {
        }
        if (FD_ISSET(m_iServerSocketFd, &tReadFds))   //测试fd1是否可读  
        {
            memset(acRecvBuf,0,UDP_MTU);//不需要组包，udp就是一个个数据包的
            iRecvLen=recvfrom(m_iServerSocketFd,acRecvBuf,sizeof(acRecvBuf),0,(struct sockaddr*)&tRecvClientAddr,&iSockaddrLen);
            if(iRecvLen<0)
            {
                perror("recvfrom Recv err\n");  
                //close(m_iServerSocketFd);   
                //break;
            }
            else
            {
                if(iRecvLen>i_iRecvBufMaxLen)
                {
                    cout<<"Recv err,RecvLen:"<<iRecvLen<<" MaxLen:"<<i_iRecvBufMaxLen<<endl;                    
                    iRet=FALSE;
                    //break;
                }
                else
                {
                    memcpy(pcRecvBuf,acRecvBuf,iRecvLen);
                    pcRecvBuf+=iRecvLen;
                    iRet=TRUE;
                    //break;
                }
            }
        }
        else
        {
            //break;
        }
    }
    
    if(iRecvLen>0 && iRet==TRUE)
    {
        if(m_strClientIP.length()>0 && m_wClientPort!=0)
        {
            bzero(&tClientAddr, sizeof(tClientAddr));
            tClientAddr.sin_family = AF_INET;
            tClientAddr.sin_port = htons(m_wClientPort);//
            tClientAddr.sin_addr.s_addr = inet_addr(m_strClientIP.c_str());
            if(0!=memcmp(&tClientAddr,&tRecvClientAddr,sizeof(struct sockaddr_in)))
            {//判断数据来源
                string strRecv(inet_ntoa(tRecvClientAddr.sin_addr));
                cout<<"Recv data from err IP:\r\n"<<strRecv<<" Port:"<<ntohs(tRecvClientAddr.sin_port)<<endl;
                iRet=FALSE;
            }
            else
            {
                string strRecv(o_acRecvBuf);
                *o_piRecvLen=iRecvLen;
                cout<<"Recv :\r\n"<<strRecv<<endl;
                iRet=TRUE;
            }
        }
        else
        {
            string strRecv(o_acRecvBuf);
            *o_piRecvLen=iRecvLen;
            //cout<<"Recv :\r\n"<<strRecv<<endl;
            iRet=TRUE;
        }
    }
    else
    {
        //cout<<"Recv err:"<<iRecvLen<<endl;
        iRet=FALSE;
    }
    return iRet;
}



/*****************************************************************************
-Fuction        : Close
-Description    : Close
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
void UdpServer::Close()
{
    if(m_iServerSocketFd!=-1)
    {
        close(m_iServerSocketFd);
    }
    else
    {
        cout<<"Close err:"<<m_iServerSocketFd<<endl;
    }
}

/*****************************************************************************
-Fuction        : UdpClient
-Description    : UdpClient
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
UdpClient::UdpClient()
{
    m_iClientSocketFd = -1;
    m_strServerIP.clear();
    m_wServerPort=0;
}

/*****************************************************************************
-Fuction        : ~UdpSocket
-Description    : ~UdpSocket
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
UdpClient::~UdpClient()
{

}

/*****************************************************************************
-Fuction        : Init
-Description    : 绑定端口确保是连续的rtp rtcp
-Input          : 
-Output         : 
-Return         : 失败返回-1，成功返回0
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpClient::Init(string *i_pstrServerIP,unsigned short i_wServerPort,string *i_pstrIP,unsigned short i_wPort)
{
    int iRet=FALSE;
    int iSocketFd=-1;
	struct sockaddr_in tServerAddr;
    struct sockaddr_in tClientAddr;
    iSocketFd=socket(AF_INET,SOCK_DGRAM,0);
    if(iSocketFd<0)
    {
        perror(NULL);
        cout<<"UdpSocketInit err"<<endl;
    }
    else
    {
        // Set Sockfd NONBLOCK //暂时使用阻塞形式的
        //iSocketStatus=fcntl(iSocketFd, F_GETFL, 0);
        //fcntl(iSocketFd, F_SETFL, iSocketStatus | O_NONBLOCK);    
        if(NULL != i_pstrIP && 0!=i_wPort)
        {//绑定端口确保端口是连续的rtp rtcp
            bzero(&tClientAddr, sizeof(tClientAddr));
            tClientAddr.sin_family = AF_INET;
            tClientAddr.sin_port = htons(i_wPort);//
            tClientAddr.sin_addr.s_addr = inet_addr(i_pstrIP->c_str());//也可以使用htonl(INADDR_ANY),表示使用本机的所有IP
            if(bind(iSocketFd,(struct sockaddr*)&tClientAddr,sizeof(tClientAddr))<0)
            {
                perror(NULL);
                cout<<"UdpClient bind err"<<endl;
                close(iSocketFd);
                iSocketFd=-1;
            }
        }
        
		bzero(&tServerAddr, sizeof(tServerAddr));
		tServerAddr.sin_family = AF_INET;
		tServerAddr.sin_port = htons(i_wPort);
		tServerAddr.sin_addr.s_addr = inet_addr(i_pstrServerIP->c_str());
		if(connect(iSocketFd, (struct sockaddr *)&tServerAddr, sizeof(tServerAddr)) < 0 && errno != EINPROGRESS) 
		{//使用connect的目的是Init后就可以获取到本地连接的端口，否则只能sendto之后才能获取
			perror(NULL);
			cout<<"UdpClient connect err"<<endl;
			close(iSocketFd);
			iSocketFd=-1;
		}
		else
		{
            m_strServerIP.assign(i_pstrServerIP->c_str());
            m_wServerPort=i_wServerPort;
			m_iClientSocketFd=iSocketFd;
			iRet=TRUE;
		}
    }
    return iRet;


}

/*****************************************************************************
-Fuction        : Send
-Description    : 阻塞的操作形式
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpClient::Send(char * i_acSendBuf,int i_iSendLen)
{
    int iRet=FALSE;
    
    struct sockaddr_in tServerAddr;
    char * acSendBuf=i_acSendBuf;
    int iSendLen=i_iSendLen;
    if(i_acSendBuf==NULL ||i_iSendLen<=0)
    {
        cout<<"Send err"<<endl;
    }
    else
    {
        bzero(&tServerAddr, sizeof(tServerAddr));
        tServerAddr.sin_family = AF_INET;
        tServerAddr.sin_port = htons(m_wServerPort);//
        tServerAddr.sin_addr.s_addr = inet_addr(m_strServerIP.c_str());
        while(1)
        {
            iRet=sendto(m_iClientSocketFd,acSendBuf,iSendLen,0,(struct sockaddr*)&tServerAddr,sizeof(tServerAddr));
            if(iRet<0)
            {
                close(m_iClientSocketFd);
                cout<<"sendto err"<<iRet<<endl;
                break;
            }
            else
            {
                if(iRet<iSendLen)
                {
                    acSendBuf+=iRet;
                    iSendLen-=iRet;
                }
                else
                {
                    iRet=TRUE;
                    break;
                }
            }
        }
        //string strSend(i_acSendBuf);
        cout<<"UdpClient SendLen:"<<i_iSendLen<<" iRet:"<<iRet<<endl;
    }
    
    return iRet;
}

/*****************************************************************************
-Fuction        : Recv
-Description    : 阻塞的操作形式
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int UdpClient::Recv(char *o_acRecvBuf,int *o_piRecvLen,int i_iRecvBufMaxLen,timeval *i_ptTime)
{
    int iRet=FALSE;
    fd_set tReadFds;
    timeval tTimeValue;
    char acRecvBuf[UDP_MTU];
    char *pcRecvBuf=o_acRecvBuf;
    //int iRecvAllLen=0;
    struct sockaddr_in tServerAddr;
    struct sockaddr_in tRecvServerAddr;
    socklen_t iSockaddrLen=sizeof(struct sockaddr_in);
    int iRecvLen=0;
    
    memset(o_acRecvBuf,0,i_iRecvBufMaxLen);;
    FD_ZERO(&tReadFds); //清空描述符集合    
    FD_SET(m_iClientSocketFd, &tReadFds); //设置描述符集合
    tTimeValue.tv_sec      = 1;//超时时间，超时返回错误
    tTimeValue.tv_usec     = 0;
    if(NULL != i_ptTime)
        memcpy(&tTimeValue,i_ptTime,sizeof(timeval));
        
    //while(1)//udp是面向数据报的，没有粘包,不需要一直读
    {
        iRet = select(m_iClientSocketFd + 1, &tReadFds, NULL, NULL, &tTimeValue);//调用select（）监控函数//NULL 一直等到有变化
        if(iRet<0)  
        {
            perror("select Recv err\n");  
            //close(m_iClientSocketFd);   
            //break;
        }
        else
        {
        }
        if (FD_ISSET(m_iClientSocketFd, &tReadFds))   //测试fd1是否可读  
        {
            memset(acRecvBuf,0,UDP_MTU);//不需要组包，udp就是一个个数据包的
            iRecvLen=recvfrom(m_iClientSocketFd,acRecvBuf,sizeof(acRecvBuf),0,(struct sockaddr*)&tRecvServerAddr,&iSockaddrLen);
            if(iRecvLen<0)
            {
                perror("recvfrom Recv err\n");  
                //close(m_iClientSocketFd);   
                //break;
            }
            else
            {
                if(iRecvLen>i_iRecvBufMaxLen)
                {
                    cout<<"Recv err,RecvLen:"<<iRecvLen<<" MaxLen:"<<i_iRecvBufMaxLen<<endl;                    
                    iRet=FALSE;
                    //break;
                }
                else
                {
                    memcpy(pcRecvBuf,acRecvBuf,iRecvLen);
                    pcRecvBuf+=iRecvLen;
                    iRet=TRUE;
                }
            }
        }
        else
        {
            //break;
        }
    }
    
    if(iRecvLen>0 && iRet==TRUE)
    {
        bzero(&tServerAddr, sizeof(tServerAddr));
        tServerAddr.sin_family = AF_INET;
        tServerAddr.sin_port = htons(m_wServerPort);//
        tServerAddr.sin_addr.s_addr = inet_addr(m_strServerIP.c_str());
        if(0!=memcmp(&tServerAddr,&tRecvServerAddr,sizeof(struct sockaddr_in)))
        {//判断数据来源
            string strRecv(inet_ntoa(tRecvServerAddr.sin_addr));
            cout<<"Recv data from err IP:\r\n"<<strRecv<<" Port:"<<ntohs(tRecvServerAddr.sin_port)<<endl;
            iRet=FALSE;
        }
        else
        {
            string strRecv(o_acRecvBuf);
            *o_piRecvLen=iRecvLen;
            cout<<"Recv :\r\n"<<strRecv<<endl;
            iRet=TRUE;
        }
    }
    else
    {
        //cout<<"Recv err:"<<iRecvLen<<endl;
        iRet=FALSE;
    }
    return iRet;
}


/*****************************************************************************
-Fuction        : Close
-Description    : Close
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2017/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
void UdpClient::Close()
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
int UdpClient::GetClientSocket()
{
    return m_iClientSocketFd;
}

