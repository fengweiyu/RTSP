/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtpPacketType.h
* Description		: 	RtpPacketType operation center
* Created			: 	2017.09.28.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RTP_PACKET_TYPE
#define RTP_PACKET_TYPE


#include <stdlib.h>
#include <stdio.h>

/*****************************************************************************
-Class			: RtpPacket
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacket
{
public:
	unsigned char ParseRtpPacketF(unsigned char *i_aucRtpPayloadData);
	unsigned char ParseRtpPacketNRI(unsigned char *i_aucRtpPayloadData);
	unsigned char ParseRtpPacketType(unsigned char *i_aucRtpPayloadData);
};

/*****************************************************************************
-Class			: NALU
-Description	: NALU载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class NALU: public RtpPacket
{
public:
	bool IsThisPacketType(unsigned char i_ucRtpPacketType);
	bool GetEndFlag();
	unsigned int CopyVideoData(unsigned char *i_aucRtpPacketData,unsigned int i_dwDataLen,unsigned char *o_aucNaluData);
};


/*****************************************************************************
-Class			: FU_A
-Description	: FU_A载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/28	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class FU_A: public RtpPacket
{
public:
	bool IsThisPacketType(unsigned char i_ucRtpPacketType);
	bool GetEndFlag();
	unsigned int CopyVideoData(unsigned char *i_aucRtpPacketData,unsigned int i_dwDataLen,unsigned char *o_aucNaluData);
	
private:
	bool IsPacketStart(unsigned char *i_aucRtpPayloadData);
	bool IsPacketEnd(unsigned char *i_aucRtpPayloadData);
	unsigned char GetNaluHeader(unsigned char *i_aucRtpPayloadData);
	const static unsigned char FU_A_TYPE;
	bool m_blStartFlag;
	bool m_blEndFlag;
};




#endif
