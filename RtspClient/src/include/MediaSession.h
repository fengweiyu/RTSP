/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	MediaSession.h
* Description		: 	传输媒体数据流
* Created			: 	2017.09.26.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef MEDIA_SESSION_H
#define MEDIA_SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"
#include "UdpSocket.h"

using namespace jrtplib;



#define USLEEP_UNIT 	1000//unit:us
#define TIMEOUT_MS 1000//unit:ms

/*****************************************************************************
-Class			: MediaSession
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class MediaSession : public RTPSession
{
public:
	MediaSession();
	MediaSession(int i_iRtpPort,int i_iTimeRate);
	~MediaSession();
	
	int Setup();
	int GetMediaData(unsigned char *o_aucDataBuf,unsigned short *o_wDataLen,unsigned int i_dwTimeoutMs= TIMEOUT_MS);

private:
	int m_iTimeRate;
	int m_iRtpPort;
	UdpServer m_UdpServer;
};








#endif
