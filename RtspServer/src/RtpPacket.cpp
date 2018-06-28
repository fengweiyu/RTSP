/*****************************************************************************
* Copyright (C) 2017-2020 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RtpPacket.cpp
* Description		: 	RtpPacket operation center
                        包含多种Rtp打包载荷类型，比如NALU,FU-A等载荷类型
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <stdlib.h>//还是需要.h
#include <stdio.h>
#include <string.h>
#include <iostream>//不加.h,c++新的头文件

#include "Definition.h"
#include "Tools.h"
#include "RtpPacket.h"

using std::cout;//需要<iostream>
using std::endl;

/*****************************************************************************
-Fuction		: RtpPacket
-Description	: RtpPacket
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacket :: RtpPacket()
{
    m_pRtpPacket = NULL;

}

/*****************************************************************************
-Fuction		: RtpPacket
-Description	: RtpPacket
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacket :: ~RtpPacket()
{
    if(m_pRtpPacket != NULL)
        delete m_pRtpPacket;
}

/*****************************************************************************
-Fuction		: GenerateRtpHeader
-Description	: GenerateRtpHeader
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtpPacket :: GenerateRtpHeader(T_RtpPacketParam *i_ptParam,T_RtpHeader *o_ptRtpHeader)
{
    int iRet=FALSE;
    if(NULL == i_ptParam || NULL == o_ptRtpHeader)
    {
        cout<<"GenerateRtpHeader err NULL"<<endl;
    }
    else
    {
        T_RtpHeader tRtpHeader={0};
        unsigned int dwTimestamp=(unsigned int)(Tools::Instance()->GetSysTime()* i_ptParam->dwTimestampFreq/ 1000000);
        tRtpHeader.Version=2;
        tRtpHeader.Pad=0;
        tRtpHeader.Extend=0;
        tRtpHeader.CsrcCount=0;
        tRtpHeader.Mark=0;
        tRtpHeader.PayloadType=i_ptParam->wPayloadType;
        tRtpHeader.wSeq=i_ptParam->wSeq++;
        tRtpHeader.dwTimestamp=dwTimestamp;
        tRtpHeader.dwSSRC=i_ptParam->dwSSRC;

        memcpy(o_ptRtpHeader,&tRtpHeader,sizeof(T_RtpHeader));
        iRet=TRUE;
    }
    return iRet;
}

/*****************************************************************************
-Fuction		: Packet
-Description	: Packet
-Input			: i_ptParam i_pbNaluBuf i_iNaluLen i_iVideoOrAudio=0 默认视频，1音频
-Output 		: 二维数组o_ppPackets放多包的内容，数组o_aiEveryPacketLen放每包的长度
-Return 		: 包的数目PacketsNum
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtpPacket :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio)
{
    int iRet=FALSE;
	if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        if(NULL != m_pRtpPacket)
        {
            delete m_pRtpPacket;
        }
        if(0==i_iVideoOrAudio)
        {
            m_pRtpPacket = new RtpPacketH264();
            iRet=m_pRtpPacket->Packet(i_ptParam,i_pbFrameBuf,i_iFrameLen,o_ppPackets,o_aiEveryPacketLen);
        }
        else
        {
            m_pRtpPacket = new RtpPacketG711();
            iRet=m_pRtpPacket->Packet(i_ptParam,i_pbFrameBuf,i_iFrameLen,o_ppPackets,o_aiEveryPacketLen);
        }
    }
    return iRet;
}
/*****************************************************************************
-Fuction		: RtpPacketH264
-Description	: RtpPacketH264
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketH264 :: RtpPacketH264()
{
    m_pRtpPacketH264 = NULL;

}

/*****************************************************************************
-Fuction		: RtpPacketH264
-Description	: ~RtpPacketH264
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketH264 :: ~RtpPacketH264()
{
    if(m_pRtpPacketH264 != NULL)
        delete m_pRtpPacketH264;

}

/*****************************************************************************
-Fuction		: Packet
-Description	: Packet
-Input			: i_ptParam i_pbNaluBuf i_iNaluLen i_iVideoOrAudio=0 默认视频，1音频
-Output 		: 二维数组o_ppPackets放多包的内容，数组o_aiEveryPacketLen放每包的长度
-Return 		: 包的数目PacketsNum
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtpPacketH264 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio)
{
    int iRet=FALSE;
    unsigned char *pbNaluBuf=i_pbNaluBuf;
    int iNaluLen=i_iNaluLen;
	if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        //drop 0001
        if (pbNaluBuf[0] == 0 && pbNaluBuf[1] == 0 && pbNaluBuf[2] == 1) 
        {
            pbNaluBuf   += 3;
            iNaluLen    -= 3;
        }
        if (pbNaluBuf[0] == 0 && pbNaluBuf[1] == 0 && pbNaluBuf[2] == 0 && pbNaluBuf[3] == 1) 
        {
            pbNaluBuf   += 4;
            iNaluLen    -= 4;
        }
        
        if(NULL != m_pRtpPacketH264)
        {
            delete m_pRtpPacketH264;
        }
        if((unsigned int)iNaluLen <=RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader))
        {//单个NAL包单元
            m_pRtpPacketH264 = new NALU();
            iRet=m_pRtpPacketH264->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
        }
        else
        {//分片单元（FU-A）
            m_pRtpPacketH264 = new FU_A();
            iRet=m_pRtpPacketH264->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
        }
    }
    return iRet;
}

/*****************************************************************************
-Fuction		: NALU
-Description	: NALU
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
NALU :: NALU()
{


}

/*****************************************************************************
-Fuction		: NALU
-Description	: ~NALU
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
NALU :: ~NALU()
{


}

/*****************************************************************************
-Fuction		: Packet
-Description	: Packet
-Input			: i_ptParam i_pbNaluBuf i_iNaluLen
-Output 		: 二维数组o_ppPackets放多包的内容，数组o_aiEveryPacketLen放每包的长度
-Return 		: 包的数目PacketsNum
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int NALU :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio)
{
    int iRet=FALSE;
    int iPackNum=0;
	if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        T_RtpHeader tRtpHeader={0};
        RtpPacket :: GenerateRtpHeader(i_ptParam,&tRtpHeader);
        memcpy(o_ppPackets[iPackNum],&tRtpHeader,sizeof(T_RtpHeader));
        memcpy(o_ppPackets[iPackNum]+sizeof(T_RtpHeader),i_pbNaluBuf,i_iNaluLen);
        o_aiEveryPacketLen[iPackNum]=i_iNaluLen+sizeof(T_RtpHeader);
        iPackNum++;
        iRet=iPackNum;        
    }
    return iRet;
}

const unsigned char FU_A::FU_A_TYPE=28;
/*****************************************************************************
-Fuction		: FU_A
-Description	: FU_A
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
FU_A :: FU_A()
{


}

/*****************************************************************************
-Fuction		: FU_A
-Description	: ~FU_A
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
FU_A :: ~FU_A()
{


}

/*****************************************************************************
-Fuction		: Packet
-Description	: Packet
-Input			: i_ptParam i_pbNaluBuf i_iNaluLen
-Output 		: 二维数组o_ppPackets放多包的内容，数组o_aiEveryPacketLen放每包的长度
-Return 		: 包的数目PacketsNum
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int FU_A :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio)
{
    int iRet=FALSE;
    int iPackNum=0;
    unsigned char *pbNaluBuf=i_pbNaluBuf;
    int iNaluLen=i_iNaluLen;
	if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        T_RtpHeader tRtpHeader={0};
        unsigned char bNaluHeader=i_pbNaluBuf[0];
        int iMark = 0;
        while (iNaluLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
        {
            iMark = 0;
            RtpPacket :: GenerateRtpHeader(i_ptParam,&tRtpHeader);
            memcpy(o_ppPackets[iPackNum],&tRtpHeader,sizeof(T_RtpHeader));
            if (iPackNum == 0) 
            {
                pbNaluBuf ++; //drop nalu header，一个字节的FRT，打包的数据中不包含(原始的)nalu header
                iNaluLen --;
            } 
            else if ((unsigned int)iNaluLen <= RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader)-2) //iNaluLen已经--了
            {//NALU Payload数据在1字节的FU indicator  和1字节的   FU header后面
                iMark = 1;//最后一包
            }
            tRtpHeader.Mark= iMark;
            
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader & 0xe0) | FU_A_TYPE;//FU indicator,低5位
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = (bNaluHeader & 0x1f);//FU indicator，高3位:S E R ,R: 1 bit 保留位必须设置为0
            if (iPackNum == 0) 
            {
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] |= 0x80; //S: 1 bit 当设置成1,开始位指示分片NAL单元的开始
            }
            
            if (iMark) 
            {
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] |= 0x40; //E: 1 bit 当设置成1, 结束位指示分片NAL单元的结束

                memcpy(o_ppPackets[iPackNum] + sizeof(T_RtpHeader) + 2, pbNaluBuf, iNaluLen);
                o_aiEveryPacketLen[iPackNum] = sizeof(T_RtpHeader) + 2 + iNaluLen;
                pbNaluBuf += iNaluLen;
                iNaluLen -= iNaluLen;
            } 
            else 
            {
                memcpy(o_ppPackets[iPackNum] + sizeof(T_RtpHeader) + 2, pbNaluBuf, RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader)-2);
                o_aiEveryPacketLen[iPackNum] = RTP_MAX_PACKET_SIZE;
                pbNaluBuf += RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader) - 2;
                iNaluLen -= RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader) - 2;
            }
            iPackNum++;
        }
        iRet=iPackNum;        
    }
    return iRet;
}

/*****************************************************************************
-Fuction		: RtpPacketG711
-Description	: RtpPacketG711
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketG711 :: RtpPacketG711()
{
    m_pRtpPacketG711 = NULL;

}

/*****************************************************************************
-Fuction		: RtpPacketH264
-Description	: ~RtpPacketH264
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketG711 :: ~RtpPacketG711()
{
    if(m_pRtpPacketG711 != NULL)
        delete m_pRtpPacketG711;

}

/*****************************************************************************
-Fuction		: Packet
-Description	: Packet
-Input			: i_ptParam i_pbNaluBuf i_iNaluLen i_iVideoOrAudio=0 默认视频，1音频
-Output 		: 二维数组o_ppPackets放多包的内容，数组o_aiEveryPacketLen放每包的长度
-Return 		: 包的数目PacketsNum
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int RtpPacketG711 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iVideoOrAudio)
{
    int iRet=FALSE;
    unsigned char *pbFrameBuf=i_pbFrameBuf;
    int iFrameLen=i_iFrameLen;
    int iPackNum=0;
	if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        T_RtpHeader tRtpHeader={0};

        while (iFrameLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
        {
            RtpPacket :: GenerateRtpHeader(i_ptParam,&tRtpHeader);
            tRtpHeader.Mark=(i_ptParam->wSeq-1==0);
            
            memcpy(o_ppPackets[iPackNum],&tRtpHeader,sizeof(T_RtpHeader));
            if ((unsigned int)iFrameLen <= RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader)) 
            {
                memcpy(o_ppPackets[iPackNum]+sizeof(T_RtpHeader),pbFrameBuf,iFrameLen);
                o_aiEveryPacketLen[iPackNum]=iFrameLen+sizeof(T_RtpHeader);
                pbFrameBuf += iFrameLen;
                iFrameLen -= iFrameLen;
            } 
            else 
            {
                memcpy(o_ppPackets[iPackNum]+sizeof(T_RtpHeader),pbFrameBuf, RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader));
                o_aiEveryPacketLen[iPackNum]= RTP_MAX_PACKET_SIZE;
                pbFrameBuf +=  RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader);
                iFrameLen -=  RTP_MAX_PACKET_SIZE - sizeof(T_RtpHeader);
            }
            iPackNum++;
        }
        iRet=iPackNum;        
    }
    return iRet;
}



