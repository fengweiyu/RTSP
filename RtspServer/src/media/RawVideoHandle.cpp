/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RawVideoHandle.cpp
* Description		: 	RawVideoHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "Definition.h"
#include "RawVideoHandle.h"
#include <string.h>
#include <iostream>


using std::cout;//需要<iostream>
using std::endl;

#define VIDEO_H264_FRAME_INTERVAL 40
#define VIDEO_H264_SAMPLE_RATE 90000

#define VIDEO_H265_FRAME_INTERVAL 40
#define VIDEO_H265_SAMPLE_RATE 90000

char * H264Handle::m_strVideoFormatName=(char *)VIDEO_ENC_FORMAT_H264_NAME;
/*****************************************************************************
-Fuction		: H264Handle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H264Handle::H264Handle()
{
	memset(&m_tVideoEncodeParam,0,sizeof(T_VideoEncodeParam));
	memset(&m_tMediaInfo,0,sizeof(T_MediaInfo));
}
/*****************************************************************************
-Fuction		: ~H264Handle
-Description	: ~H264Handle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H264Handle::~H264Handle()
{
}


/*****************************************************************************
-Fuction		: VideoHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    m_tMediaInfo.dwVideoSampleRate = VIDEO_H264_SAMPLE_RATE;
    m_tMediaInfo.eVideoEncType = VIDEO_ENCODE_TYPE_H264;
    m_tMediaInfo.eStreamType = STREAM_TYPE_VIDEO_STREAM;
    iRet = TRUE;
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
    int iFramMark = 0;
    unsigned char *pcFrameStartPos = NULL;
    unsigned char *pcNaluStartPos = NULL;
    unsigned char *pcNaluEndPos = NULL;
    unsigned char *pcFrameData = NULL;
	int iRemainDataLen = 0;
    unsigned char bNaluType = 0;
    
	if(m_ptMediaFrameParam == NULL ||m_ptMediaFrameParam->iFrameBufLen <= 4)
	{
        cout<<"GetNextFrame err:"<<m_ptMediaFrameParam->iFrameBufLen<<endl;
        return iRet;
	}
	
	pcFrameData = m_ptMediaFrameParam->pbFrameBuf;
	iRemainDataLen = m_ptMediaFrameParam->iFrameBufLen;
    m_ptMediaFrameParam->dwNaluCount = 0;
    m_ptMediaFrameParam->iFrameLen = 0;
    while(iRemainDataLen > 0)
    {
        if (iRemainDataLen >= 3 && pcFrameData[0] == 0 && pcFrameData[1] == 0 && pcFrameData[2] == 1)
        {
            if(pcNaluStartPos != NULL)
            {
                pcNaluEndPos = pcFrameData;//此时是一个nalu的结束
            }
            else
            {
                pcNaluStartPos = pcFrameData;//此时是一个nalu的开始
                bNaluType = pcNaluStartPos[3] & 0x1f;
            }
            if(pcNaluEndPos != NULL)
            {
                if(pcFrameStartPos == NULL)
                {
                    pcFrameStartPos = pcNaluStartPos;
                    m_ptMediaFrameParam->pbFrameStartPos = pcFrameStartPos;
                }
                m_ptMediaFrameParam->iFrameLen += (pcNaluEndPos - pcNaluStartPos);
                m_ptMediaFrameParam->a_dwNaluEndOffset[m_ptMediaFrameParam->dwNaluCount] = (pcNaluEndPos - pcFrameStartPos);
                m_ptMediaFrameParam->dwNaluCount++;
                if(pcNaluEndPos - pcNaluStartPos > 3)
                {
                    switch(bNaluType)//取nalu类型
                    {
                        case 0x7:
                        {
                            memset(m_tVideoEncodeParam.abSPS,0,sizeof(m_tVideoEncodeParam.abSPS));
                            m_tVideoEncodeParam.iSizeOfSPS = pcNaluEndPos - pcNaluStartPos - 3;//包括类型减3
                            memcpy(m_tVideoEncodeParam.abSPS,pcNaluStartPos+3,m_tVideoEncodeParam.iSizeOfSPS);
                            break;
                        }
                        case 0x8:
                        {
                            memset(m_tVideoEncodeParam.abPPS,0,sizeof(m_tVideoEncodeParam.abPPS));
                            m_tVideoEncodeParam.iSizeOfPPS = pcNaluEndPos - pcNaluStartPos - 3;//包括类型减3
                            memcpy(m_tVideoEncodeParam.abPPS,pcNaluStartPos+3,m_tVideoEncodeParam.iSizeOfPPS);
                            break;
                        }
                        case 0x1:
                        {
                            m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_P_FRAME;
                            iFramMark = 1;
                            break;
                        }
                        case 0x5:
                        {
                            m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_I_FRAME;
                            iFramMark = 1;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                pcNaluStartPos = pcNaluEndPos;//上一个nalu的结束为下一个nalu的开始
                bNaluType = pcNaluStartPos[3] & 0x1f;
                pcNaluEndPos = NULL;
                if(0 != iFramMark)
                {
                    //时间戳的单位是1/VIDEO_H264_SAMPLE_RATE(s),频率的倒数
                    m_ptMediaFrameParam->dwTimeStamp += VIDEO_H264_FRAME_INTERVAL*VIDEO_H264_SAMPLE_RATE/1000;
                    break;
                }
            }
            pcFrameData += 3;
            iRemainDataLen -= 3;
        }
        else if (iRemainDataLen >= 4 && pcFrameData[0] == 0 && pcFrameData[1] == 0 && pcFrameData[2] == 0 && pcFrameData[3] == 1)
        {
            if(pcNaluStartPos != NULL)
            {
                pcNaluEndPos = pcFrameData;
            }
            else
            {
                pcNaluStartPos = pcFrameData;
                bNaluType = pcNaluStartPos[4] & 0x1f;
            }
            if(pcNaluEndPos != NULL)
            {
                if(pcFrameStartPos == NULL)
                {
                    pcFrameStartPos = pcNaluStartPos;
                    m_ptMediaFrameParam->pbFrameStartPos = pcFrameStartPos;
                }
                m_ptMediaFrameParam->iFrameLen += (pcNaluEndPos - pcNaluStartPos);
                m_ptMediaFrameParam->a_dwNaluEndOffset[m_ptMediaFrameParam->dwNaluCount] = (pcNaluEndPos - pcFrameStartPos);
                m_ptMediaFrameParam->dwNaluCount++;
                if(pcNaluEndPos - pcNaluStartPos > 4)
                {
                    switch(bNaluType)//取nalu类型
                    {
                        case 0x7:
                        {
                            memset(m_tVideoEncodeParam.abSPS,0,sizeof(m_tVideoEncodeParam.abSPS));
                            m_tVideoEncodeParam.iSizeOfSPS = pcNaluEndPos - pcNaluStartPos - 4;//包括类型减4开始码
                            memcpy(m_tVideoEncodeParam.abSPS,pcNaluStartPos+4,m_tVideoEncodeParam.iSizeOfSPS);
                            break;
                        }
                        case 0x8:
                        {
                            memset(m_tVideoEncodeParam.abPPS,0,sizeof(m_tVideoEncodeParam.abPPS));
                            m_tVideoEncodeParam.iSizeOfPPS = pcNaluEndPos - pcNaluStartPos - 4;//包括类型减4
                            memcpy(m_tVideoEncodeParam.abPPS,pcNaluStartPos+4,m_tVideoEncodeParam.iSizeOfPPS);
                            break;
                        }
                        case 0x1:
                        {
                            m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_P_FRAME;
                            iFramMark = 1;
                            break;
                        }
                        case 0x5:
                        {
                            m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_I_FRAME;
                            iFramMark = 1;//i p b nalu才表示一帧的结束
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                pcNaluStartPos = pcNaluEndPos;
                bNaluType = pcNaluStartPos[4] & 0x1f;
                pcNaluEndPos = NULL;
                if(0 != iFramMark)
                {
                    //时间戳的单位是1/VIDEO_H264_SAMPLE_RATE(s),频率的倒数
                    m_ptMediaFrameParam->dwTimeStamp += VIDEO_H264_FRAME_INTERVAL*VIDEO_H264_SAMPLE_RATE/1000;
                    break;
                }
            }
            pcFrameData += 4;
            iRemainDataLen -= 4;
        }
        else
        {
            pcFrameData ++;
            iRemainDataLen --;
        }
    }
	if(NULL != m_ptMediaFrameParam->pbFrameStartPos)
	{
        m_ptMediaFrameParam->iFrameProcessedLen += m_ptMediaFrameParam->pbFrameStartPos - m_ptMediaFrameParam->pbFrameBuf + m_ptMediaFrameParam->iFrameLen;
	}
    if(0 != iFramMark)
    {
        iRet = TRUE;
    }

	return iRet;
}
/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam)
{
    int iRet=FALSE;

	if(o_ptVideoEncodeParam == NULL)
	{
        cout<<"GetVideoEncParam NULL"<<endl;
        return iRet;
	}

    memcpy(o_ptVideoEncodeParam,&m_tVideoEncodeParam,sizeof(T_VideoEncodeParam));
    return TRUE;
}
/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::GetMediaInfo(T_MediaInfo *o_ptMediaInfo)
{
    int iRet=FALSE;

	if(o_ptMediaInfo == NULL)
	{
        cout<<"GetMediaInfo NULL"<<endl;
        return iRet;
	}

    memcpy(o_ptMediaInfo,&m_tMediaInfo,sizeof(T_MediaInfo));
    return TRUE;
}


/*****************************************************************************
-Fuction		: VideoHandle::RemoveH264EmulationBytes
-Description	: 去掉h264中防止竞争的字节（脱壳操作）
-Input			: i_pbNaluBuf i_iNaluLen i_iMaxNaluBufLen
-Output 		: o_pbNaluBuf
-Return 		: iNaluLen //返回脱壳操作后的长度
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H264Handle::RemoveH264EmulationBytes(unsigned char *o_pbNaluBuf,int i_iMaxNaluBufLen,unsigned char *i_pbNaluBuf,int i_iNaluLen)
{
    int iNaluLen=0;
    
    int i = 0;
    while (i < i_iNaluLen && iNaluLen+1 < i_iMaxNaluBufLen) 
    {
      if (i+2 < i_iNaluLen && i_pbNaluBuf[i] == 0 && i_pbNaluBuf[i+1] == 0 && i_pbNaluBuf[i+2] == 3) 
      {
        o_pbNaluBuf[iNaluLen] = o_pbNaluBuf[iNaluLen+1] = 0;
        iNaluLen += 2;
        i += 3;
      } 
      else 
      {
        o_pbNaluBuf[iNaluLen] = i_pbNaluBuf[i];
        iNaluLen += 1;
        i += 1;
      }
    }
    
    return iNaluLen;
}
char * H265Handle::m_strVideoFormatName=(char *)VIDEO_ENC_FORMAT_H265_NAME;
/*****************************************************************************
-Fuction		: H264Handle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H265Handle::H265Handle()
{
	memset(&m_tVideoEncodeParam,0,sizeof(T_VideoEncodeParam));
	memset(&m_tMediaInfo,0,sizeof(T_MediaInfo));
	
}
/*****************************************************************************
-Fuction		: ~H264Handle
-Description	: ~H264Handle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
H265Handle::~H265Handle()
{
}
/*****************************************************************************
-Fuction		: VideoHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H265Handle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    iRet = TRUE;
    m_tMediaInfo.dwVideoSampleRate = VIDEO_H265_SAMPLE_RATE;
    m_tMediaInfo.eVideoEncType = VIDEO_ENCODE_TYPE_H265;
    m_tMediaInfo.eStreamType = STREAM_TYPE_VIDEO_STREAM;
	return iRet;
}

/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H265Handle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
    int iFramMark = 0;
    unsigned char *pcFrameStartPos = NULL;
    unsigned char *pcNaluStartPos = NULL;
    unsigned char *pcNaluEndPos = NULL;
    unsigned char *pcFrameData = NULL;
	int iRemainDataLen = 0;
    unsigned char bNaluType = 0;
    
	if(m_ptMediaFrameParam == NULL ||m_ptMediaFrameParam->iFrameBufLen <= 4)
	{
        cout<<"GetNextFrame err:"<<m_ptMediaFrameParam->iFrameBufLen<<endl;
        return iRet;
	}
	
	pcFrameData = m_ptMediaFrameParam->pbFrameBuf;
	iRemainDataLen = m_ptMediaFrameParam->iFrameBufLen;
    m_ptMediaFrameParam->dwNaluCount = 0;
    m_ptMediaFrameParam->iFrameLen = 0;
    while(iRemainDataLen > 0)
    {
        if (iRemainDataLen >= 4 && pcFrameData[0] == 0 && pcFrameData[1] == 0 && pcFrameData[2] == 0 && pcFrameData[3] == 1)
        {
            if(pcNaluStartPos != NULL)
            {
                pcNaluEndPos = pcFrameData;
            }
            else
            {
                pcNaluStartPos = pcFrameData;
                bNaluType = (pcNaluStartPos[4] & 0x7E)>>1;//取nalu类型
            }
            if(pcNaluEndPos != NULL)
            {
                if(pcFrameStartPos == NULL)
                {
                    pcFrameStartPos = pcNaluStartPos;
                    m_ptMediaFrameParam->pbFrameStartPos = pcFrameStartPos;
                }
                m_ptMediaFrameParam->iFrameLen += (pcNaluEndPos - pcNaluStartPos);
                m_ptMediaFrameParam->a_dwNaluEndOffset[m_ptMediaFrameParam->dwNaluCount] = (pcNaluEndPos - pcFrameStartPos);
                m_ptMediaFrameParam->dwNaluCount++;
                if(pcNaluEndPos - pcNaluStartPos > 4)
                {
                    if(bNaluType >= 0 && bNaluType <= 9)// p slice 片
                    {
                        m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_P_FRAME;
                        iFramMark = 1;//i p b nalu才表示一帧的结束
                    }
                    else if(bNaluType >= 16 && bNaluType <= 21)// IRAP 等同于i帧
                    {
                        m_ptMediaFrameParam->eFrameType = FRAME_TYPE_VIDEO_I_FRAME;
                        iFramMark = 1;//i p b nalu才表示一帧的结束
                    }
                    else if(bNaluType == 32)//VPS
                    {
                        memset(m_tVideoEncodeParam.abVPS,0,sizeof(m_tVideoEncodeParam.abVPS));
                        m_tVideoEncodeParam.iSizeOfVPS = pcNaluEndPos - pcNaluStartPos - 4;//包括类型减4
                        memcpy(m_tVideoEncodeParam.abVPS,pcNaluStartPos+4,m_tVideoEncodeParam.iSizeOfVPS);
                    }
                    else if(bNaluType == 33)//SPS
                    {
                        memset(m_tVideoEncodeParam.abSPS,0,sizeof(m_tVideoEncodeParam.abSPS));
                        m_tVideoEncodeParam.iSizeOfSPS = pcNaluEndPos - pcNaluStartPos - 4;//包括类型减4
                        memcpy(m_tVideoEncodeParam.abSPS,pcNaluStartPos+4,m_tVideoEncodeParam.iSizeOfSPS);
                    }
                    else if(bNaluType == 34)//PPS
                    {
                        memset(m_tVideoEncodeParam.abPPS,0,sizeof(m_tVideoEncodeParam.abPPS));
                        m_tVideoEncodeParam.iSizeOfPPS = pcNaluEndPos - pcNaluStartPos - 4;//包括类型减4
                        memcpy(m_tVideoEncodeParam.abPPS,pcNaluStartPos+4,m_tVideoEncodeParam.iSizeOfPPS);
                    }
                }
                pcNaluStartPos = pcNaluEndPos;
                bNaluType = (pcNaluStartPos[4] & 0x7E)>>1;//取nalu类型
                pcNaluEndPos = NULL;
                if(0 != iFramMark)
                {
                    //时间戳的单位是1/VIDEO_H265_SAMPLE_RATE(s),频率的倒数
                    m_ptMediaFrameParam->dwTimeStamp += VIDEO_H265_FRAME_INTERVAL*VIDEO_H265_SAMPLE_RATE/1000;
                    break;
                }
            }
            pcFrameData += 4;
            iRemainDataLen -= 4;
        }
        else
        {
            pcFrameData ++;
            iRemainDataLen --;
        }
    }
	if(NULL != m_ptMediaFrameParam->pbFrameStartPos)
	{
        m_ptMediaFrameParam->iFrameProcessedLen += m_ptMediaFrameParam->pbFrameStartPos - m_ptMediaFrameParam->pbFrameBuf + m_ptMediaFrameParam->iFrameLen;
	}
    if(0 != iFramMark)
    {
        iRet = TRUE;
    }
	return iRet;
}
/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H265Handle::GetVideoEncParam(T_VideoEncodeParam *o_ptVideoEncodeParam)
{
    int iRet=FALSE;

	if(o_ptVideoEncodeParam == NULL)
	{
        cout<<"GetVideoEncParam NULL"<<endl;
        return iRet;
	}

    memcpy(o_ptVideoEncodeParam,&m_tVideoEncodeParam,sizeof(T_VideoEncodeParam));
    return TRUE;
}
/*****************************************************************************
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int H265Handle::GetMediaInfo(T_MediaInfo *o_ptMediaInfo)
{
    int iRet=FALSE;

	if(o_ptMediaInfo == NULL)
	{
        cout<<"GetMediaInfo NULL"<<endl;
        return iRet;
	}

    memcpy(o_ptMediaInfo,&m_tMediaInfo,sizeof(T_MediaInfo));
    return TRUE;
}



