/*****************************************************************************
* Copyright (C) 2017-2018 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	RawAudioHandle.cpp
* Description		: 	RawAudioHandle operation center
* Created			: 	2017.09.21.
* Author			: 	Yu Weifeng
* Function List		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include <string.h>
#include <iostream>
#include "Definition.h"
#include "RawAudioHandle.h"


using std::cout;//需要<iostream>
using std::endl;

#define AUDIO_G711_SAMPLE_RATE 8000
#define AUDIO_G711_A_FRAME_SAMPLE_POINT_NUM 320

char * G711Handle::m_strAudioFormatName = (char *)AUDIO_ENC_FORMAT_G711_NAME;
int G711Handle::m_iAudioFixLen = AUDIO_G711_A_FRAME_SAMPLE_POINT_NUM;//G711中1B就是一个样本数据

/*****************************************************************************
-Fuction		: G711Handle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
G711Handle::G711Handle()
{
	memset(&m_tMediaInfo,0,sizeof(T_MediaInfo));
}
/*****************************************************************************
-Fuction		: ~G711Handle
-Description	: ~G711Handle
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
G711Handle::~G711Handle()
{

}

/*****************************************************************************
-Fuction		: AudioHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int G711Handle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    m_tMediaInfo.dwAudioSampleRate = AUDIO_G711_SAMPLE_RATE;
    m_tMediaInfo.eAudioEncType = AUDIO_ENCODE_TYPE_G711U;
    m_tMediaInfo.eStreamType = STREAM_TYPE_AUDIO_STREAM;
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
int G711Handle::GetMediaInfo(T_MediaInfo *o_ptMediaInfo)
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
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int G711Handle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
	
	if(m_ptMediaFrameParam == NULL ||m_ptMediaFrameParam->iFrameBufLen < G711Handle::m_iAudioFixLen)
	{
        cout<<"G711Handle GetNextFrame err:"<<m_ptMediaFrameParam->iFrameBufLen<<endl;
        return iRet;
    }
    m_ptMediaFrameParam->pbFrameStartPos = m_ptMediaFrameParam->pbFrameBuf;
    m_ptMediaFrameParam->iFrameLen = m_iAudioFixLen;
	if(NULL != m_ptMediaFrameParam->pbFrameStartPos)
	{
        m_ptMediaFrameParam->iFrameProcessedLen = m_ptMediaFrameParam->pbFrameStartPos - m_ptMediaFrameParam->pbFrameBuf + m_ptMediaFrameParam->iFrameLen;
        m_ptMediaFrameParam->eFrameType = FRAME_TYPE_AUDIO_FRAME;
        m_ptMediaFrameParam->dwTimeStamp += AUDIO_G711_A_FRAME_SAMPLE_POINT_NUM;
        iRet = TRUE;
	}
	return iRet;
}

#define AUDIO_AAC_DEFAULT_SAMPLE_RATE 0
#define AUDIO_AAC_A_FRAME_SAMPLE_POINT_NUM 1024

typedef struct AdtsFixedHeader
{
    unsigned short Sync:12;//总是0xFFF, 代表一个ADTS帧的开始, 用于同步
    unsigned short ID:1;//MPEG Version: 0 for MPEG-4，1 for MPEG-2
    unsigned short Layer:2;//always: ‘00’
    unsigned short ProtectionAbsent:1;//Warning, set to 1 if there is no CRC and 0 if there is CRC

    unsigned short Profile:2;//表示使用哪个级别的AAC，如01 Low Complexity(LC) – AAC LC
    unsigned short SamplingFrequencyIndex:4;//采样率下标值
    unsigned short PrivateBit:1;
    unsigned short ChannelConfig:3;
    unsigned short OriginalCopy:1;
    unsigned short Home:1;
    unsigned short Res:4;
}T_AdtsFixedHeader;
typedef struct AdtsVariableHeader
{
    unsigned int CopyrightIdentificationBit:1;
    unsigned int CopyrightIdentificationStart:1;
    unsigned int AACFrameLength:13;//一个ADTS帧的长度包括ADTS头和AAC原始流
    unsigned int AdtsBufferFullness:11;//0x7FF 说明是码率可变的码流
    unsigned int NumberOfRawDataBlocksInFrame:2;//表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧
    unsigned int Res:4;//
}T_AdtsVariableHeader;
typedef struct AACHeader
{
    T_AdtsFixedHeader tAdtsFixedHeader;
    T_AdtsVariableHeader tAdtsVariableHeader;
}T_AACHeader;

typedef struct IndexValue
{
    int iIndex;
    int iValue;
}T_IndexValue;

static T_IndexValue g_atAACSamplingFreqIndexValue[] ={
        {0x00,96000},
        {0x01,88200},
        {0x02,64000},
        {0x03,48000},
        {0x04,44100},
        {0x05,32000},
        {0x06,24000},
        {0x07,22050},
        {0x08,16000},
        {0x09,12000},
        {0x0a,11025},
        {0x0b,8000},
        {0x0c,7350}
};
static int g_aiAACSamplingFreqIndexValue[] ={96000,
88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350};

char * AACHandle::m_strAudioFormatName = (char *)AUDIO_ENC_FORMAT_AAC_NAME;
/*****************************************************************************
-Fuction		: AACHandle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
AACHandle::AACHandle()
{
	memset(&m_tMediaInfo,0,sizeof(T_MediaInfo));
}

/*****************************************************************************
-Fuction		: AACHandle
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
AACHandle::~AACHandle()
{

}

/*****************************************************************************
-Fuction		: AACHandle::Init
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int AACHandle::Init(char *i_strPath)
{
    int iRet=FALSE;
    if(NULL == i_strPath)
    {
        cout<<"Init NULL"<<endl;
        return iRet;
    }
    m_tMediaInfo.dwAudioSampleRate = AUDIO_AAC_DEFAULT_SAMPLE_RATE;
    m_tMediaInfo.eAudioEncType = AUDIO_ENCODE_TYPE_AAC;
    m_tMediaInfo.eStreamType = STREAM_TYPE_AUDIO_STREAM;
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
int AACHandle::GetMediaInfo(T_MediaInfo *o_ptMediaInfo)
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
-Fuction		: VideoHandle::GetNextVideoFrame
-Description	: 
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int AACHandle::GetNextFrame(T_MediaFrameParam *m_ptMediaFrameParam)
{
    int iRet=FALSE;
    int iFramMark = 0;
    unsigned char *pcFrameStartPos = NULL;
    unsigned char *pcFrameData = NULL;
    int iRemainDataLen = 0;
    int iSampleRateIndex = 0;
    
    if(m_ptMediaFrameParam == NULL ||m_ptMediaFrameParam->iFrameBufLen <= 7)
    {
        cout<<"GetNextFrame err:"<<m_ptMediaFrameParam->iFrameBufLen<<endl;
        return iRet;
    }
    
    pcFrameData = m_ptMediaFrameParam->pbFrameBuf;
    iRemainDataLen = m_ptMediaFrameParam->iFrameBufLen;
    m_ptMediaFrameParam->iFrameLen = 0;
    while(iRemainDataLen > 0)
    {
        if (iRemainDataLen >= 7 && pcFrameData[0] == 0xFF && ((pcFrameData[1] & 0xF0) == 0xF0))
        {
            pcFrameStartPos = pcFrameData;
            iFramMark = 1;
            iSampleRateIndex = (int)((pcFrameStartPos[2]&0x3C)>>2);
            m_ptMediaFrameParam->pbFrameStartPos = pcFrameStartPos;
            m_ptMediaFrameParam->iFrameLen = (int)((pcFrameStartPos[3]&0x03)<<11|pcFrameStartPos[4]<<3|(pcFrameStartPos[5]&0xE0)>>5);
            m_ptMediaFrameParam->eFrameType = FRAME_TYPE_AUDIO_FRAME;
            if(iSampleRateIndex>=0&&iSampleRateIndex<sizeof(g_aiAACSamplingFreqIndexValue)/sizeof(int));
                m_tMediaInfo.dwAudioSampleRate = g_aiAACSamplingFreqIndexValue[iSampleRateIndex];
            if(0 != iFramMark)
            {
                //AAC定义每1024个采样点为1帧
                //采样点的时间单位与时间戳一样
                //因此时间戳的增量为1024(1/采样率)
                m_ptMediaFrameParam->dwTimeStamp += AUDIO_AAC_A_FRAME_SAMPLE_POINT_NUM;
            }
            break;
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


