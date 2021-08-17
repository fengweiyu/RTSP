/*****************************************************************************
* Copyright (C) 2017-2020 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module       : 	Main.cpp
* Description       : 	RtspServer Demo

设计思路:
音视频源和socket单独出来作为rtspserver的参数
单独的会话等待线程得到新的socket后，则new一个rtspserver
然后把new出来的这个rtspserver包括其socket链接 也就是会话保存到队列里
同时有一个线程不断去处理这个会话队列。
其中,rtsp命令可以抽出来，放到管理队列的类中。

待完善：
1.使用vlc作为客户端无法播放，只能使用对应的客户端
2.关于URL目前是用RTSP_SERVER_URL宏定义写死的，后续要改为可配置
或者优化为不用传ip，默认端口的方式。
3.rtsp目前直接依赖MediaHandle VideoHandle，后续考虑设计模式
4.MediaHandle获取媒体参数信息，多线程考虑加锁。

* Created           : 	2017.11.21.
* Author            : 	Yu Weifeng
* Function List     : 	
* Last Modified     : 	
* History           : 	
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "Definition.h"
#include "RtspServer.h"

using std::cout;
using std::endl;


#define MAX_CHANNEL     16
#define RTSP_SERVER_URL "rtsp://%s:8554/1"
#define RTSP_SERVER_PORT 8554

static void PrintUsage(char *i_strProcName);

/*****************************************************************************
-Fuction        : main
-Description    : main
-Input          : 
-Output         : 
-Return         : 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int main(int argc,char **argv)
{
    int iRet = FALSE;
    RtspServer * pRtspServer = NULL; 
    char strURL[128];
    
    if (argc != 3) //对输入参数进行容错处理
    {
        PrintUsage(argv[0]);
    }
    else
    {
        memset(strURL,0,sizeof(strURL));
        snprintf(strURL,sizeof(strURL),RTSP_SERVER_URL,argv[1]);
        cout<<"Rtsp server url:"<<strURL<<endl;
        pRtspServer = new RtspServer();
        if(pRtspServer!=NULL)
        {
            iRet = pRtspServer->Init((char *)strURL,argv[2]);//如果有链接则其内部会保存会话到队列,并有线程管理该队列
        }
        if(iRet == TRUE)
        {
            while(1)
            {
                pRtspServer->WaitConnectHandle();//如果有链接则其内部会保存会话到队列,并有线程管理该队列
            }
        }
        else
        {
            PrintUsage(argv[0]);
        }
    }
    //process exit handle
    if(pRtspServer!=NULL)
    {
        delete pRtspServer;
    }
    return 0;
}

/*****************************************************************************
-Fuction        : PrintUsage
-Description    : PrintUsage
-Input          : 
-Output         : 
-Return         : 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void PrintUsage(char *i_strProcName)
{
    cout<<"Usage: "<<i_strProcName<<" 192.168.7.199"<<" <H264FILE> "<<endl;
    cout<<"Usage: "<<i_strProcName<<" 192.168.7.199"<<" <G711AFILE>"<<endl;
    cout<<"Usage: "<<i_strProcName<<" 192.168.7.199"<<" <H265FILE>"<<endl;
    cout<<"eg: "<<i_strProcName<<" 192.168.7.199"<<" sintel.h264"<<endl;    
}

