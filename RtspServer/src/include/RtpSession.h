/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtpSession.h
* Description		: 	RtpSession operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RTP_SESSION_H
#define RTP_SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "UdpSocket.h"
#include "TcpSocket.h"

using std::string;


#define RTP_PAYLOAD_H264    96
#define RTP_PAYLOAD_G711    97

//后续放到音视频处理类中
#define VIDEO_H264_SAMPLE_RATE 90000
#define AUDIO_G711_SAMPLE_RATE 8000

typedef struct RtpPacketParam
{
    unsigned int    dwSSRC;
    unsigned short  wSeq;
    unsigned int    dwTimestampFreq;
    unsigned int    wPayloadType;
}T_RtpPacketParam;//这些参数在每个rtp会话中都不一样，即唯一的。

/*****************************************************************************
-Class			: RtpSession
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpSession
{
public:
    RtpSession(int i_iVideoOrAudio);
    ~RtpSession();
    int Init(bool i_blIsTcp,string i_strLocalIP,string i_strIP,unsigned short i_wRtpPort,unsigned short i_wRtcpPort);
    int SendRtpData(char * i_acSendBuf,int i_iSendLen);
    int GetRtpSocket();
    int GetRtcpSocket();
    int GetRtpPacketParam(T_RtpPacketParam *o_RtpPacketParam);
    int SetRtpPacketParam(T_RtpPacketParam *i_ptRtpPacketParam);
    unsigned int GetSSRC(void);
    void Close();
private:
    UdpClient   *m_pRtpClientOverUDP;
    UdpClient   *m_pRtcpClientOverUDP;
    TcpClient   *m_pRtpClientOverTCP;
    TcpClient   *m_pRtcpClientOverTCP;
    T_RtpPacketParam m_tRtpPacketParam;
};



#endif
