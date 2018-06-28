/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module       : 	Main.cpp
* Description       : 	RtspClient Demo

待完善:
1. (使用VLC的情况下)有花屏,
以及只能接收一段视频(900个包)无法接收完整的
2. //packetization-mode=1 只支持非交错的，这里要容错处理
3. 支持多种包类型(分包模式)
4. 统一日志(统一打印信息)

* Created           : 	2017.09.21.
* Author            : 	Yu Weifeng
* Function List     : 	
* Last Modified     : 	
* History           : 	
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include "RtspClient.h"
#include "Definition.h"

using std::cout;
using std::endl;


#define MAX_RECV_PACKET_NUM 	(1000)

/*****************************************************************************
-Fuction        : main
-Description    : main
-Input          : 
-Output         : 
-Return         : 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int main(int argc,char **argv)
{
	int iRet=FALSE;
	enum RtspState{
		INIT,
		DESCRIBE,
		SETUP,
		PLAY,
		TEARDOWN,
		ERROR,
	};
	enum RtspState eRtspState=INIT;
	bool blRunFlag=true;
	string VideoTransProtocol("");
	string TraceID("");
	int iLocalRtpPort=6000;
	MediaSession MediaSessionHandle(iLocalRtpPort,8000);//还是使用对方的端口?
	string SessionId("");
	int iFd;
	unsigned char aucMediaBuf[100*1024];
	unsigned int dwMediaBufLen=0;
	unsigned int dwPacketNum=0;
	if(argc!=2)
	{
		cout<<"Usage:"<<argv[0]<<" <URL>"<<endl;
		cout<<"For example:"<<endl;
		cout<<argv[0]<<" rtsp://192.168.43.46:8554/1"<<endl;//rtsp://192.168.1.131:8554/1
	}
	else
	{
		RtspClient MyRtspClient(argv[1]);
		while(blRunFlag)
		{//RTSP客户端状态机
			switch(eRtspState)
			{
				case INIT:
				{
					if(FALSE==MyRtspClient.Init())
					{
						eRtspState=ERROR;
						cout<<"RtspClientInit err!"<<endl;
					}
					else
					{
						cout<<"RtspClientInit Success!"<<endl;
						eRtspState=DESCRIBE;
					}

					break;
				}				
				case DESCRIBE:
				{
					if(FALSE==MyRtspClient.SendDescribe())
					{
						eRtspState=ERROR;
						cout<<"RtspClient SendDescribe err!"<<endl;
					}
					else
					{	
						if(FALSE==MyRtspClient.HandleDescribeAck(&VideoTransProtocol,&TraceID))
						{
							eRtspState=ERROR;
							cout<<"RtspClient HandleDescribeAck err!"<<endl;
						}
						else
						{
							eRtspState=SETUP;
						}					
					}
					
					break;
				}
				case SETUP:
				{
					if(FALSE==MyRtspClient.SendSetup(&VideoTransProtocol,&TraceID,iLocalRtpPort))
					{
						eRtspState=ERROR;
						cout<<"RtspClient SendSetup err!"<<endl;
					}
					else
					{	
						if(FALSE==MyRtspClient.HandleSetupAck(&SessionId,&MediaSessionHandle))
						{
							eRtspState=ERROR;
							cout<<"RtspClient HandleSetupAck err!"<<endl;
						}
						else
						{
							eRtspState=PLAY;
						}					
					}
					
					break;
				}
				case PLAY:
				{
					if(FALSE==MyRtspClient.SendPlay(&SessionId))
					{
						eRtspState=ERROR;
						cout<<"RtspClient SendPlay err!"<<endl;
					}
					else
					{	
						if(FALSE==MyRtspClient.HandlePlayAck())
						{
							eRtspState=ERROR;
							cout<<"RtspClient HandlePlayAck err!"<<endl;
						}
						else
						{
							iFd = open("MediaReceived.h264", O_CREAT | O_WRONLY | O_TRUNC);							
							while(++dwPacketNum < MAX_RECV_PACKET_NUM) 
							{
								memset(aucMediaBuf,0,sizeof(aucMediaBuf));
								dwMediaBufLen=0;
								if(FALSE==MyRtspClient.GetVideoData(&MediaSessionHandle, aucMediaBuf, &dwMediaBufLen, sizeof(aucMediaBuf))) 
								{
								}
								else
								{
									if(write(iFd , aucMediaBuf, dwMediaBufLen) < 0) 
									{
										perror(NULL);
										cout<<"write err:"<<dwMediaBufLen<<endl;
									}
									else
									{
									}
									cout<<"RecvLen:"<<dwMediaBufLen<<endl;
								}
								cout<<"PacketNum:"<<dwPacketNum<<endl;
							}
							eRtspState=TEARDOWN;
						}					
					}
					break;
				}
				
				case TEARDOWN:
				{
					if(FALSE==MyRtspClient.SendTeardown(&SessionId))
					{
						eRtspState=ERROR;
						cout<<"RtspClient SendTeardown err!"<<endl;
					}
					else
					{	
						if(FALSE==MyRtspClient.HandleTeardownAck())
						{
							eRtspState=ERROR;
							cout<<"RtspClient HandleTeardownAck err!"<<endl;
						}
						else
						{
							eRtspState=INIT;
						}					
					}
					iRet=TRUE;
					blRunFlag=false;
					break;
				}
				case ERROR:
				{
					MyRtspClient.Close();
					blRunFlag=false;
					break;
				}
				default :
				{
					blRunFlag=false;
					break;
				}
			}
		}
	}
	return iRet;
}

