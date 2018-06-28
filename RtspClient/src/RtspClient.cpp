/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtspClient.cpp
* Description		: 	RtspClient operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <sstream>
#include "Definition.h"
#include "RtpPacketType.h"
#include "Base64.hh"
#include "RtspClient.h"

using std::cout;//需要<iostream>
using std::endl;
using std::stringstream;

/*****************************************************************************
-Fuction		: RtspClient
-Description	: RtspClient
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtspClient::RtspClient(char *i_strURL)
{
	m_URL.assign(i_strURL);
	m_iCSeq=0;
	m_iRtpPort=0;
	m_SPS.assign("");
	m_PPS.assign("");
}

/*****************************************************************************
-Fuction		: ~RtspClient
-Description	: ~RtspClient
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtspClient::~RtspClient()
{


}

/*****************************************************************************
-Fuction		: Init
-Description	: Init
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::Init()
{
	return TcpSocket::Create(m_URL);
}

/*****************************************************************************
-Fuction		: SendDescribe
-Description	: SendDescribe
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::SendDescribe()
{
	int iRet=FALSE;
	string Cmd("DESCRIBE");
	stringstream Msg("");
	Msg<<Cmd<<" "<<m_URL<<" "<<RTSP_VERSION<<"\r\n";
	Msg << "CSeq: " << ++m_iCSeq<< "\r\n";
	Msg << "\r\n";
	string SendString(Msg.str());//返回的是临时对象
	iRet=TcpSocket::Send(&SendString);
	if(iRet<0)
	{
		cout<<"SendDescribe err:"<<iRet<<endl;//后续优化打印为统一打印成日志的方式，并增加打印级别
	}
	else
	{
		iRet=TRUE;
	}
	return iRet;
	
}
/*****************************************************************************
-Fuction		: HandleDescribeAck
-Description	: //只支持h264与非交错类型的,去掉了sps pps后续增加
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::HandleDescribeAck(string *o_pVideoTransProtocol,string *o_pTraceID)
{
	int iRet=FALSE;
	string RtspResponse("");
	iRet=TcpSocket::Recv(&RtspResponse);
	if(FALSE==iRet)
	{
	}
	else
	{
		unsigned int iAckOkPos;
		iAckOkPos=RtspResponse.find(RTSP_ACK_OK);
		if(iAckOkPos==string::npos)
		{
			iRet=FALSE;
			cout<<"HandleDescribeAck can not find RTSP_ACK_OK"<<iAckOkPos<<endl;
		}
		else
		{
			//Parse SDP//嵌套层次太深后续优化
			unsigned int iContentLengthPos;
			unsigned int iVideoPos;
			unsigned int iVideoEndPos;
			unsigned int iVideoTransProtocolPos;
			unsigned int iVideoTransProtocolEndPos;
			unsigned int iTraceIdPos;
			unsigned int iTraceIdEndPos;
			iContentLengthPos=RtspResponse.find("Content-Length");
			if(iContentLengthPos==string::npos)
			{
				iRet=FALSE;
				cout<<"can not find content length"<<iContentLengthPos<<endl;
			}
			else
			{
				iVideoPos=RtspResponse.find("m=video");
				if(iVideoPos==string::npos)
				{
					iRet=FALSE;
					cout<<"can not find video"<<iVideoPos<<endl;
				}
				else
				{
					iVideoEndPos=RtspResponse.find("\r\n",iVideoPos);
					if(iVideoEndPos==string::npos)
					{
						iRet=FALSE;
						cout<<"can not find video end"<<iVideoEndPos<<endl;
					}
					else
					{
						iVideoTransProtocolPos=RtspResponse.find(" ",iVideoPos+8);
						if(iVideoTransProtocolPos==string::npos)
						{
							iRet=FALSE;
							cout<<"can not find videoTransProtocol"<<iVideoTransProtocolPos<<endl;
						}
						else
						{
							iVideoTransProtocolEndPos=RtspResponse.rfind(" ",iVideoEndPos);
							if(iVideoTransProtocolEndPos==string::npos)
							{
								iRet=FALSE;
								cout<<"can not find videoTransProtocol end"<<iVideoTransProtocolEndPos<<endl;
							}
							else
							{
								o_pVideoTransProtocol->assign("");
								o_pVideoTransProtocol->assign(RtspResponse,iVideoTransProtocolPos+1,iVideoTransProtocolEndPos-iVideoTransProtocolPos-1);
								iTraceIdPos=RtspResponse.find("track");
								if(iTraceIdPos==string::npos)
								{
									iRet=FALSE;
									cout<<"can not find TraceId"<<iTraceIdPos<<endl;
								}
								else
								{
									iTraceIdEndPos=RtspResponse.find("\r\n",iTraceIdPos);
									if(iTraceIdEndPos==string::npos)
									{
										iRet=FALSE;
										cout<<"can not find TraceIdEnd"<<iTraceIdEndPos<<endl;
									}
									else
									{
									
										o_pTraceID->assign("");//这里是完整地址，所以后面就要去掉url直接用这个就可以
										o_pTraceID->assign(RtspResponse,iTraceIdPos,iTraceIdEndPos-iTraceIdPos);
										iRet=TRUE;
										cout<<"VideoTransProtocol:"<<o_pVideoTransProtocol->c_str()<<" TraceID:"<<o_pTraceID->c_str()<<endl;
										GetSPS_PPS(RtspResponse);
									}
								}
							}
						}						
					}
				}
			}	
		}	
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: SendSetup
-Description	: SendSetup
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::SendSetup(string *i_pVideoTransProtocol,string *i_pTraceId,int i_iRtpPort)
{
	int iRet=FALSE;
	string Cmd("SETUP");
	stringstream Msg("");
	Msg << Cmd << " " <<m_URL<<"/"<<i_pTraceId->c_str()<< " "<< RTSP_VERSION << "\r\n";	
	Msg << "CSeq: " << ++m_iCSeq << "\r\n";
	Msg << "Transport: " << i_pVideoTransProtocol->c_str() << "/UDP;unicast;client_port="<< i_iRtpPort<< "-" << i_iRtpPort+1 << "\r\n";
	//暂不使用认证
	//Msg << "User-Agent: fengweiyu" << "\r\n";暂不指明用户
	Msg << "\r\n";
	string SendString(Msg.str());//返回的是临时对象
	iRet=TcpSocket::Send(&SendString);
	if(iRet<0)
	{
		cout<<"SendSetup err:"<<iRet<<endl;
	}
	else
	{
		iRet=TRUE;
		m_iRtpPort=i_iRtpPort;
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: HandleSetupAck
-Description	: HandleSetupAck
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::HandleSetupAck(string *o_pSessionId,MediaSession *i_pMediaSession)
{
	int iRet=FALSE;
	string RtspResponse("");
	iRet=TcpSocket::Recv(&RtspResponse);
	if(FALSE==iRet)
	{
	}
	else
	{
		unsigned int iAckOkPos;
		iAckOkPos=RtspResponse.find(RTSP_ACK_OK);
		if(iAckOkPos==string::npos)
		{
			iRet=FALSE;
			cout<<"HandleSetupAck can not find RTSP_ACK_OK"<<iAckOkPos<<endl;
		}
		else
		{
			//get session id
			unsigned int iSessionIdPos;
			unsigned  iSessionIdEndPos;
			iSessionIdPos=RtspResponse.find("Session:");
			if(iSessionIdPos==string::npos)
			{
				iRet=FALSE;
				cout<<"HandleSetupAck can not find SessionIdPos"<<iSessionIdPos<<endl;
			}
			else
			{
				iSessionIdEndPos=RtspResponse.find(";",iSessionIdPos);
				if(iSessionIdEndPos==string::npos)
				{
					iRet=FALSE;
					cout<<"HandleSetupAck can not find iSessionIdEndPos"<<iSessionIdEndPos<<endl;
				}
				else
				{
					o_pSessionId->assign("");
					o_pSessionId->assign(RtspResponse,iSessionIdPos+8,iSessionIdEndPos-iSessionIdPos-8);
					cout<<" SessionId:"<<o_pSessionId->c_str()<<endl;
					//setup rtp session
					iRet=i_pMediaSession->Setup();
				}
			}
		}
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: SendPlay
-Description	: SendPlay
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::SendPlay(string *i_pSessionId)
{
	int iRet=FALSE;
	string Cmd("PLAY");
	stringstream Msg("");
	Msg << Cmd << " " << m_URL<< " "<< RTSP_VERSION << "\r\n";	
	Msg << "CSeq: " << ++m_iCSeq << "\r\n";
	Msg << "Session: " << i_pSessionId->c_str() << "\r\n";
	//暂不使用认证
	//Msg << "User-Agent: fengweiyu" << "\r\n";暂不指明用户
	Msg << "\r\n";
	string SendString(Msg.str());//返回的是临时对象
	iRet=TcpSocket::Send(&SendString);
	if(iRet<0)
	{
		cout<<"SendPlay err:"<<iRet<<endl;
	}
	else
	{
		iRet=TRUE;
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: HandlePlayAck
-Description	: HandlePlayAck
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::HandlePlayAck()
{
	int iRet=FALSE;
	string RtspResponse("");
	iRet=TcpSocket::Recv(&RtspResponse);
	if(FALSE==iRet)
	{
	}
	else
	{
		unsigned int iAckOkPos;
		iAckOkPos=RtspResponse.find(RTSP_ACK_OK);
		if(iAckOkPos==string::npos)
		{
			iRet=FALSE;
			cout<<"HandlePlayAck can not find RTSP_ACK_OK"<<iAckOkPos<<endl;
		}
		else
		{
			iRet=TRUE;
		}
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: GetSpsNalu
-Description	: GetSpsNalu
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::GetSpsNalu(unsigned char *o_acVideoData,unsigned int *o_dwDataLen)
{
	int iRet=FALSE;	
	unsigned int dwSpsSize = 0;
	unsigned char * pucSPS=NULL;
	if(NULL==o_acVideoData||NULL==o_dwDataLen)
	{
	}
	else
	{
		*o_dwDataLen = 0;
		
		o_acVideoData[0] = 0; 
		o_acVideoData[1] = 0; 
		o_acVideoData[2] = 0; 
		o_acVideoData[3] = 1;
		*o_dwDataLen += 4;
		
		pucSPS = base64Decode(m_SPS.c_str(), dwSpsSize, true);
		memcpy(o_acVideoData + (*o_dwDataLen), pucSPS, dwSpsSize);
		*o_dwDataLen += dwSpsSize;
		
		delete[] pucSPS;
		pucSPS = NULL;
	
		iRet=TRUE;
	}		
	return iRet;
}
/*****************************************************************************
-Fuction		: GetPpsNalu
-Description	: GetPpsNalu
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::GetPpsNalu(unsigned char *o_acVideoData,unsigned int *o_dwDataLen)
{
	int iRet=FALSE;	
	unsigned int dwPpsSize = 0;
	unsigned char * pucPPS = NULL;
	if(NULL==o_acVideoData||NULL==o_dwDataLen)
	{
	}
	else
	{
		*o_dwDataLen = 0;
		
		o_acVideoData[0] = 0; 
		o_acVideoData[1] = 0; 
		o_acVideoData[2] = 0; 
		o_acVideoData[3] = 1;
		*o_dwDataLen += 4;
		
		pucPPS = base64Decode(m_PPS.c_str(), dwPpsSize, true);
		memcpy(o_acVideoData + (*o_dwDataLen), pucPPS, dwPpsSize);
		*o_dwDataLen += dwPpsSize;
		
		delete[] pucPPS;
		pucPPS = NULL;

		iRet=TRUE;
	}

	return iRet;
}

/*****************************************************************************
-Fuction		: GetVideoData
-Description	: GetVideoData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::GetVideoData(MediaSession *i_pMediaSession,unsigned char *o_VideoData,unsigned int *o_dwDataLen,unsigned int i_dwDataMaxLen)
{
	int iRet=FALSE;
	unsigned char aucVideoBuf[VIDEO_BUF_LEN]={0};
	unsigned short wVideoBufLen=0;
	unsigned char *pucVideoData=o_VideoData;
	unsigned int dwVideoDataLen=0;
	unsigned int dwTempLen=0;
	bool blEndFlag=false;

	unsigned char RtpPacketType=0;
	RtpPacket RtpPacketHandle;
	NALU NaluHandle;
	FU_A  FU_A_Handle;

	static int s_iGetVideoDataCnt=GET_SPS_PPS_PERIOD;
	//文件一开始就要加入SPS PPS参数对应的数据包,否则播放器(vlc)就无法播放(文件名一直在闪烁)
	if(s_iGetVideoDataCnt >= GET_SPS_PPS_PERIOD) 
	{
		s_iGetVideoDataCnt = 0;
	
		iRet=GetSpsNalu(o_VideoData + dwVideoDataLen, &dwTempLen);
		if(FALSE==iRet|| dwTempLen <= NALU_START_CODE_LEN) 
		{
			cout<<"WARNING: No SPS:"<<iRet<<endl;
		} 
		else 
		{
			dwVideoDataLen += dwTempLen;
		}
		iRet=GetPpsNalu(o_VideoData + dwVideoDataLen, &dwTempLen);
		if(FALSE==iRet || dwTempLen <= NALU_START_CODE_LEN) 
		{
			cout<<"WARNING: No PPS:"<<iRet<<endl;
		} 
		else 
		{
			dwVideoDataLen += dwTempLen;
		}
	} 
	else 
	{
		s_iGetVideoDataCnt++;
		do
		{
			blEndFlag=true;
			memset(aucVideoBuf,0,sizeof(aucVideoBuf));
			iRet=i_pMediaSession->GetMediaData(aucVideoBuf,&wVideoBufLen);
			if(iRet==FALSE||(wVideoBufLen+dwVideoDataLen)>i_dwDataMaxLen||wVideoBufLen>sizeof(aucVideoBuf)||wVideoBufLen<2)
			{
				//cout<<"GetMediaData err:"<<wVideoBufLen<<endl;
				printf("GetMediaData err,len:%d\r\n",wVideoBufLen);
				break;
			}
			else
			{//组包处理
				RtpPacketType=RtpPacketHandle.ParseRtpPacketType(aucVideoBuf);
				if(true==NaluHandle.IsThisPacketType(RtpPacketType))
				{
					dwVideoDataLen+=NaluHandle.CopyVideoData(aucVideoBuf,wVideoBufLen,pucVideoData+dwVideoDataLen);
					blEndFlag=NaluHandle.GetEndFlag();
				}
				else if(true==FU_A_Handle.IsThisPacketType(RtpPacketType))
				{
					dwVideoDataLen+=FU_A_Handle.CopyVideoData(aucVideoBuf,wVideoBufLen,pucVideoData+dwVideoDataLen);
					blEndFlag=FU_A_Handle.GetEndFlag();
				}
				else
				{
					//cout<<"GetMediaData err,UnkownRtpPacketType:"<<RtpPacketType<<endl;
					printf("GetMediaData err,UnkownRtpPacketType:%#x,len:%d\r\n",RtpPacketType,wVideoBufLen);
					break;
				}
				//printf("GetMediaData,RtpPacketType:%#x,len:%d,EndFlag:%d\r\n",RtpPacketType,wVideoBufLen,blEndFlag);
			}
		}while(!blEndFlag);
	}

	*o_dwDataLen=dwVideoDataLen;
	
	return iRet;
}
/*****************************************************************************
-Fuction		: SendTeardown
-Description	: SendTeardown
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::SendTeardown(string *i_pSessionId)
{
	int iRet=FALSE;
	string Cmd("TEARDOWN");
	stringstream Msg("");
	Msg << Cmd << " " << m_URL<< " " << RTSP_VERSION << "\r\n";	
	Msg << "CSeq: " << ++m_iCSeq << "\r\n";
	Msg << "Session: " << i_pSessionId->c_str() << "\r\n";
	//暂不使用认证
	//Msg << "User-Agent: fengweiyu" << "\r\n";暂不指明用户
	Msg << "\r\n";
	string SendString(Msg.str());//返回的是临时对象
	iRet=TcpSocket::Send(&SendString);
	if(iRet<0)
	{
		cout<<"SendTeardown err:"<<iRet<<endl;
	}
	else
	{
		iRet=TRUE;
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: HandleTeardownAck
-Description	: HandleTeardownAck
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::HandleTeardownAck()
{
	int iRet=FALSE;
	string RtspResponse("");
	iRet=TcpSocket::Recv(&RtspResponse);
	if(FALSE==iRet)
	{
	}
	else
	{
		unsigned int iAckOkPos;
		iAckOkPos=RtspResponse.find(RTSP_ACK_OK);
		if(iAckOkPos==string::npos)
		{
			iRet=FALSE;
			cout<<"HandleTeardownAck can not find RTSP_ACK_OK"<<iAckOkPos<<endl;
		}
		else
		{
			iRet=TRUE;
		}
	}
	TcpSocket::Close();
	return iRet;
}

/*****************************************************************************
-Fuction		: GetSPS_PPS
-Description	: Get SPS and PPS from SDP
示例:
sprop-parameter-sets=Z2QAIKzZQEgG2wEQAAADABAAAAMB6PGDGWA=,aOvjyyLA;

-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtspClient::GetSPS_PPS(string &i_SDP)
{
	int iRet=FALSE;
	unsigned int dwSpsPos;
	unsigned int dwSpsEndPos;
	//unsigned int dwPpsPos;
	unsigned int dwPpsEndPos;
	
	dwSpsPos=i_SDP.find("sprop-parameter-sets=");//返回的位置是从0开始的，即第一个位置是0
	if(string::npos==dwSpsPos)
	{
		iRet=FALSE;
		cout<<"GetSPS_PPS  find sprop-parameter-sets=  err"<<endl;
		return iRet;
	}

	dwSpsEndPos=i_SDP.find(",",dwSpsPos);
	if(string::npos==dwSpsEndPos)
	{
		iRet=FALSE;
		cout<<"GetSPS_PPS  find ,  err"<<endl;
		return iRet;
	}

	m_SPS.assign(i_SDP,dwSpsPos+strlen("sprop-parameter-sets="),dwSpsEndPos-(dwSpsPos+strlen("sprop-parameter-sets=")));	
	dwPpsEndPos=i_SDP.find("\r\n",dwSpsEndPos);
	if(string::npos==dwSpsEndPos)
	{
		iRet=FALSE;
		cout<<"GetSPS_PPS  find \r\n err"<<endl;
		return iRet;
	}
	
	m_PPS.assign(i_SDP,dwSpsEndPos+1,dwPpsEndPos-(dwSpsEndPos+1));
	cout<<"GetSPS_PPS  find success sps:"<<m_SPS<<" vps:"<<m_PPS<<endl;
	iRet=TRUE;
	return iRet;

	//packetization-mode=1 只支持非交错的，这里要容错处理
}




