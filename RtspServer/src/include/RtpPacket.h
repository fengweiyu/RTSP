/*****************************************************************************
* Copyright (C) 2017-2020 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtpPacket.h
* Description		: 	RtpPacket operation center
                        包含多种Rtp打包载荷类型，比如NALU,FU-A等载荷类型
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef RTP_PACKET_H
#define RTP_PACKET_H

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "RtpSession.h"


using std::string;

#define RTP_MAX_PACKET_SIZE	((1500-42)/4*4)//MTU
#define RTP_MAX_PACKET_NUM	(300)



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
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacket
{
public:
    RtpPacket();
    ~RtpPacket();
    int GenerateRtpHeader(T_RtpPacketParam *i_ptParam,T_RtpHeader *o_ptRtpHeader);
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio=0);
private:
    RtpPacket *m_pRtpPacket;
};


/*****************************************************************************
-Class			: RtpPacketH264
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacketH264 : public RtpPacket
{
public:
    RtpPacketH264();
    ~RtpPacketH264();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio=0);
private:
    RtpPacketH264 *m_pRtpPacketH264;
};


/*****************************************************************************
-Class			: RtpPacketG711
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacketG711 : public RtpPacket
{
public:
    RtpPacketG711();
    ~RtpPacketG711();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio=0);
private:
    RtpPacketG711 *m_pRtpPacketG711;
};

/*****************************************************************************
-Class			: NALU
-Description	: NALU载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class NALU : public RtpPacketH264
{
public:
    NALU();
    ~NALU();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio=0);

};

/*****************************************************************************
-Class			: FU_A
-Description	: FU_A载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class FU_A : public RtpPacketH264
{
public:
    FU_A();
    ~FU_A();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio=0);
    static const unsigned char FU_A_TYPE;

};














#endif
