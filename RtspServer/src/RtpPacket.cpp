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
#include "RtpPacket.h"

#ifndef TRUE
#define TRUE 0
#endif
#ifndef FALSE
#define FALSE -1
#endif


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
    m_iRtpType = 0;

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
        T_RtpHeader tRtpHeader;
        memset(&tRtpHeader,0,sizeof(T_RtpHeader));
        tRtpHeader.Version=2;
        tRtpHeader.Pad=0;
        tRtpHeader.Extend=0;
        tRtpHeader.CsrcCount=0;
        tRtpHeader.Mark=0;
        tRtpHeader.PayloadType=i_ptParam->wPayloadType;
        tRtpHeader.wSeq=i_ptParam->wSeq++;
        tRtpHeader.dwTimestamp=i_ptParam->dwTimestamp;
        tRtpHeader.dwSSRC=i_ptParam->dwSSRC;

        memcpy(o_ptRtpHeader,&tRtpHeader,sizeof(T_RtpHeader));
        iRet=TRUE;
    }
    return iRet;
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
int RtpPacket :: GenerateRtpHeader(T_RtpPacketParam *i_ptParam,int i_iPaddingLen,int i_iMark,unsigned char *o_bRtpHeader)
{
    int iRet=FALSE;
    if(NULL == i_ptParam || NULL == o_bRtpHeader || RTP_MAX_PACKET_SIZE <= RTP_HEADER_LEN)
    {
        cout<<"GenerateRtpHeader err NULL"<<endl;
    }
    else
    {
        o_bRtpHeader[0] = 0x80 | //v=2
                        (i_iPaddingLen > 0 ? 0x20:0) | //是否有填充
                        0 | 
                        0; //CSRC count = 0
        o_bRtpHeader[1] = (unsigned char)(((i_iMark & 1) << 7) | (i_ptParam->wPayloadType & 0x7f));//1bit markbit, 7 bits pt
        o_bRtpHeader[2] =(unsigned char)(i_ptParam->wSeq >> 8);
        o_bRtpHeader[3] =(unsigned char)(i_ptParam->wSeq & 0xff);
        
        o_bRtpHeader[4] =(unsigned char)(i_ptParam->dwTimestamp >> 24);
        o_bRtpHeader[5] =(unsigned char)(i_ptParam->dwTimestamp >> 16);
        o_bRtpHeader[6] =(unsigned char)(i_ptParam->dwTimestamp >> 8);
        o_bRtpHeader[7] =(unsigned char)(i_ptParam->dwTimestamp & 0xff);

        o_bRtpHeader[8] =(unsigned char)(i_ptParam->dwSSRC >> 24);
        o_bRtpHeader[9] =(unsigned char)(i_ptParam->dwSSRC >> 16);
        o_bRtpHeader[10] =(unsigned char)(i_ptParam->dwSSRC >> 8);
        o_bRtpHeader[11] =(unsigned char)(i_ptParam->dwSSRC & 0xff);
        
        i_ptParam->wSeq ++;

        iRet=TRUE;
    }
    return iRet;
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
int RtpPacket :: Init(unsigned char **m_ppPackets,int i_iMaxPacketNum)
{
    int i=0;
    int iRet=FALSE;
    if(NULL == m_ppPackets)
    {
        cout<<"RtpPacket Init NULL"<<endl;
        return iRet;
    }
    for(i =0;i<i_iMaxPacketNum;i++)
    {
        m_ppPackets[i] = new unsigned char[RTP_MAX_PACKET_SIZE];
        if(NULL == m_ppPackets[i])
        {
            DeInit(m_ppPackets,i_iMaxPacketNum);
            return iRet;
        }
        memset(m_ppPackets[i],0,RTP_MAX_PACKET_SIZE);//初始化放里面，多个nalu分别一起发
    }
    return TRUE;
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
int RtpPacket :: DeInit(unsigned char **m_ppPackets,int i_iMaxPacketNum)
{
    int i=0;
    int iRet=FALSE;
    if(NULL == m_ppPackets)
    {
        cout<<"RtpPacket DeInit NULL"<<endl;
        return iRet;
    }
    for(i =0;i<i_iMaxPacketNum;i++)
    {
        if(NULL != m_ppPackets[i])
        {
            delete [] m_ppPackets[i];
            m_ppPackets[i] = NULL;
        }
    }
    return TRUE;
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
int RtpPacket :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
	if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
        return iRet;
    }
    
    switch(i_iRtpPacketType)
    {
        case RTP_PACKET_TYPE_H264:
        {
            if(NULL != m_pRtpPacket)
            {
                if(m_pRtpPacket->m_iRtpType != i_iRtpPacketType)
                {
                    delete m_pRtpPacket;
                    m_pRtpPacket = NULL;
                    m_pRtpPacket = new RtpPacketH264();
                }
            }
            else
            {
                m_pRtpPacket = new RtpPacketH264();
            }
            break;
        }
        case RTP_PACKET_TYPE_H265:
        {
            if(NULL != m_pRtpPacket)
            {
                if(m_pRtpPacket->m_iRtpType != i_iRtpPacketType)
                {
                    delete m_pRtpPacket;
                    m_pRtpPacket = NULL;
                    m_pRtpPacket = new RtpPacketH265();
                }
            }
            else
            {
                m_pRtpPacket = new RtpPacketH265();
            }
            break;
        }
        case RTP_PACKET_TYPE_AAC:
        {
            if(NULL != m_pRtpPacket)
            {
                if(m_pRtpPacket->m_iRtpType != i_iRtpPacketType)
                {
                    delete m_pRtpPacket;
                    m_pRtpPacket = NULL;
                    m_pRtpPacket = new RtpPacketAAC();
                }
            }
            else
            {
                m_pRtpPacket = new RtpPacketAAC();
            }
            break;
        }
        case RTP_PACKET_TYPE_G711U:
        case RTP_PACKET_TYPE_G711A:
        {
            if(NULL != m_pRtpPacket)
            {
                if(m_pRtpPacket->m_iRtpType != i_iRtpPacketType)
                {
                    delete m_pRtpPacket;
                    m_pRtpPacket = NULL;
                    m_pRtpPacket = new RtpPacketG711();
                }
            }
            else
            {
                m_pRtpPacket = new RtpPacketG711();
            }
            break;
        }
        default:
        {
            cout<<"i_iRtpPacketType err "<<i_iRtpPacketType<<endl;
            break;
        }
    }
    if(NULL == m_pRtpPacket)
    {
        cout<<"i_iRtpPacketType new err "<<i_iRtpPacketType<<endl;
    }
    else
    {
        iRet=m_pRtpPacket->Packet(i_ptParam,i_pbFrameBuf,i_iFrameLen,o_ppPackets,o_aiEveryPacketLen);
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
    m_pRtpPacketNALU = NULL;
    m_pRtpPacketFU_A = NULL;
    m_iRtpType = RTP_PACKET_TYPE_H264;
    m_iRtpVideoType = 0;
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
    if(m_pRtpPacketNALU != NULL)
        delete m_pRtpPacketNALU;
    if(m_pRtpPacketFU_A != NULL)
        delete m_pRtpPacketFU_A;
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
int RtpPacketH264 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
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
        
        if((unsigned int)iNaluLen <=RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader))
        {//单个NAL包单元
            if(NULL == m_pRtpPacketNALU)
            {
                m_pRtpPacketNALU = new H264NALU();
            }
            if(NULL != m_pRtpPacketNALU)
            {
                iRet=m_pRtpPacketNALU->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
            }
        }
        else
        {//分片单元（FU-A）
            if(NULL == m_pRtpPacketFU_A)
            {
                m_pRtpPacketFU_A = new H264FU_A();
            }
            if(NULL != m_pRtpPacketFU_A)
            {
                iRet=m_pRtpPacketFU_A->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
            }
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
H264NALU :: H264NALU()
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
H264NALU :: ~H264NALU()
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
int H264NALU :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    int iPackNum=0;
    int iMark = 0;
    int iPaddingLen = 0;
    
	if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        switch(i_pbNaluBuf[0] & 0x1f)
        {
            case 0x5:
            case 0x1:
                iMark = 1;//i b p 的nalu才表示一帧的结束
                break;
            default:
                break;
        }
        if((i_iNaluLen%4)>0)
        {
            iPaddingLen = 4 -(i_iNaluLen%4);//4字节对齐
        }
        RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
        memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,i_pbNaluBuf,i_iNaluLen);
        o_aiEveryPacketLen[iPackNum]=i_iNaluLen+RTP_HEADER_LEN;
        if(iPaddingLen>0)
        {
            //添加填充位，填充位的最后一位表示填充长度
            o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
            o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
        }
        iPackNum++;
        iRet=iPackNum;        
    }
    return iRet;
}

