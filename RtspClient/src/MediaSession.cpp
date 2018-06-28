/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	MediaSession.cpp
* Description		: 	传输媒体数据流
* Created			: 	2017.09.26.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <strings.h>
#include "Definition.h"
#include "MediaSession.h"
#include "RtpPacketType.h"

using std::cout;
using std::endl;

/*****************************************************************************
-Fuction		: MediaSession
-Description	: MediaSession
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
MediaSession::MediaSession():m_iTimeRate(8000),m_iRtpPort(554)
{
}
/*****************************************************************************
-Fuction		: MediaSession
-Description	: MediaSession
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
MediaSession::MediaSession(int i_iRtpPort,int i_iTimeRate)
{
	m_iRtpPort=i_iRtpPort;
	m_iTimeRate=i_iTimeRate;
}
/*****************************************************************************
-Fuction		: ~MediaSession
-Description	: ~MediaSession
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
MediaSession::~MediaSession()
{
}

/*****************************************************************************
-Fuction		: Setup
-Description	: RTP Setup
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int MediaSession::Setup()
{
	int iRet=FALSE;
	if(0==m_iRtpPort)
	{
	}
	else
	{
#ifdef USED_JRTPLIB	
        int iStatus;
		// Now, we'll create a RTP session, set the destination
		// and poll for incoming data.		
		RTPUDPv4TransmissionParams tTransparams;
		RTPSessionParams tSessparams;	
		// IMPORTANT: The local timestamp unit MUST be set, otherwise
		//			       RTCP Sender Report info will be calculated wrong
		// In this case, we'll be sending 8000 samples each second, so we'll
		// put the timestamp unit to (1.0/8000),m_iTimeRate=8000
		tSessparams.SetOwnTimestampUnit(1.0/m_iTimeRate);		 	
		tSessparams.SetAcceptOwnPackets(true);
		tTransparams.SetPortbase(m_iRtpPort);
		
		iStatus = RTPSession::Create(tSessparams,&tTransparams);  
		if(0!=iStatus)
		{
			cout<<"RTPSession Create err:"<<iStatus<<endl;
		}
		else
		{
			iRet=TRUE;
		}
#else
		iRet=m_UdpServer.Init(NULL, 0, NULL, m_iRtpPort);
#endif		
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: GetMediaData
-Description	: GetMediaData
-Input			: 获取的是一个完整的rtp包，注意去除了rtp头
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int MediaSession::GetMediaData(unsigned char *o_aucDataBuf,unsigned short *o_wDataLen,unsigned int i_dwTimeoutMs)
{
	int iRet=FALSE;	
#ifdef USED_JRTPLIB	
	
	RTPPacket *ptPacket=NULL;
	size_t PacketLen = 0;
	uint8_t *pucPacket = NULL;
	unsigned int UsleepTimes=i_dwTimeoutMs;
	do {
#ifndef RTP_SUPPORT_THREAD
		int status = RTPSession::Poll();
		if(status!=0) 
		{
		    cout<<"RTPSession::Poll err:"<<status<<endl;
            return iRet;
		}
#endif 

		RTPSession::BeginDataAccess();
		// check incoming packets
		if (!RTPSession::GotoFirstSourceWithData()) 
		{
		    //cout<<"RTPSession::GotoFirstSourceWithData err:"<<endl;
		}
		else
		{
			ptPacket = RTPSession::GetNextPacket();
			if(NULL==ptPacket)
			{
                cout<<"RTPSession::GetNextPacket NULL==ptPacket"<<endl;
			}
			else
			{
				pucPacket = ptPacket->GetPayloadData();
				PacketLen = ptPacket->GetPayloadLength();
				*o_wDataLen = PacketLen;
				memcpy(o_aucDataBuf, pucPacket, PacketLen);			
				// we don't longer need the packet, so
				// we'll delete it
				RTPSession::DeletePacket(ptPacket);
				iRet=TRUE;
				break;
			}
		}
		RTPSession::EndDataAccess();
		usleep(USLEEP_UNIT);
		UsleepTimes--;
	} while(UsleepTimes > 0);
	
#else
    char *pcRecvBuf =(char *)malloc(1024*10);
    int iRecvAllLen=0;
    int iRecvLen=0;
    //bool blRtpPacketStartFlag=false;
    //bool blRtpPacketEndFlag=false;
    timeval tTimeValue;
    tTimeValue.tv_sec      = 0;//超时时间，超时返回错误
    tTimeValue.tv_usec     = 1000;
    while(i_dwTimeoutMs--)
    {
        if(TRUE==m_UdpServer.Recv(pcRecvBuf+iRecvAllLen,&iRecvLen,(1024*10), &tTimeValue))
        {
            iRecvAllLen+=iRecvLen;
            if((unsigned int)iRecvAllLen<sizeof(T_RtpHeader))
            {
                cout<<"m_UdpServer.Recv too short:"<<iRecvAllLen<<" T_RtpHeaderSize:"<<sizeof(T_RtpHeader)<<endl;
            }
            else
            {
                cout<<"iRecvAllLen:"<<iRecvAllLen<<endl;
                T_RtpHeader *ptRtpHeader=(T_RtpHeader *)pcRecvBuf;
                if(ptRtpHeader->Version!=2)
                {
                    cout<<"ptRtpHeader.Version err:"<<pcRecvBuf[0]<<" T_RtpHeaderSize:"<<sizeof(T_RtpHeader)<<endl;
                    break;
                }
                else
                {
                    *o_wDataLen = iRecvAllLen-sizeof(T_RtpHeader);
                    memcpy(o_aucDataBuf, pcRecvBuf+sizeof(T_RtpHeader), *o_wDataLen);          
                    iRet=TRUE;
                    break;
                }
            }
        }

    }
    free(pcRecvBuf);

#endif
	return iRet;
}

