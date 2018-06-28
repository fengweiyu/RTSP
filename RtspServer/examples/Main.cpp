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

#include "Definition.h"
#include "RtspServer.h"

using std::cout;
using std::endl;


#define MAX_CHANNEL     16
#define RTSP_SERVER_URL "rtsp://192.168.4.199:8554/1"

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
    int i = 0;
    FILE * apVideoFile[MAX_CHANNEL] = {NULL};//VideoHandle *apVideoHandle[MAX_CHANNEL]
    FILE * apAudioFile[MAX_CHANNEL] = {NULL};//AudioHandle *apAudioHandle[MAX_CHANNEL]
    RtspServer * aRtspServer[MAX_CHANNEL] = {NULL}; 
    
    if (argc>MAX_CHANNEL+1 ||argc <2 /*||argc%2!=1*/) //对输入参数进行容错处理
    {
        PrintUsage(argv[0]);
    }
    else
    {
        iRet = TRUE;
        cout<<"Rtsp server url:"<<RTSP_SERVER_URL<<endl;
        for(i=0;i<argc/2;i++)
        {
            apVideoFile[i] = fopen(argv[i*2+1],"rb");//apVideoHandle[i]=new VideoHandle();
            if(NULL == apVideoFile[i])//if(FALSE==apVideoHandle[i]->Init(argv[i*2+1]))
            {
                cout<<"Open "<<argv[i*2+1]<<"failed !"<<endl;
                iRet = FALSE;
                break;
            }
            if((i*2+1+1)<argc)
            {//过滤没有音频文件的情况
                apAudioFile[i] = fopen(argv[i*2+1+1],"rb");//apAudioHandle[i]=new AudioHandle();
                if(NULL == apVideoFile[i])//if(FALSE==apAudioHandle[i]->Init(argv[i*2+1+1]))
                {
                    cout<<"Open "<<argv[i*2+1+1]<<"failed !"<<endl;
                    iRet = FALSE;
                    break;
                }                
            }
        }
        if(FALSE == iRet)
        {
            PrintUsage(argv[0]);
        }
        else
        {
            for(i=0;i<MAX_CHANNEL;i++)
            {
                if(apVideoFile[i]!=NULL || apAudioFile[i]!=NULL)//if(apVideoHandle[i]!=NULL || apAudioHandle[i]!=NULL)
                {
                    aRtspServer[i] = new RtspServer(apVideoFile[i],apAudioFile[i]);//后续优化为传入读取音视频的函数或者对象
                }//aRtspServer[i] = new RtspServer(apVideoHandle[i],apAudioHandle[i]);
            }

            while(1)
            {
                for(i=0;i<MAX_CHANNEL;i++)
                {
                    if(aRtspServer[i]!=NULL)
                    {
                        aRtspServer[i]->ConnectHandle((char *)RTSP_SERVER_URL);//如果有链接则其内部会保存会话到队列,并有线程管理该队列
                    }
                }
            }
        }    
    }
    //process exit handle
    for(i=0;i<MAX_CHANNEL;i++)
    {
        if(apVideoFile[i]!=NULL)//if(apVideoHandle[i]!=NULL)
        {
            fclose(apVideoFile[i]);//delete apVideoHandle[i];
        }
        if(apAudioFile[i]!=NULL)//if(apAudioHandle[i]!=NULL)
        {
            fclose(apVideoFile[i]);//delete apAudioHandle[i];
        }
        if(aRtspServer[i]!=NULL)
        {
            delete aRtspServer[i];
        }
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
    cout<<"Usage: "<<i_strProcName<<" <H264FILE> "<<endl;
    cout<<"Usage: "<<i_strProcName<<" <H264FILE> <G711AFILE>"<<endl;
    cout<<"Usage: "<<i_strProcName<<" <H264FILE> <G711AFILE> <H264FILE>"<<endl;
    cout<<"Usage: "<<i_strProcName<<" <H264FILE> <G711AFILE> <H264FILE> <G711AFILE> ... ..."<<endl;
}