const unsigned char H264FU_A::FU_A_TYPE=28;
const unsigned char H264FU_A::FU_A_HEADER_LEN=2;
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
H264FU_A :: H264FU_A()
{
    m_iRtpVideoType = FU_A_TYPE;

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
H264FU_A :: ~H264FU_A()
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
int H264FU_A :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    int iPackNum=0;
    unsigned char *pbNaluBuf=i_pbNaluBuf;
    int iNaluLen=i_iNaluLen;
    int iPaddingLen=0;
    
	if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
        return iRet;
    }

    unsigned char bNaluHeader=i_pbNaluBuf[0];
    int iMark = 0;
    while (iNaluLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
    {
        iMark = 0;
        if (iPackNum == 0) 
        {
            pbNaluBuf ++; //drop nalu header，一个字节的FRT，打包的数据中不包含(原始的)nalu header
            iNaluLen --;
        } 
        else if ((unsigned int)iNaluLen <= RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader)-FU_A_HEADER_LEN) //iNaluLen已经--了
        {//NALU Payload数据在1字节的FU indicator  和1字节的   FU header后面
            iMark = 1;//最后一包
        }

        if (iPackNum == 0) 
        {
            RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader & 0xe0) | FU_A_TYPE;//FU indicator,低5位
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = (bNaluHeader & 0x1f);//FU indicator，高3位:S E R ,R: 1 bit 保留位必须设置为0
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] |= 0x80; //S: 1 bit 当设置成1,开始位指示分片NAL单元的开始
            memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN + FU_A_HEADER_LEN, pbNaluBuf, RTP_MAX_PACKET_SIZE- RTP_HEADER_LEN-FU_A_HEADER_LEN);
            o_aiEveryPacketLen[iPackNum] = RTP_MAX_PACKET_SIZE;

            pbNaluBuf += RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
            iNaluLen -= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
        }
        else
        {
            if (iMark) 
            {
                if(((RTP_HEADER_LEN+FU_A_HEADER_LEN+iNaluLen)%4)>0)
                {
                    iPaddingLen = 4 -((RTP_HEADER_LEN+FU_A_HEADER_LEN+iNaluLen)%4);//4字节对齐
                }
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader & 0x60) | FU_A_TYPE;//FU indicator,低5位
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = (bNaluHeader & 0x1f);//FU indicator，高3位:S E R ,R: 1 bit 保留位必须设置为0
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] |= 0x40; //E: 1 bit 当设置成1, 结束位指示分片NAL单元的结束
            
                memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN + FU_A_HEADER_LEN, pbNaluBuf, iNaluLen);
                o_aiEveryPacketLen[iPackNum] = RTP_HEADER_LEN+ FU_A_HEADER_LEN + iNaluLen;
                if(iPaddingLen>0)
                {
                    o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
                    o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
                }
                pbNaluBuf += iNaluLen;
                iNaluLen -= iNaluLen;
            } 
            else 
            {
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader & 0x60) | FU_A_TYPE;//FU indicator,低5位
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = (bNaluHeader & 0x1f);//FU indicator，高3位:S E R ,R: 1 bit 保留位必须设置为0
                memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN +FU_A_HEADER_LEN, pbNaluBuf, RTP_MAX_PACKET_SIZE- RTP_HEADER_LEN-FU_A_HEADER_LEN);
                o_aiEveryPacketLen[iPackNum] = RTP_MAX_PACKET_SIZE;
                
                pbNaluBuf += RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN -FU_A_HEADER_LEN;
                iNaluLen -= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
            }
        }
        iPackNum++;
    }
    iRet=iPackNum;        

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
RtpPacketH265 :: RtpPacketH265()
{
    m_pRtpPacketNALU = NULL;
    m_pRtpPacketFU_A = NULL;
    m_iRtpType = RTP_PACKET_TYPE_H265;
    m_iRtpVideoType = 0;
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
RtpPacketH265 :: ~RtpPacketH265()
{
    if(m_pRtpPacketNALU != NULL)
        delete m_pRtpPacketNALU;
    if(m_pRtpPacketFU_A != NULL)
        delete m_pRtpPacketFU_A;
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
int RtpPacketH265 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
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
        if (pbNaluBuf[0] == 0 && pbNaluBuf[1] == 0 && pbNaluBuf[2] == 0 && pbNaluBuf[3] == 1) 
        {
            pbNaluBuf   += 4;
            iNaluLen    -= 4;
        }
        
        if((unsigned int)iNaluLen <=RTP_MAX_PACKET_SIZE- sizeof(T_RtpHeader))
        {//单个NAL包单元
            if(NULL == m_pRtpPacketNALU)
            {
                m_pRtpPacketNALU = new H265NALU();
            }
            if(NULL != m_pRtpPacketNALU)
            {
                iRet=m_pRtpPacketNALU->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
            }
        }
        else
        {//分片单元（FU-A）
            if(NULL == m_pRtpPacketFU_A)
            {
                m_pRtpPacketFU_A = new H265FU_A();
            }
            if(NULL != m_pRtpPacketFU_A)
            {
                iRet=m_pRtpPacketFU_A->Packet(i_ptParam,pbNaluBuf,iNaluLen,o_ppPackets,o_aiEveryPacketLen);
            }
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
H265NALU :: H265NALU()
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
H265NALU :: ~H265NALU()
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
int H265NALU :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    int iPackNum=0;
    int iMark = 0;
    int iPaddingLen = 0;
    unsigned char bNaluType = 0;
    
    if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        bNaluType = (i_pbNaluBuf[0] & 0x7E)>>1;//取nalu类型
        if(bNaluType >= 0 && bNaluType <= 9)// p slice 片
        {
            iMark = 1;//i p b nalu才表示一帧的结束
        }
        else if(bNaluType >= 16 && bNaluType <= 21)// IRAP 等同于i帧
        {
            iMark = 1;//i p b nalu才表示一帧的结束
        }
        
        if((i_iNaluLen%4)>0)
        {
            iPaddingLen = 4 -(i_iNaluLen%4);//4字节对齐
        }
        RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
        memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,i_pbNaluBuf,i_iNaluLen);
        o_aiEveryPacketLen[iPackNum]=i_iNaluLen+RTP_HEADER_LEN;
        if(iPaddingLen>0)
        {
            //添加填充位，填充位的最后一位表示填充长度
            o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
            o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
        }
        iPackNum++;
        iRet=iPackNum;        
    }
    return iRet;
}


