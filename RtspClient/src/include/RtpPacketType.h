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

typedef struct RtpHeader
{
#ifdef RTP_BIG_ENDIAN
	unsigned short Version:2;//版本号（V）：用来标志使用的RTP版本，占2位，当前协议版本号为2
	unsigned short Pad:1;//填充位（P）：填充标志，占1位，如果P=1，则该RTP包的尾部就包含附加的填充字节
	unsigned short Extend:1;//扩展位（X）：扩展标志，占1位，如果X=1，则在RTP固定头部后面就跟有一个扩展头部
	unsigned short CsrcCount:4;//CSRC计数器（CC）：CSRC计数器，占4位，指示固定头部后面跟着的CSRC 标识符的个数

	unsigned short Mark:1;//标记位（M）：标记，占1位，一般而言，对于视频，标记一帧的结束；对于音频，标记会话的开始
	unsigned short PayloadType:7;//载荷类型（PayloadType）： 有效荷载类型，占7位，用于说明RTP报文中有效载荷的类型
#else //little endian
	unsigned short CsrcCount:4;//CSRC计数器（CC）：CSRC计数器，占4位，指示固定头部后面跟着的CSRC 标识符的个数
	unsigned short Extend:1;//扩展位（X）：扩展标志，占1位，如果X=1，则在RTP固定头部后面就跟有一个扩展头部
	unsigned short Pad:1;//填充位（P）：填充标志，占1位，如果P=1，则该RTP包的尾部就包含附加的填充字节
	unsigned short Version:2;//版本号（V）：用来标志使用的RTP版本，占2位，当前协议版本号为2

	unsigned short PayloadType:7;//载荷类型（PayloadType）： 有效荷载类型，占7位，用于说明RTP报文中有效载荷的类型
	unsigned short Mark:1;//标记位（M）：标记，占1位，一般而言，对于视频，标记一帧的结束；对于音频，标记会话的开始
#endif
	unsigned short wSeq;//序列号（SN）：占16位，用于标识发送者所发送的RTP报文的序列号，每发送一个报文，序列号增1
	unsigned int dwTimestamp;//时间戳(Timestamp): 占32位，记录了该包中数据的第一个字节的采样时刻
	unsigned int dwSSRC;//同步源标识符(SSRC)：占32位，用于标识同步信源，同步源就是指RTP包流的来源。在同一个RTP会话中不能有两个相同的SSRC值
}T_RtpHeader;//size 12

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
