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


using std::string;

#define IP_MAX_LEN 				(42)
#define RTP_MAX_PACKET_SIZE	((1500-IP_MAX_LEN)/4*4)//MTU
#define RTP_MAX_PACKET_NUM	(300)
#define RTP_HEADER_LEN 			(12)


#define RTP_PAYLOAD_H264    96
#define RTP_PAYLOAD_H265    97
#define RTP_PAYLOAD_G711    104



typedef enum
{
	RTP_PACKET_TYPE_H264 = 0,
    RTP_PACKET_TYPE_H265,
    RTP_PACKET_TYPE_G711U,
    RTP_PACKET_TYPE_G711A,
    RTP_PACKET_TYPE_G726,
    RTP_PACKET_TYPE_AAC
        
}E_RtpPacketType;



typedef struct RtpPacketParam
{
    unsigned int    dwSSRC;
    unsigned short  wSeq;
    unsigned int    dwTimestampFreq;
    unsigned int    wPayloadType;
    unsigned int    dwTimestamp;
}T_RtpPacketParam;//这些参数在每个rtp会话中都不一样，即唯一的。


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
    virtual ~RtpPacket();
    int Init(unsigned char **m_ppPackets,int i_iMaxPacketNum);
    int DeInit(unsigned char **m_ppPackets,int i_iMaxPacketNum);
    int GenerateRtpHeader(T_RtpPacketParam *i_ptParam,T_RtpHeader *o_ptRtpHeader);
    int GenerateRtpHeader(T_RtpPacketParam *i_ptParam,int i_iPaddingLen,int i_iMark,unsigned char *o_bRtpHeader);
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);

protected:
	int m_iRtpType;

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
    virtual ~RtpPacketH264();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);

protected:
	int m_iRtpVideoType;

private:
    RtpPacketH264 *m_pRtpPacketNALU;
    RtpPacketH264 *m_pRtpPacketFU_A;
};

/*****************************************************************************
-Class			: NALU
-Description	: NALU载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H264NALU : public RtpPacketH264
{
public:
    H264NALU();
    virtual ~H264NALU();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);

};

/*****************************************************************************
-Class			: FU_A
-Description	: FU_A载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H264FU_A : public RtpPacketH264
{
public:
    H264FU_A();
    virtual ~H264FU_A();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);
    static const unsigned char FU_A_TYPE;
    static const unsigned char FU_A_HEADER_LEN;
};


/*****************************************************************************
-Class			: RtpPacketH264
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacketH265 : public RtpPacket
{
public:
    RtpPacketH265();
    virtual ~RtpPacketH265();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);

protected:
	int m_iRtpVideoType;

private:
    RtpPacketH265 *m_pRtpPacketNALU;
    RtpPacketH265 *m_pRtpPacketFU_A;
};

/*****************************************************************************
-Class			: NALU
-Description	: NALU载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H265NALU : public RtpPacketH265
{
public:
    H265NALU();
    virtual ~H265NALU();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);

};

/*****************************************************************************
-Class			: FU_A
-Description	: FU_A载荷类型的RTP包
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class H265FU_A : public RtpPacketH265
{
public:
    H265FU_A();
    virtual ~H265FU_A();
    int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);
    static const unsigned char FU_A_TYPE;
    static const unsigned char FU_A_HEADER_LEN;
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
    virtual ~RtpPacketG711();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);
private:
    RtpPacketG711 *m_pRtpPacketG711;
};


/*****************************************************************************
-Class			: RtpPacketG726
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacketG726 : public RtpPacket
{
public:
    RtpPacketG726();
    virtual ~RtpPacketG726();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);
private:
    RtpPacketG726 *m_pRtpPacketG726;
};


/*****************************************************************************
-Class			: RtpPacketAAC
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class RtpPacketAAC : public RtpPacket
{
public:
    RtpPacketAAC();
    virtual ~RtpPacketAAC();
    virtual int Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType=0);
private:
    RtpPacketAAC *m_pRtpPacketAAC;
};














#endif