const unsigned char H265FU_A::FU_A_TYPE=49;
const unsigned char H265FU_A::FU_A_HEADER_LEN=3;
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
H265FU_A :: H265FU_A()
{
    m_iRtpVideoType = FU_A_TYPE;

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
H265FU_A :: ~H265FU_A()
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
int H265FU_A :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbNaluBuf,int i_iNaluLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    int iPackNum=0;
    unsigned char *pbNaluBuf=i_pbNaluBuf;
    int iNaluLen=i_iNaluLen;
    int iPaddingLen=0;
    
    if (!i_pbNaluBuf || i_iNaluLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
        return iRet;
    }

    unsigned char bNaluHeader1=i_pbNaluBuf[0];
    unsigned char bNaluHeader2=i_pbNaluBuf[1];
    int iMark = 0;
    while (iNaluLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
    {
        iMark = 0;
        if (iPackNum == 0) 
        {
            pbNaluBuf +=2; //drop nalu header，两字节，打包的数据中不包含(原始的)nalu header
            iNaluLen -=2;
        } 
        else if ((unsigned int)iNaluLen <= RTP_MAX_PACKET_SIZE- RTP_HEADER_LEN-FU_A_HEADER_LEN) //iNaluLen已经--了
        {//NALU Payload数据在1字节的FU indicator  和1字节的   FU header后面
            iMark = 1;//最后一包
        }

        if (iPackNum == 0) 
        {
            RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader1 & 0x81) | (FU_A_TYPE<<1);//修改type值为49(& 0x81先清掉原有的占位数据)
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = bNaluHeader2;//不变
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 2] = (bNaluHeader1>>1) & 0x3F; //nalu type
            o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 2] |= 0x80; //S: 1 bit 当设置成1,开始位指示分片NAL单元的开始
            memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN + FU_A_HEADER_LEN, pbNaluBuf, RTP_MAX_PACKET_SIZE- RTP_HEADER_LEN-FU_A_HEADER_LEN);
            o_aiEveryPacketLen[iPackNum] = RTP_MAX_PACKET_SIZE;

            pbNaluBuf += RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
            iNaluLen -= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
        }
        else
        {
            if (iMark) 
            {
                if(((RTP_HEADER_LEN+FU_A_HEADER_LEN+iNaluLen)%4)>0)
                {
                    iPaddingLen = 4 -((RTP_HEADER_LEN+FU_A_HEADER_LEN+iNaluLen)%4);//4字节对齐
                }
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader1 & 0x81) | (FU_A_TYPE<<1);//修改type值为49(& 0x81先清掉原有的占位数据)
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = bNaluHeader2;//不变
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 2] = (bNaluHeader1>>1) & 0x3F; //nalu type
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 2] |= 0x40; //E: 1 bit 当设置成1, 结束位指示分片NAL单元的结束
            
                memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN + FU_A_HEADER_LEN, pbNaluBuf, iNaluLen);
                o_aiEveryPacketLen[iPackNum] = RTP_HEADER_LEN+ FU_A_HEADER_LEN + iNaluLen;
                if(iPaddingLen>0)
                {
                    o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
                    o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
                }
                pbNaluBuf += iNaluLen;
                iNaluLen -= iNaluLen;
            } 
            else 
            {
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 0] = (bNaluHeader1 & 0x81) | (FU_A_TYPE<<1);//修改type值为49(& 0x81先清掉原有的占位数据)
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 1] = bNaluHeader2;//不变
                o_ppPackets[iPackNum][sizeof(T_RtpHeader) + 2] = (bNaluHeader1>>1) & 0x3F; //nalu type
                memcpy(o_ppPackets[iPackNum] + RTP_HEADER_LEN + FU_A_HEADER_LEN, pbNaluBuf, RTP_MAX_PACKET_SIZE- RTP_HEADER_LEN-FU_A_HEADER_LEN);
                o_aiEveryPacketLen[iPackNum] = RTP_MAX_PACKET_SIZE;
                
                pbNaluBuf += RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
                iNaluLen -= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN - FU_A_HEADER_LEN;
            }
        }
        iPackNum++;
    }
    iRet=iPackNum;        

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
    m_iRtpType = RTP_PACKET_TYPE_G711U;
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
int RtpPacketG711 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    unsigned char *pbFrameBuf=i_pbFrameBuf;
    int iFrameLen=i_iFrameLen;
    int iPackNum=0;
    int iMark = 0;
    int iPaddingLen = 0;
    
	if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        while (iFrameLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
        {
            if ((unsigned int)iFrameLen <= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN) 
            {
                iMark = 1;
                if(((iFrameLen+RTP_HEADER_LEN)%4)>0)
                {
                    iPaddingLen = 4 -((iFrameLen+RTP_HEADER_LEN)%4);//4字节对齐
                }
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,pbFrameBuf,iFrameLen);
                o_aiEveryPacketLen[iPackNum]=iFrameLen+RTP_HEADER_LEN;
                if(iPaddingLen>0)
                {
                    //添加填充位，填充位最后一位表示填充字段的长度
                    o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
                    o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
                }
                pbFrameBuf += iFrameLen;
                iFrameLen -= iFrameLen;
            } 
            else 
            {
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,pbFrameBuf, RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN);
                o_aiEveryPacketLen[iPackNum]= RTP_MAX_PACKET_SIZE;
                pbFrameBuf +=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
                iFrameLen -=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
            }
            iPackNum++;
        }
        iRet=iPackNum;        
    }
    return iRet;
}
/*****************************************************************************
-Fuction		: RtpPacketG726
-Description	: RtpPacketG726
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketG726 :: RtpPacketG726()
{
    m_pRtpPacketG726 = NULL;
    m_iRtpType = RTP_PACKET_TYPE_G726;

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
RtpPacketG726 :: ~RtpPacketG726()
{
    if(m_pRtpPacketG726 != NULL)
        delete m_pRtpPacketG726;
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
int RtpPacketG726 :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    unsigned char *pbFrameBuf=i_pbFrameBuf;
    int iFrameLen=i_iFrameLen;
    int iPackNum=0;
    int iMark = 0;
    int iPaddingLen = 0;
    
    if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
    }
    else
    {
        while (iFrameLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
        {
            if ((unsigned int)iFrameLen <= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN) 
            {
                iMark = 1;
                if(((iFrameLen+RTP_HEADER_LEN)%4)>0)
                {
                    iPaddingLen = 4 -((iFrameLen+RTP_HEADER_LEN)%4);//4字节对齐
                }
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,pbFrameBuf,iFrameLen);
                o_aiEveryPacketLen[iPackNum]=iFrameLen+RTP_HEADER_LEN;
                if(iPaddingLen>0)
                {
                    //添加填充位，填充位最后一位表示填充字段的长度
                    o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
                    o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
                }
                pbFrameBuf += iFrameLen;
                iFrameLen -= iFrameLen;
            } 
            else 
            {
                RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
                memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN,pbFrameBuf, RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN);
                o_aiEveryPacketLen[iPackNum]= RTP_MAX_PACKET_SIZE;
                pbFrameBuf +=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
                iFrameLen -=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
            }
            iPackNum++;
        }
        iRet=iPackNum;        
    }
    return iRet;
}

/*****************************************************************************
-Fuction		: RtpPacketAAC
-Description	: RtpPacketAAC
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketAAC :: RtpPacketAAC()
{
    m_pRtpPacketAAC = NULL;
    m_iRtpType = RTP_PACKET_TYPE_AAC;
}

/*****************************************************************************
-Fuction		: RtpPacketAAC
-Description	: ~RtpPacketAAC
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
RtpPacketAAC :: ~RtpPacketAAC()
{
    if(m_pRtpPacketAAC != NULL)
        delete m_pRtpPacketAAC;

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
int RtpPacketAAC :: Packet(T_RtpPacketParam *i_ptParam,unsigned char *i_pbFrameBuf,int i_iFrameLen,unsigned char **o_ppPackets,int *o_aiEveryPacketLen,int i_iRtpPacketType)
{
    int iRet=FALSE;
    unsigned char *pbFrameBuf=i_pbFrameBuf;
    int iFrameLen=i_iFrameLen;
    int iPackNum=0;
    int iMark = 0;
    int iPaddingLen = 0;
    
    if (!i_pbFrameBuf || i_iFrameLen <= 0 || !o_ppPackets || !o_aiEveryPacketLen)
    {
        cout<<"Packet err NULL"<<endl;
        return iRet;
    }
    
    pbFrameBuf += 7;//aac 要偏移7字节头才是数据
    iFrameLen-=7;
    while (iFrameLen> 0 && iPackNum < RTP_MAX_PACKET_NUM) 
    {
        if ((unsigned int)iFrameLen <= RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN-4) 
        {
            iMark = 1;
            if(((iFrameLen+RTP_HEADER_LEN+4)%4)>0)
            {
                iPaddingLen = 4 -((iFrameLen+RTP_HEADER_LEN+4)%4);//4字节对齐
            }
            RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
            o_ppPackets[iPackNum][RTP_HEADER_LEN] = 0x00;
            o_ppPackets[iPackNum][RTP_HEADER_LEN+1] = 0x10;
            o_ppPackets[iPackNum][RTP_HEADER_LEN+2] = (unsigned char)((iFrameLen & 0x1FE0)>>5);//取长度的高8位
            o_ppPackets[iPackNum][RTP_HEADER_LEN+3] = (unsigned char)((iFrameLen & 0x1F)<<3);//取长度的低5位。高13位是aac data的长度
            
            memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN+4,pbFrameBuf,iFrameLen);
            o_aiEveryPacketLen[iPackNum]=iFrameLen+RTP_HEADER_LEN+4;
            if(iPaddingLen>0)
            {
                //添加填充位，填充位最后一位表示填充字段的长度
                o_aiEveryPacketLen[iPackNum]+=iPaddingLen;
                o_ppPackets[iPackNum][o_aiEveryPacketLen[iPackNum]-1] = iPaddingLen;
            }
            pbFrameBuf += iFrameLen;
            iFrameLen -= iFrameLen;
        } 
        else 
        {
            RtpPacket :: GenerateRtpHeader(i_ptParam,iPaddingLen,iMark,o_ppPackets[iPackNum]);
            o_ppPackets[iPackNum][RTP_HEADER_LEN] = 0x00;
            o_ppPackets[iPackNum][RTP_HEADER_LEN+1] = 0x10;
            o_ppPackets[iPackNum][RTP_HEADER_LEN+2] = (unsigned char)((iFrameLen & 0x1FE0)>>5);//取长度的高8位
            o_ppPackets[iPackNum][RTP_HEADER_LEN+3] = (unsigned char)((iFrameLen & 0x1F)<<3);//取长度的低5位。高13位是aac data的长度
            
            memcpy(o_ppPackets[iPackNum]+RTP_HEADER_LEN+4,pbFrameBuf, RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN-4);
            o_aiEveryPacketLen[iPackNum]= RTP_MAX_PACKET_SIZE;
            pbFrameBuf +=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
            iFrameLen -=  RTP_MAX_PACKET_SIZE - RTP_HEADER_LEN;
        }
        iPackNum++;
    }
    iRet=iPackNum;        

    return iRet;
}




