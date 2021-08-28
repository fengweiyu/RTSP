/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtspServer.h
* Description		: 	RtspServer operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RTSP_CLIENT_H
#define RTSP_CLIENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include "TcpSocket.h"
#include "RtpSession.h"
#include "RtpPacket.h"
#include "MediaHandle.h"

using std::map;
using std::string;
using std::list;

#define RTSP_SERVER_NAME                        "yuweifeng RTSP Server Process "
#define RTSP_SERVER_VERSION                     "V1.0.0, 2018.05.18"



#define RTSP_VERSION                        "RTSP/1.0"
#define RTSP_RESPONSE_OK                    " 200 OK"

#define RTSP_RESPONSE_BAD_400                                   " 400 Bad Request"
#define RTSP_RESPONSE_ERROR_404                                 " 404 File Not Found, Or In Incorrect Format"
#define RTSP_RESPONSE_NOT_FOUND_404                             " 404 Stream Not Found"
#define RTSP_RESPONSE_NOT_SUPPORTED_405                         " 405 Method Not Allowed"
#define RTSP_RESPONSE_PARAM_NOT_UNDERSTOOD_451                  " 451 Parameter Not Understood"
#define RTSP_RESPONSE_CONFERENCE_NOT_FOUND_452                  " 452 Conference Not Found"
#define RTSP_RESPONSE_NOT_ENOUGH_BANDWIDTH_453                  " 453 Not Enough Bandwidth"
#define RTSP_RESPONSE_SESSION_NOT_FOUND_454                     " 454 Session Not Found"
#define RTSP_RESPONSE_METHOD_NOT_VALID_455                      " 455 Method Not Valid in This State"
#define RTSP_RESPONSE_HEADER_FIELD_NOT_VALID_456                " 456 Header Field Not Valid for Resource"
#define RTSP_RESPONSE_INVALID_RANGE_457                         " 457 Invalid Range"
#define RTSP_RESPONSE_PARAM_IS_READ_ONLY_458                    " 458 Parameter Is Read-Only"
#define RTSP_RESPONSE_AGGREGATE_OPR_NOT_ALLOWED_459             " 459 Aggregate Operation Not Allowed"
#define RTSP_RESPONSE_ONLY_AGGREGATE_OPR_ALLOWED_460            " 460 Only Aggregate Operation Allowed"
#define RTSP_RESPONSE_UNSUPPORTED_TRANSPORT_461                 " 461 Unsupported Transport"
#define RTSP_RESPONSE_DESTINATION_UNREACHABLE_462               " 462 Destination Unreachable"
#define RTSP_RESPONSE_INTERNAL_SERVER_ERROR_500                 " 500 Internal Server Error"
#define RTSP_RESPONSE_OPTION_NOT_SUPPORTED_551                  " 551 Option not supported"


#define SDP_H264_ENC_FORMAT_NAME            "H264"
#define SDP_H265_ENC_FORMAT_NAME            "H265"

#define SDP_AAC_ENC_FORMAT_NAME            	"mpeg4-generic"
#define SDP_G711U_ENC_FORMAT_NAME           "PCMU"
#define SDP_G711A_ENC_FORMAT_NAME           "PCMA"

#define SDP_AUDIO_CHANNEL_NUM                      "/1"


#define RTSP_TRANSPORT_BASE                 "RTP/AVP/"
#define RTSP_TRANSPORT_RTP_OVER_TCP         "RTP/AVP/TCP"






#define VIDEO_BUF_LEN				8192

#define GET_SPS_PPS_PERIOD 			30
#define NALU_START_CODE_LEN 		4


typedef enum RtspState
{
    INIT,
    SETUP_PLAY_READY,//只有播放状态需要控制
    PLAYING,
    
}E_RtspState;


typedef struct Session
{
    int             iClientSocketFd;
    struct timeval  tCreateTime;//会话创建时的时间，用作sessionid
    E_RtspState     eRtspState;
    int             iTrackNumber;//表示当前是第几个会话,用作trackid
    RtpSession      *pVideoRtpSession;
    RtpSession      *pAudioRtpSession;
    unsigned int    dwSessionId;
    unsigned int 	dwLastTimestamp;//流控需要
    struct timespec  tLastTimeSpec;
    int             iTimeOut;//The server uses it to indicate to the client how long the server is prepared to wait between RTSP commands before closing the session due to lack of activity
    time_t          dwLastRecvDataTime;//由于没有那么多客户端的链接,资源足够不需要主动关闭,也就是使用的是长链接。资源足够所以暂时不需要超时来关闭客户端
}T_Session;//没有超时关闭机制,虽然tcp会两小时超时关闭,但是会话链表m_SessionList中保存这个关闭的会话没有删除，所以还需优化

typedef struct VideoEncTypeToSdpEncName
{
	E_VideoEncodeType eVideoEncodeType;
	char srtSdpEncName[16];
}T_VideoEncTypeToSdpEncName;
typedef struct AudioEncTypeToSdpEncName
{
	E_AudioEncodeType eAudioEncodeType;
	char srtSdpEncName[16];
}T_AudioEncTypeToSdpEncName;



/*****************************************************************************
-Class			: RtspServer
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtspServer : public TcpServer
{
public:
	RtspServer();
	~RtspServer();

	int Init(char *i_strURL,char * i_strFilePath);
    int WaitConnectHandle();
    int SessionHandle();
    int RtspStreamHandle();
    
    static void * SessionHandleThread(void *pArg);
    static void * RtspStreamHandleThread(void *pArg);
    static string m_astrCmd[];
	
	
private:
    int GetIpAndPort(string * i_pstrURL,string *o_Ip,unsigned short *o_wPort);
    int RtspCmdHandle(T_Session *i_ptSession,string *i_pstrMsg,string *o_pstrMsg);
    int HandleCmdOPTIONS(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);
    int HandleCmdDESCRIBE(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);
    int HandleCmdSETUP(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);
    int HandleCmdPLAY(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);
    int HandleCmdPAUSE(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);
    int HandleCmdTEARDOWN(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);


    int GenerateSDP(T_Session *i_ptSession,string *o_pstrSDP);
    int GenerateMediaSDP(T_Session *i_ptSession,string *o_pstrMediaSDP);
    const char * GetDateHeader();



	typedef int (RtspServer::*HandleCmd)(T_Session *i_ptSession,string *i_pstrMsg,int i_iCSeq,string *o_pstrMsg);//放在类内部也要指明类名，不然编译器无法转换

	MediaHandle             m_MediaHandle;
	
	unsigned int            m_dwBandwidth;
	string                  m_strURL; 
	string                  m_strIP;
	unsigned short          m_wPort;
	list<T_Session>	        m_SessionList;
	pthread_mutex_t 		m_tSessionMutex; 
	map<string, HandleCmd>  m_HandleCmdMap;
	int                     m_iSessionCount;//链接上的会话个数
	RtpPacket               *m_pRtpPacket;
};




#endif
