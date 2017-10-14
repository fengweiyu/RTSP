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
	int iStatus;
	if(0==m_iRtpPort)
	{
	}
	else
	{
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
	}
	return iRet;
}
/*****************************************************************************
-Fuction		: GetMediaData
-Description	: GetMediaData
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int MediaSession::GetMediaData(unsigned char *o_aucDataBuf,unsigned short *o_wDataLen,unsigned int i_dwTimeoutMs)
{
	int iRet=FALSE;
	RTPPacket *ptPacket=NULL;
	size_t PacketLen = 0;
	uint8_t *pucPacket = NULL;
	unsigned int UsleepTimes=i_dwTimeoutMs;
	do {
#ifndef RTP_SUPPORT_THREAD
		int status = RTPSession::Poll();
		if(status!=0) return iRet;
#endif 

		RTPSession::BeginDataAccess();
		// check incoming packets
		if (!RTPSession::GotoFirstSourceWithData()) 
		{
		}
		else
		{
			ptPacket = RTPSession::GetNextPacket();
			if(NULL==ptPacket)
			{
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
	return iRet;
}

