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


using std::cout;//ÐèÒª<iostream>
using std::endl;

#define AUDIO_G711_SAMPLE_RATE 8000


char * G711Handle::m_strAudioFormatName = (char *)AUDIO_ENC_FORMAT_G711_NAME;
int G711Handle::m_iAudioFixLen = AUDIO_BUFFER_G711_FIX_LEN;

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
    m_tMediaInfo.dwVideoSampleRate = AUDIO_G711_SAMPLE_RATE;
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



